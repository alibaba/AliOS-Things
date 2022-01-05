/*
 * This file is part of the MicroPython ESP32 project, https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 LoBo (https://github.com/loboris)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <math.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/rmt.h"
#include "soc/dport_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "libs/neopixel.h"
#include "esp_log.h"

static xSemaphoreHandle neopixel_sem = NULL;
static intr_handle_t rmt_intr_handle = NULL;
static rmt_channel_t RMTchannel = RMT_CHANNEL_0;
static uint16_t neopixel_pos, neopixel_half, neopixel_bufIsDirty, neopixel_termsent;
static uint16_t neopixel_buf_len = 0;
static pixel_settings_t *neopixel_px;
static uint8_t *neopixel_buffer = NULL;
static uint8_t neopixel_brightness = 255;

static uint8_t used_channels[RMT_CHANNEL_MAX] = {0};

// Get color value of RGB component
//---------------------------------------------------
static uint8_t offset_color(char o, uint32_t color) {
	uint8_t clr = 0;
	switch(o) {
		case 'R':
			clr = (uint8_t)(color >> 24);
			break;
		case 'G':
			clr = (uint8_t)(color >> 16);
			break;
		case 'B':
			clr = (uint8_t)(color >> 8);
			break;
		case 'W':
			clr = (uint8_t)(color & 0xFF);
			break;
		default:
			clr = 0;
	}
	return clr;
}

// Set pixel color at buffer position from RGB color value
//=========================================================================
void np_set_pixel_color(pixel_settings_t *px, uint16_t idx, uint32_t color)
{
	uint16_t ofs = idx * (px->nbits / 8);
	px->pixels[ofs] = offset_color(px->color_order[0], color);
	px->pixels[ofs+1] = offset_color(px->color_order[1], color);
	px->pixels[ofs+2] = offset_color(px->color_order[2], color);
	if (px->nbits == 32) px->pixels[ofs+3] = offset_color(px->color_order[3], color);
}

// Set pixel color at buffer position from HSB color value
//============================================================================================================
void np_set_pixel_color_hsb(pixel_settings_t *px, uint16_t idx, float hue, float saturation, float brightness)
{
	uint32_t color = hsb_to_rgb(hue, saturation, brightness);
	np_set_pixel_color(px, idx, color);
}

// Get RGB color value from RGB components corrected by brightness factor
//=============================================================================
uint32_t np_get_pixel_color(pixel_settings_t *px, uint16_t idx, uint8_t *white)
{
	uint32_t clr = 0;
	uint32_t color = 0;
	uint8_t bpp = px->nbits/8;
	uint16_t ofs = idx * bpp;

	for (int i=0; i < bpp; i++) {
		clr = (uint16_t)px->pixels[ofs+i];
		switch(px->color_order[i]) {
			case 'R':
				color |= (uint32_t)clr << 16;
				break;
			case 'G':
				color |= (uint32_t)clr << 8;
				break;
			case 'B':
				color |= (uint32_t)clr;
				break;
			case 'W':
				*white = px->pixels[ofs+i];
				break;
		}
	}
	return color;
}

// Set two levels of RMT output to the Neopixel value for bit value "1".
//--------------------------------------------------------------------
static void neopixel_mark(rmt_item32_t *pItem, pixel_settings_t *px) {
	pItem->level0    = px->timings.mark.level0;
	pItem->duration0 = px->timings.mark.duration0;
	pItem->level1    = px->timings.mark.level1;
	pItem->duration1 = px->timings.mark.duration1;
}

// Set two levels of RMT output to the Neopixel value for bit value "0".
//---------------------------------------------------------------------
static void neopixel_space(rmt_item32_t *pItem, pixel_settings_t *px) {
	pItem->level0    = px->timings.space.level0;
	pItem->duration0 = px->timings.space.duration0;
	pItem->level1    = px->timings.space.level1;
	pItem->duration1 = px->timings.space.duration1;
}

// Set levels and duration of RMT output to the Neopixel value for Reset.
//--------------------------------------------------------------------
static void rmt_terminate(rmt_item32_t *pItem, pixel_settings_t *px) {
	pItem->level0    = px->timings.reset.level0;
	pItem->duration0 = px->timings.reset.duration0;
	pItem->level1    = px->timings.reset.level1;
	pItem->duration1 = px->timings.reset.duration1;
}

// Transfer pixels from buffer to Neopixel strip
//-------------------------------
static void copyToRmtBlock_half()
{
	// This fills half an RMT block
	// When wrap around is happening, we want to keep the inactive half of the RMT block filled
	uint16_t i, offset, len, byteval;
	rmt_item32_t CurrentItem;
	offset = neopixel_half * MAX_PULSES;
	neopixel_half = !neopixel_half;  // for next offset calculation
	int j;

	len = neopixel_buf_len - neopixel_pos; // remaining bytes in buffer
	if (len > (MAX_PULSES / 8)) len = (MAX_PULSES / 8);

	if (!len) {
		if (!neopixel_bufIsDirty) return;
		// Clear the channel's data block and return
		j = 0;
		if (!neopixel_termsent) {
			i++;
			rmt_terminate(&CurrentItem, neopixel_px);
			RMTMEM.chan[RMTchannel].data32[0].val = CurrentItem.val;
			neopixel_termsent = 1;
			j++;
		}
		for (i = j; i < MAX_PULSES; i++) {
			RMTMEM.chan[RMTchannel].data32[i + offset].val = 0;
		}
		neopixel_bufIsDirty = 0;
		return;
	}
	neopixel_bufIsDirty = 1;

	// Populate RMT bit buffer from 'neopixel_buffer' containing one byte for each RGB(W) value
	for (i = 0; i < len; i++) {
		byteval = (uint16_t)neopixel_buffer[i+neopixel_pos];
		// Correct by brightness factor
		byteval = (byteval * neopixel_brightness) / 255;

		// Shift bits out, MSB first, setting RMTMEM.chan[n].data32[x] to the rmtPulsePair value corresponding to the buffered bit value
		for (j=7; j>=0; j--) {
			if (byteval & (1<<j)) neopixel_mark(&CurrentItem, neopixel_px);
			else neopixel_space(&CurrentItem, neopixel_px);

			RMTMEM.chan[RMTchannel].data32[(i * 8) + offset + (7-j)].val = CurrentItem.val;
		}
		if ((i < ((MAX_PULSES / 8)-1)) && (i + neopixel_pos == neopixel_buf_len - 1)) {
			i++;
			rmt_terminate(&CurrentItem, neopixel_px);
			RMTMEM.chan[RMTchannel].data32[(i * 8) + offset + 7].val = CurrentItem.val;
			neopixel_termsent = 1;
			break;
		}
	}
	// Clear the remainder of the channel's data not set above
	for (i *= 8; i < MAX_PULSES; i++) {
		RMTMEM.chan[RMTchannel].data32[i + offset].val = 0;
	}

	neopixel_pos += len;
	return;
}

// RMT interrupt handler
//---------------------------------------------
static void neopixel_handleInterrupt(void *arg)
{
  portBASE_TYPE taskAwoken = 0;

  uint32_t tx_thr_event_mask = 0x01000000 << RMTchannel;
  uint32_t tx_end_event_mask = 1 << (RMTchannel*3);
  uint32_t intr_st = RMT.int_st.val;

  if (intr_st & tx_thr_event_mask) {
    copyToRmtBlock_half();
    RMT.int_clr.val = tx_thr_event_mask;
  }
  else if ((intr_st & tx_end_event_mask) && neopixel_sem) {
    xSemaphoreGiveFromISR(neopixel_sem, &taskAwoken);
    RMT.int_clr.val = tx_end_event_mask;
  }

  return;
}

// Initialize Neopixel RMT interface on specific GPIO
//===================================================
int neopixel_init(int gpioNum, rmt_channel_t channel)
{
	esp_err_t res;
	// Create semaphore if needed
	if (neopixel_sem == NULL) {
		neopixel_sem = xSemaphoreCreateBinary();
		if (neopixel_sem == NULL) return ESP_FAIL;
		// Note: binary semaphores created using xSemaphoreCreateBinary() are created in a state
		// such that the semaphore must first be 'given' before it can be 'taken' !
		xSemaphoreGive(neopixel_sem);

		DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_RMT_CLK_EN);
		DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_RMT_RST);
	}

	// Allocate RMT interrupt if needed
	if (rmt_intr_handle == NULL) {
		res = esp_intr_alloc(ETS_RMT_INTR_SOURCE, 0, neopixel_handleInterrupt, NULL, &rmt_intr_handle);
		if (res != ESP_OK) return res;
	}

	xSemaphoreTake(neopixel_sem, portMAX_DELAY);

	uint32_t tx_thr_event_mask = 0x01000000 << channel;
	uint32_t tx_end_event_mask = 1 << (channel*3);

	res = rmt_set_pin(channel, RMT_MODE_TX, (gpio_num_t)gpioNum);
	if (res != ESP_OK) {
		xSemaphoreGive(neopixel_sem);
		return res;
	}

	RMT.apb_conf.fifo_mask = 1;						//enable memory access, instead of FIFO mode.
	RMT.apb_conf.mem_tx_wrap_en = 1;				//wrap around when hitting end of buffer
	RMT.conf_ch[channel].conf0.div_cnt = DIVIDER;
	RMT.conf_ch[channel].conf0.mem_size = 1;
	RMT.conf_ch[channel].conf0.carrier_en = 0;
	RMT.conf_ch[channel].conf0.carrier_out_lv = 1;
	RMT.conf_ch[channel].conf0.mem_pd = 0;

	RMT.conf_ch[channel].conf1.rx_en = 0;
	RMT.conf_ch[channel].conf1.mem_owner = 0;
	RMT.conf_ch[channel].conf1.tx_conti_mode = 0;	//loop back mode.
	RMT.conf_ch[channel].conf1.ref_always_on = 1;	// use apb clock: 80M
	RMT.conf_ch[channel].conf1.idle_out_en = 1;
	RMT.conf_ch[channel].conf1.idle_out_lv = 0;

	RMT.tx_lim_ch[channel].limit = MAX_PULSES;
	RMT.int_ena.val = tx_thr_event_mask | tx_end_event_mask;

	used_channels[channel] = 1;
	xSemaphoreGive(neopixel_sem);
	return ESP_OK;
}

// Deinitialize RMT interface
//=========================================
void neopixel_deinit(rmt_channel_t channel)
{
	xSemaphoreTake(neopixel_sem, portMAX_DELAY);

	rmt_set_rx_intr_en(channel, 0);
    rmt_set_err_intr_en(channel, 0);
    rmt_set_tx_intr_en(channel, 0);
    rmt_set_tx_thr_intr_en(channel, 0, 0xffff);

	used_channels[channel] = 0;
	uint8_t nused = 0;
	for (int i=0; i<RMT_CHANNEL_MAX; i++ ) {
		if (used_channels[i]) nused++;
	}
	if (nused == 0) {
		// No RMT channels used, cleanup
		if (rmt_intr_handle) {
			esp_intr_free(rmt_intr_handle);
			rmt_intr_handle = NULL;
		}
	    if (neopixel_buffer) {
	    	free(neopixel_buffer);
	    	neopixel_buffer = NULL;
	    }
		xSemaphoreGive(neopixel_sem);
	    vSemaphoreDelete(neopixel_sem);
	    neopixel_sem = NULL;
	}
	else {
		xSemaphoreGive(neopixel_sem);
	}
}

// Start the transfer of Neopixel color bytes from buffer
//=======================================================
void np_show(pixel_settings_t *px, rmt_channel_t channel)
{
	// Wait for previous operation to finish
	xSemaphoreTake(neopixel_sem, portMAX_DELAY);

	RMTchannel = channel;
	// Enable interrupt for neopixel RMT channel
	uint32_t tx_thr_event_mask = 0x01000000 << channel;
	uint32_t tx_end_event_mask = 1 << (channel*3);
	RMT.int_ena.val = tx_thr_event_mask | tx_end_event_mask;

	uint16_t blen = px->pixel_count * (px->nbits / 8);

	// Allocate neopixel buffer if needed
	if (neopixel_buffer == NULL) {
		neopixel_buffer = (uint8_t *)malloc(blen);
		if (neopixel_buffer == NULL) return;
		neopixel_buf_len = blen;
	}
	// Resize neopixel buffer if needed
	if (neopixel_buf_len < blen) {
		// larger buffer needed
		free(neopixel_buffer);
		neopixel_buffer = (uint8_t *)malloc(blen);
		if (neopixel_buffer == NULL) return;
	}
	memcpy(neopixel_buffer, px->pixels, blen);

	neopixel_buf_len = blen;
	neopixel_pos = 0;
	neopixel_half = 0;
	neopixel_px = px;
	neopixel_half = 0;
	neopixel_termsent = 0;
	neopixel_brightness = px->brightness;

	copyToRmtBlock_half();

	if (neopixel_pos < neopixel_buf_len) {
		// Fill the other half of the buffer block
		copyToRmtBlock_half();
	}

	// Start sending
	RMT.conf_ch[RMTchannel].conf1.mem_rd_rst = 1;
	RMT.conf_ch[RMTchannel].conf1.tx_start = 1;

	// Wait for operation to finish
	if (xSemaphoreTake(neopixel_sem, 0) == pdTRUE) {
		xSemaphoreTake(neopixel_sem, portMAX_DELAY);
	}
	xSemaphoreGive(neopixel_sem);
}

// Clear the Neopixel color buffer
//=================================
void np_clear(pixel_settings_t *px)
{
	memset(px->pixels, 0, px->pixel_count * (px->nbits/8));
}

//------------------------------------
static float Min(double a, double b) {
	return a <= b ? a : b;
}

//------------------------------------
static float Max(double a, double b) {
	return a >= b ? a : b;
}

// Convert 24-bit color to HSB representation
//===================================================================
void rgb_to_hsb( uint32_t color, float *hue, float *sat, float *bri )
{
	float delta, min;
	float h = 0, s, v;
	uint8_t red = (color >> 16) & 0xFF;
	uint8_t green = (color >> 8) & 0xFF;
	uint8_t blue = color & 0xFF;

	min = Min(Min(red, green), blue);
	v = Max(Max(red, green), blue);
	delta = v - min;

	if (v == 0.0) s = 0;
	else s = delta / v;

	if (s == 0)	h = 0.0;
	else
	{
		if (red == v)
			h = (green - blue) / delta;
		else if (green == v)
			h = 2 + (blue - red) / delta;
		else if (blue == v)
			h = 4 + (red - green) / delta;

		h *= 60;

		if (h < 0.0) h = h + 360;
	}

	*hue = h;
	*sat = s;
	*bri = v / 255;
}

// Convert HSB color to 24-bit color representation
//============================================================
uint32_t hsb_to_rgb(float _hue, float _sat, float _brightness)
{
	float red = 0.0;
	float green = 0.0;
	float blue = 0.0;

	if (_sat == 0.0) {
		red = _brightness;
		green = _brightness;
		blue = _brightness;
	}
	else {
		if (_hue >= 360.0) _hue = fmod(_hue, 360);

		int slice = (int)(_hue / 60.0);
		float hue_frac = (_hue / 60.0) - slice;

		float aa = _brightness * (1.0 - _sat);
		float bb = _brightness * (1.0 - _sat * hue_frac);
		float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));

		switch(slice) {
			case 0:
				red = _brightness;
				green = cc;
				blue = aa;
				break;
			case 1:
				red = bb;
				green = _brightness;
				blue = aa;
				break;
			case 2:
				red = aa;
				green = _brightness;
				blue = cc;
				break;
			case 3:
				red = aa;
				green = bb;
				blue = _brightness;
				break;
			case 4:
				red = cc;
				green = aa;
				blue = _brightness;
				break;
			case 5:
				red = _brightness;
				green = aa;
				blue = bb;
				break;
			default:
				red = 0.0;
				green = 0.0;
				blue = 0.0;
				break;
		}
	}

	return (uint32_t)((uint8_t)(red * 255.0) << 16) | ((uint8_t)(green * 255.0) << 8) | ((uint8_t)(blue * 255.0));
}

// Convert HSB color to 24-bit color representation
// _hue: 0 ~ 359
// _sat: 0 ~ 255
// _bri: 0 ~ 255
//=======================================================
uint32_t hsb_to_rgb_int(int hue, int sat, int brightness)
{
	float _hue = (float)hue;
	float _sat = (float)((float)sat / 1000.0);
	float _brightness = (float)((float)brightness / 1000.0);
	float red = 0.0;
	float green = 0.0;
	float blue = 0.0;

	if (_sat == 0.0) {
		red = _brightness;
		green = _brightness;
		blue = _brightness;
	}
	else {
		if (_hue >= 360.0) _hue = fmod(_hue, 360);

		int slice = (int)(_hue / 60.0);
		float hue_frac = (_hue / 60.0) - slice;

		float aa = _brightness * (1.0 - _sat);
		float bb = _brightness * (1.0 - _sat * hue_frac);
		float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));

		switch(slice) {
			case 0:
				red = _brightness;
				green = cc;
				blue = aa;
				break;
			case 1:
				red = bb;
				green = _brightness;
				blue = aa;
				break;
			case 2:
				red = aa;
				green = _brightness;
				blue = cc;
				break;
			case 3:
				red = aa;
				green = bb;
				blue = _brightness;
				break;
			case 4:
				red = cc;
				green = aa;
				blue = _brightness;
				break;
			case 5:
				red = _brightness;
				green = aa;
				blue = bb;
				break;
			default:
				red = 0.0;
				green = 0.0;
				blue = 0.0;
				break;
		}
	}

	return (uint32_t)((uint8_t)(red * 255.0) << 16) | ((uint8_t)(green * 255.0) << 8) | ((uint8_t)(blue * 255.0));
}

