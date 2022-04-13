//#include "input.h"
#include "input_hal.h"
#include "board.h"
#include "drv_mouse.h"
#include "drv_clcd.h"

#define DBG_TAG "drv.mouse"
#define DBG_LVL DBG_INFO

#define MOUSE_ADDRESS    (0x10007000)
#define MOUSE_IRQ_NUM    (IRQ_VEXPRESS_A9_MOUSE)
#define MOUSE_XMAX       (BSP_LCD_WIDTH)
#define MOUSE_YMAX       (BSP_LCD_HEIGHT)

#define MOUSE_BUTTON_LEFT    (0x01)
#define MOUSE_BUTTON_RIGHT   (0x02)
#define MOUSE_BUTTON_MIDDLE  (0x04)
#define MOUSE_BUTTON_DOWN    (0x10)
#define MOUSE_BUTTON_UP      (0x20)
#define MOUSE_BUTTON_MOVE    (0x40)
#define MOUSE_BUTTON_WHELL   (0x80)

typedef uint32_t virtual_addr_t;

enum {
    MOUSE_CR        = 0x00,
    MOUSE_STAT      = 0x04,
    MOUSE_DATA      = 0x08,
    MOUSE_CLKDIV    = 0x0c,
    MOUSE_IIR       = 0x10,
};

struct mouse_pl050_pdata_t {
    virtual_addr_t virt;
    int irq;
    int xmax, ymax;
    int xpos, ypos;
    unsigned char packet[4];
    unsigned char index;
    int obtn;
    int type;
};
struct mouse_pl050_pdata_t * pdat = NULL;
struct mouse_pl050_pdata_t  g_p1050_dat;

input_obj_t *mouse_input_obj = NULL;

static inline uint8_t read8(uint32_t addr)
{
    return (*((volatile uint8_t *)(addr)));
}

static inline void write8(uint32_t addr, uint8_t value)
{
    *((volatile uint8_t *)(addr)) = value;
}

static inline uint32_t read32(uint32_t addr)
{
    return (*((volatile uint32_t *)(addr)));
}

static inline void write32(uint32_t addr, uint32_t value)
{
    *((volatile uint32_t *)(addr)) = value;
}

static inline int kmi_write(struct mouse_pl050_pdata_t * pdat, uint8_t value)
{
    int timeout = 1000;

    while((read8(pdat->virt + MOUSE_STAT) & (1 << 6)) == 0 && timeout--);

    if(timeout)
    {
        write8(pdat->virt + MOUSE_DATA, value);
        while((read8(pdat->virt + MOUSE_STAT) & (1 << 4)) == 0);

        if(read8(pdat->virt + MOUSE_DATA) == 0xfa)
            return INPUT_TRUE;
    }
    return INPUT_FALSE;
}

static inline int kmi_read(struct mouse_pl050_pdata_t * pdat, uint8_t * value)
{
    if((read8(pdat->virt + MOUSE_STAT) & (1 << 4)))
    {
        *value = read8(pdat->virt + MOUSE_DATA);
        return INPUT_TRUE;
    }
    return INPUT_FALSE;
}

static uint32_t emouse_id;

void push_event_touch_move(int x, int y)
{
    input_report_abs(mouse_input_obj, ABS_X, x);
    input_report_abs(mouse_input_obj, ABS_Y, y);
    //input_report_key(mouse_input_obj, BTN_TOUCH, 0);
    input_sync(mouse_input_obj);
    printf("touch move to x:%d y:%d\n", x, y);
}

void push_event_touch_begin(int x, int y)
{
    input_report_abs(mouse_input_obj, ABS_X, x);
    input_report_abs(mouse_input_obj, ABS_Y, y);
    input_report_key(mouse_input_obj, BTN_TOUCH, 1);
    input_sync(mouse_input_obj);
    printf("touch_down x:%d y:%d\n", x, y);
}

void push_event_touch_end(int x, int y)
{
    input_report_abs(mouse_input_obj, ABS_X, x);
    input_report_abs(mouse_input_obj, ABS_Y, y);
    input_report_key(mouse_input_obj, BTN_TOUCH, 0);
    input_sync(mouse_input_obj);
    printf("touch_up x:%d y:%d\n", x, y);

}

static void mouse_pl050_interrupt(int irq, void * data)
{
    //struct mouse_pl050_pdata_t * pdat = (struct mouse_pl050_pdata_t *)data;
    if(pdat == NULL)
    {
        return;
    }
    int x, y, relx, rely;
    static int last_x = 0, last_y = 0;
    int btndown, btnup, btn;
    int status = 0;

    status = read8(pdat->virt + MOUSE_IIR);

    while(status & (1 << 0))
    {
        //printf("line %d index %d,value %d,enter mouse_pl050_interrupt\n",__LINE__, pdat->index,pdat->packet[pdat->index]);

        pdat->packet[pdat->index] = read8(pdat->virt + MOUSE_DATA);
        pdat->index = (pdat->index + 1) & 0x3;

        if(pdat->index == 0)
        {
            btn = pdat->packet[0] & 0x7;
            btndown = (btn ^ pdat->obtn) & btn;
            btnup = (btn ^ pdat->obtn) & pdat->obtn;
            pdat->obtn = btn;
            //printf("line %d,enter mouse_pl050_interrupt\n",__LINE__);

            if(pdat->packet[0] & 0x10)
                relx = 0xffffff00 | pdat->packet[1];
            else
                relx = pdat->packet[1];

            if(pdat->packet[0] & 0x20)
                rely = 0xffffff00 | pdat->packet[2];
            else
                rely = pdat->packet[2];
            rely = -rely;

            /*delta = pdat->packet[3] & 0xf;
            if(delta == 0xf)
                delta = -1;*/

            if(relx != 0)
            {
                pdat->xpos = pdat->xpos + relx;
                if(pdat->xpos < 0)
                    pdat->xpos = 0;
                if(pdat->xpos > pdat->xmax - 1)
                    pdat->xpos = pdat->xmax - 1;
            }
            if(rely != 0)
            {
                pdat->ypos = pdat->ypos + rely;
                if(pdat->ypos < 0)
                    pdat->ypos = 0;
                if(pdat->ypos > pdat->ymax - 1)
                    pdat->ypos = pdat->ymax - 1;
            }
            x = pdat->xpos;
            y = pdat->ypos;

#if 0
            if((btn & (0x01 << 0)) && ((relx != 0) || (rely != 0)))
                push_event_touch_move(x, y);
#endif
            if(btndown & (0x01 << 0))
                push_event_touch_begin(x, y);
            else if(btnup & (0x01 << 0))
                push_event_touch_end(x, y);
            else {
                if (!((last_x == x) && (last_y == y)))
                    push_event_touch_move(x, y);
                last_x = x;
                last_y = y;
            }
        }

        status = read8(pdat->virt + MOUSE_IIR);
    }
        //printf("exit mouse_pl050_interrupt\n");
}

static input_obj_t *input_obj_create(void)
{
    input_obj_t input_obj;
    input_obj_t *pinput_obj = NULL;

    memset(&input_obj, 0, sizeof(input_obj_t));
    input_obj.tag = TAG_DEV_TOUCHPANEL;
    input_obj.path = dev_touchpanel_path;
    input_obj.mode = DEV_INT;

    pinput_obj = input_create_obj(&input_obj);

    return pinput_obj;
}

int drv_mouse_hw_init(void)
{
    uint8_t value;
    uint32_t id;
    //struct mouse_pl050_pdata_t *pdat;
    virtual_addr_t virt = MOUSE_ADDRESS;
    int irq = MOUSE_IRQ_NUM;

    id = (((read32(virt + 0xfec) & 0xff) << 24) |
                ((read32(virt + 0xfe8) & 0xff) << 16) |
                ((read32(virt + 0xfe4) & 0xff) <<  8) |
                ((read32(virt + 0xfe0) & 0xff) <<  0));

    if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x050)
    {
        printf("read id fail id:0x%08x", id);
         return INPUT_ERROR;
    }
    pdat = &g_p1050_dat;
#if 0
    pdat = malloc(sizeof(struct mouse_pl050_pdata_t));
    if(!pdat)
    {
        printf("malloc memory failed");
        return INPUT_ERROR;
    }
 #endif
    memset(pdat, 0, sizeof(struct mouse_pl050_pdata_t));

    mouse_input_obj = input_obj_create();

    pdat->virt = virt;
    pdat->irq = irq;
    pdat->xmax = MOUSE_XMAX;
    pdat->ymax = MOUSE_YMAX;
    pdat->xpos = pdat->xmax / 2;
    pdat->ypos = pdat->ymax / 2;
    pdat->packet[0] = 0;
    pdat->packet[1] = 0;
    pdat->packet[2] = 0;
    pdat->packet[3] = 0;
    pdat->index = 0;
    pdat->obtn = 0;
    //printf("line %d, index:%d, enter rhw_mouse_init\n",__LINE__,pdat->index);

    write8(pdat->virt + MOUSE_CLKDIV, 0);
    write8(pdat->virt + MOUSE_CR, (1 << 2));
    kmi_write(pdat, 0xff);
    kmi_read(pdat, &value);
    kmi_write(pdat, 0xf3);
    kmi_write(pdat, 200);
    kmi_write(pdat, 0xf3);
    kmi_write(pdat, 100);
    kmi_write(pdat, 0xf3);
    kmi_write(pdat, 80);
    kmi_write(pdat, 0xf2);
    kmi_read(pdat, &value);
    kmi_read(pdat, &value);
    kmi_write(pdat, 0xf3);
    kmi_write(pdat, 100);
    kmi_write(pdat, 0xe8);
    kmi_write(pdat, 0x02);
    kmi_write(pdat, 0xe6);
    kmi_write(pdat, 0xf4);
    kmi_read(pdat, &value);
    kmi_read(pdat, &value);
    kmi_read(pdat, &value);
    kmi_read(pdat, &value);
    write8(pdat->virt + MOUSE_CR, (1 << 2) | (1 << 4));

    printf("hw_mouse_init succesed\n");

    (void)irq_register_isr(pdat->irq, mouse_pl050_interrupt);
    gic_enable_interrupt(pdat->irq);

    return INPUT_OK;
}
