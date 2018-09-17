/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// File: iomux_config.c

/* ------------------------------------------------------------------------------
 * <auto-generated>
 *     This code was generated by a tool.
 *     Runtime Version:3.4.0.0
 *
 *     Changes to this file may cause incorrect behavior and will be lost if
 *     the code is regenerated.
 * </auto-generated>
 * ------------------------------------------------------------------------------
*/

#include <iomux_config.h>

// Function to configure iomux for i.MX6SDL board Smart_Device rev. B.
void iomux_config(void)
{
    audmux_iomux_config();
    ccm_iomux_config();
    ecspi1_iomux_config();
    enet_iomux_config();
    epdc_iomux_config();
    flexcan1_iomux_config();
    gpio1_iomux_config();
    gpio2_iomux_config();
    gpio3_iomux_config();
    gpio4_iomux_config();
    gpio5_iomux_config();
    gpio6_iomux_config();
    gpio7_iomux_config();
    i2c1_iomux_config();
    i2c2_iomux_config();
    i2c3_iomux_config();
    ipu1_iomux_config();
    mmdc_iomux_config();
    pwm1_iomux_config();
    sjc_iomux_config();
    uart1_iomux_config();
    usb_iomux_config();
    usdhc2_iomux_config();
    usdhc3_iomux_config();
    usdhc4_iomux_config();
    gps_iomux_config();
}

// Definitions for unused modules.
void arm_iomux_config()
{
};

void asrc_iomux_config()
{
};

void dcic1_iomux_config()
{
};

void dcic2_iomux_config()
{
};

void ecspi2_iomux_config()
{
};

void ecspi3_iomux_config()
{
};

void ecspi4_iomux_config()
{
};

void eim_iomux_config()
{
};

void epit1_iomux_config()
{
};

void epit2_iomux_config()
{
};

void esai_iomux_config()
{
};

void flexcan2_iomux_config()
{
};

void gpmi_iomux_config()
{
};

void gpt_iomux_config()
{
};

void hdmi_iomux_config()
{
};

void i2c4_iomux_config()
{
};

void kpp_iomux_config()
{
};

void lcd_iomux_config()
{
};

void ldb_iomux_config()
{
};

void mipi_csi_iomux_config()
{
};

void mipi_dsi_iomux_config()
{
};

void mipi_hsi_iomux_config()
{
};

void mlb_iomux_config()
{
};

void pcie_iomux_config()
{
};

void pmu_iomux_config()
{
};

void pwm2_iomux_config()
{
};

void pwm3_iomux_config()
{
};

void pwm4_iomux_config()
{
};

void sdma_iomux_config()
{
};

void snvs_iomux_config()
{
};

void spdif_iomux_config()
{
};

void src_iomux_config()
{
};

void uart2_iomux_config()
{
};

void uart3_iomux_config()
{
};

void uart4_iomux_config()
{
};

void uart5_iomux_config()
{
};

void usdhc1_iomux_config()
{
};

void wdog1_iomux_config()
{
};

void wdog2_iomux_config()
{
};

void xtalosc_iomux_config()
{
};
