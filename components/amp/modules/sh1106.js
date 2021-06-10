const SET_SCAN_DIR = 0xc0
const LOW_COLUMN_ADDRESS = 0x00
const HIGH_COLUMN_ADDRESS = 0x10
const SET_PAGE_ADDRESS = 0xB0
const SET_CONTRAST = 0x81
const SET_ENTIRE_ON = 0xa4
const SET_NORM_INV = 0xa6
const SET_DISP = 0xae
const SET_MEM_ADDR = 0x20
const SET_DISP_START_LINE = 0x40
const SET_SEG_REMAP = 0xa0
const SET_MUX_RATIO = 0xa8
const SET_COM_OUT_DIR = 0xc0
const SET_DISP_OFFSET = 0xd3
const SET_COM_PIN_CFG = 0xda
const SET_DISP_CLK_DIV = 0xd5
const SET_PRECHARGE = 0xd9
const SET_VCOM_DESEL = 0xdb
const SET_CHARGE_PUMP = 0x8d

export default class SH1106 {
    constructor(width, height, spi, dc, res, cs) {
        this.width = width;
        this.height = height;
        this.spi = spi;
        this.dc = dc;
        this.res = res;
        this.cs = cs;
        this.dc_var = 0;
        this.cs_var = 1;

        this.pages = Math.floor(this.height / 8)
        this.framebuf = new Array(this.pages * this.width).fill(0)
        return this;
    }

    reset() {
        if (this.res != undefined) {
            this.res.writeValue(1)
            sleepMs(1)
            this.res.writeValue(0)
            sleepMs(20)
            this.res.writeValue(1)
            sleepMs(20)
        }
    }

    show() {
        for (let page = 0; page < this.pages; page++) {
            this.write_cmd(SET_PAGE_ADDRESS | page)
            this.write_cmd(LOW_COLUMN_ADDRESS)
            this.write_cmd(HIGH_COLUMN_ADDRESS)
            this.write_data(this.framebuf.slice(this.width * page, this.width * (page + 1)))
        }
    }

    write_cmd(cmd) {
        if (this.dc_var == 1) {
            this.dc.writeValue(0)
            this.dc_var = 0
        }
        if (this.cs != undefined && this.cs_var == 1) {
            this.cs.writeValue(0)
            this.cs_var = 0
        }
        this.spi.write([cmd])
    }

    write_data(buf) {
        if (this.dc_var == 0) {
            this.dc.writeValue(1)
            this.dc_var = 1
        }
        if (this.cs != undefined && this.cs_var == 1) {
            this.cs.writeValue(0)
            this.cs_var = 0
        }
        this.spi.write(buf)
    }

    fill(color) {
        this.framebuf.fill(color == 0 ? 0 : 255)
    }

    open() {
        this.reset()
        this.dc.writeValue(0)
        this.dc_var = 0;
        if (this.cs != undefined) {
            this.cs.writeValue(1)
            this.cs_var = 1;
        }

        this.write_cmd(SET_DISP | 0x00)    // 关闭显示
        this.write_cmd(SET_DISP_CLK_DIV)
        this.write_cmd(0x80) // 设置时钟分频因子
        this.write_cmd(SET_MUX_RATIO)
        this.write_cmd(this.height - 1) // 设置驱动路数 路数默认0x3F（1/64）
        this.write_cmd(SET_DISP_OFFSET)
        this.write_cmd(0x00)  // 设置显示偏移 偏移默认为0
        this.write_cmd(SET_DISP_START_LINE | 0x00) // 设置显示开始行[5:0]
        this.write_cmd(SET_CHARGE_PUMP)
        this.write_cmd(0x14) // 电荷泵设置 bit2，开启/关闭
        this.write_cmd(SET_MEM_ADDR)
        this.write_cmd(0x02) // 设置内存地址模式 [1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
        this.write_cmd(SET_SEG_REMAP | 0x01) // 段重定义设置,bit0:0,0->0;1,0->127;
        this.write_cmd(SET_COM_OUT_DIR | 0x08) // 设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
        this.write_cmd(SET_COM_PIN_CFG)
        this.write_cmd(0x12) // 设置COM硬件引脚配置   [5:4]配置
        this.write_cmd(SET_PRECHARGE)
        this.write_cmd(0xf1) // 设置预充电周期 [3:0],PHASE 1;[7:4],PHASE 2;
        this.write_cmd(SET_VCOM_DESEL)
        this.write_cmd(0x30)  // 设置VCOMH 电压倍率 [6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
        this.write_cmd(SET_CONTRAST)
        this.write_cmd(0xff) // 对比度设置 默认0x7F（范围1~255，越大越亮）
        this.write_cmd(SET_ENTIRE_ON) // 全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
        this.write_cmd(SET_NORM_INV) // 设置显示方式;bit0:1,反相显示;0,正常显示
        this.write_cmd(SET_DISP | 0x01)
        this.fill(1)
        this.show()
    }

    poweroff() {
        this.write_cmd(SET_DISP | 0x00)
    }

    poweron() {
        this.write_cmd(SET_DISP | 0x01)
    }

    rotate(flag, update) {
        if (update === undefined) {
            update = true
        }
        if (flag) {
            this.write_cmd(SET_SEG_REMAP | 0x01)  // mirror display vertically
            this.write_cmd(SET_SCAN_DIR | 0x08)  // mirror display hor.
        }
        else {
            this.write_cmd(SET_SEG_REMAP | 0x00)
            this.write_cmd(SET_SCAN_DIR | 0x00)
        }
        if (update) {
            this.show()
        }
    }

    contrast(contrast) {
        this.write_cmd(SET_CONTRAST)
        this.write_cmd(contrast)
    }

    invert(invert) {
        this.write_cmd(SET_NORM_INV | (invert & 1))
    }
}
