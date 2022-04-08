# UI Lite控件集合

UI Lite for Python以LVGL 8.1为基础进行打造，本文是各控件的示例代码和效果图集合，方便开发者进行索引。关于各对象的方法和属性，可以参考[lvgl 8.1官方文档](https://docs.lvgl.io/8.1/intro/index.html)。

## obj - 基础能力集合
* 此对象实现了控件的基础能力，包括坐标、父子对象、基础样式、点击/滚动等属性。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_basicobject.png width=30%/>
</div>

* 示例代码：
    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()              # lvgl组件初始化
    disp.init()            # 屏幕/触摸屏初始化等

    scr = lv.scr_act()

    # 创建基础控件对象，设置控件尺寸和控件摆放位置
    obj1 = lv.obj(scr)
    obj1.set_size(100, 50)
    obj1.align(lv.ALIGN.CENTER, -60, -30)

    # 创建一个阴影的样式
    style_shadow = lv.style_t()
    style_shadow.init()
    style_shadow.set_shadow_width(10)
    style_shadow.set_shadow_spread(5)
    style_shadow.set_shadow_color(lv.palette_main(lv.PALETTE.BLUE))

    # 创建基础控件，设置控件样式、尺寸和控件摆放位置
    obj2 = lv.obj(scr)
    obj2.add_style(style_shadow, 0)

    obj2.align(lv.ALIGN.CENTER, 60, 30)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[obj](https://docs.lvgl.io/8.1/widgets/obj.html#overview)。

## arc - 弧线
* *Arc由前景弧线和背景弧线组成，前景弧线可通过触摸调节；

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_arc.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()              # lvgl组件初始化
    disp.init()            # 屏幕/触摸屏初始化等

    scr = lv.scr_act()

    # 创建Arc对象，设置其末端的角度、尺寸及位置
    arc = lv.arc(scr)
    arc.set_end_angle(200)
    arc.set_size(150, 150)
    arc.center()

    # 将控件显示在屏幕上
    lv.scr_load(scr)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[arc](https://docs.lvgl.io/8.1/widgets/core/arc.html)。

## bar - 进度条
* Bar有背景和一个指示器，指示器的宽度由Bar当前的宽度决定。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_bar.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()              # lvgl组件初始化
    disp.init()            # 屏幕/触摸屏初始化等

    scr = lv.scr_act()

    # 创建bar, 设置其大小、摆放位置和进度位置
    bar1 = lv.bar(scr)
    bar1.set_size(200, 20)
    bar1.center()
    bar1.set_value(70, lv.ANIM.OFF)

    # 将控件显示在屏幕上
    lv.scr_load(scr)

    ```
    </details>

&emsp;&emsp;
详细API说明请参考[bar](https://docs.lvgl.io/8.1/widgets/core/bar.html#overview)。

## btn - 按钮
* 按钮主要用于提交页面的内容，或者是确认某种操作。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_button.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()              # lvgl组件初始化
    disp.init()            # 屏幕/触摸屏初始化等

    scr = lv.scr_act()

    def event_handler(evt):
        code = evt.get_code()

        if code == lv.EVENT.CLICKED:
                print("Clicked event seen")
        elif code == lv.EVENT.VALUE_CHANGED:
            print("Value changed seen")

    # 创建一个简单按钮
    btn1 = lv.btn(scr)

    # 设置按钮相关事件的callback，所有按下/松开灯事件都需要送到回调函数
    btn1.add_event_cb(event_handler,lv.EVENT.ALL, None)

    btn1.align(lv.ALIGN.CENTER,0,-40)
    label=lv.label(btn1)
    label.set_text("Button")

    # 创建一个状态开关按钮
    btn2 = lv.btn(scr)

    # 设置状态开关按钮相关事件的callback，所有按下/松开灯事件都需要送到回调函数
    btn2.add_event_cb(event_handler,lv.EVENT.ALL, None)
    #btn2.add_event_cb(event_handler,lv.EVENT.VALUE_CHANGED,None)

    btn2.align(lv.ALIGN.CENTER,0,40)
    btn2.add_flag(lv.obj.FLAG.CHECKABLE)
    btn2.set_height(lv.SIZE.CONTENT)

    label=lv.label(btn2)
    label.set_text("Toggle")
    label.center()

    # 将控件显示在屏幕上
    lv.scr_load(scr)

    ```
    </details>

&emsp;&emsp;
详细API说明请参考[btn](https://docs.lvgl.io/8.1/widgets/core/btn.html#overview)。

## btnmatrix - 组合按钮
* Button matrix控件是一种轻量级实现展示多个横竖排列Button的控件，比如计算器、电话拨号盘等。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_button_matrix.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()              # lvgl组件初始化
    disp.init()            # 屏幕/触摸屏初始化等

    scr = lv.scr_act()

    # 回调处理函数
    def event_handler(evt):
        code = evt.get_code()
        obj  = evt.get_target()

        if code == lv.EVENT.VALUE_CHANGED :
            id = obj.get_selected_btn()
            txt = obj.get_btn_text(id)

            print("%s was pressed"%txt)

    btnm_map = ["1", "2", "3", "4", "5", "\n",
                "6", "7", "8", "9", "0", "\n",
                "Action1", "Action2", ""]

    # 创建组合按钮，设置按钮大小，按钮上要显示的文字及按钮属性、按钮动作回调函数等
    btnm1 = lv.btnmatrix(scr)
    btnm1.set_map(btnm_map)
    btnm1.set_btn_width(10, 2)        # Make "Action1" twice as wide as "Action2"
    btnm1.set_btn_ctrl(10, lv.btnmatrix.CTRL.CHECKABLE)
    btnm1.set_btn_ctrl(11, lv.btnmatrix.CTRL.CHECKED)
    btnm1.align(lv.ALIGN.CENTER, 0, 0)
    btnm1.add_event_cb(event_handler, lv.EVENT.ALL, None)

    # 将控件显示在屏幕上
    lv.scr_load(scr)

    ```
    </details>

&emsp;&emsp;
详细API说明请参考[btnmatrix](https://docs.lvgl.io/8.1/widgets/core/btnmatrix.html)。

##  canvas - 画布
* 可以在画布控件上面画圆、方块、线、写字、贴图等，并且可以做旋转、放大、缩小等效果；

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_canvas.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()              # lvgl组件初始化
    disp.init()            # 屏幕/触摸屏初始化等

    scr = lv.scr_act()

    _CANVAS_WIDTH = 200
    _CANVAS_HEIGHT = 150
    LV_IMG_ZOOM_NONE = 256

    rect_dsc = lv.draw_rect_dsc_t()
    rect_dsc.init()
    rect_dsc.radius = 10
    rect_dsc.bg_opa = lv.OPA.COVER
    rect_dsc.bg_grad.dir = lv.GRAD_DIR.HOR
    rect_dsc.bg_grad.stops[0].color = lv.palette_main(lv.PALETTE.RED)
    rect_dsc.bg_grad.stops[1].color = lv.palette_main(lv.PALETTE.BLUE)
    rect_dsc.border_width = 2
    rect_dsc.border_opa = lv.OPA._90
    rect_dsc.border_color = lv.color_white()
    rect_dsc.shadow_width = 5
    rect_dsc.shadow_ofs_x = 5
    rect_dsc.shadow_ofs_y = 5

    label_dsc = lv.draw_label_dsc_t()
    label_dsc.init()
    label_dsc.color = lv.palette_main(lv.PALETTE.YELLOW)

    cbuf = bytearray(_CANVAS_WIDTH * _CANVAS_HEIGHT * 4)

    canvas = lv.canvas(scr)
    canvas.set_buffer(cbuf, _CANVAS_WIDTH, _CANVAS_HEIGHT, lv.img.CF.TRUE_COLOR)
    canvas.center()
    canvas.fill_bg(lv.palette_lighten(lv.PALETTE.GREY, 3), lv.OPA.COVER)

    canvas.draw_rect(70, 60, 100, 70, rect_dsc)
    canvas.draw_text(40, 20, 100, label_dsc, "Some text on text canvas")

    # Test the rotation. It requires another buffer where the original image is stored.
    # So copy the current image to buffer and rotate it to the canvas

    img = lv.img_dsc_t()
    img.data = cbuf[:]
    img.header.cf = lv.img.CF.TRUE_COLOR
    img.header.w = _CANVAS_WIDTH
    img.header.h = _CANVAS_HEIGHT

    canvas.fill_bg(lv.palette_lighten(lv.PALETTE.GREY, 3), lv.OPA.COVER)
    canvas.transform(img, 30, LV_IMG_ZOOM_NONE, 0, 0, _CANVAS_WIDTH // 2, _CANVAS_HEIGHT // 2, True)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[canvas](https://docs.lvgl.io/8.1/widgets/core/canvas.html)。

## Checkbox - 复选框
* 复选框由“勾选框”和标签组成，复选框是最常用的选择类控件之一。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_checkbox.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()              # lvgl组件初始化
    disp.init()            # 屏幕/触摸屏初始化等

    scr = lv.scr_act()

    # 复选框事件回调函数：获取用户的点击事件，并将点击事件打印出来
    def event_handler(e):
        code = e.get_code()
        obj = e.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            txt = obj.get_text()
            if obj.get_state() & lv.STATE.CHECKED:
                state = "Checked"
            else:
                state = "Unchecked"
            print(txt + ":" + state)

    # 设置画面布局样式
    scr.set_flex_flow(lv.FLEX_FLOW.COLUMN)
    scr.set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.CENTER)

    # 创建复选框，并设定复选框名称、属性等信息并设置复选框相关动作回调函数
    cb = lv.checkbox(scr)
    cb.set_text("Apple")
    cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

    cb = lv.checkbox(scr)
    cb.set_text("Banana")
    cb.add_state(lv.STATE.CHECKED)
    cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

    cb = lv.checkbox(scr)
    cb.set_text("Lemon")
    cb.add_state(lv.STATE.DISABLED)
    cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

    cb = lv.checkbox(scr)
    cb.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
    cb.set_text("Melon")
    cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

    # 更新画面布局
    cb.update_layout()

    # 将图像显示在屏幕上
    lv.scr_load(scr)

    ```
    </details>

&emsp;&emsp;
详细API说明请参考[checkbox](https://docs.lvgl.io/8.1/widgets/core/checkbox.html)。

## dropdown - 下拉列表
* 下拉列表允许用户从列表中选择一个值。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_dropdownlist.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()              # lvgl组件初始化
    disp.init()            # 屏幕/触摸屏初始化等

    scr = lv.scr_act()

    # 下拉列表动作回调函数
    def event_handler(e):
        code = e.get_code()
        obj = e.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            option = " "*10 # should be large enough to store the option
            obj.get_selected_str(option, len(option))
            # .strip() removes trailing spaces
            print("Option: \"%s\"" % option.strip())

    # 创建下拉列表
    dd = lv.dropdown(lv.scr_act())

    # 设置下拉列表下拉选项
    dd.set_options("\n".join([
        "Apple",
        "Banana",
        "Orange",
        "Cherry",
        "Grape",
        "Raspberry",
        "Melon",
        "Orange",
        "Lemon",
        "Nuts"]))

    # 设置下拉选项的样式
    dd.align(lv.ALIGN.TOP_MID, 0, 20)

    # 设置下拉列表回调函数
    dd.add_event_cb(event_handler, lv.EVENT.ALL, None)

    # 将图像显示在屏幕上
    lv.scr_load(scr)

    ```
    </details>

&emsp;&emsp;
详细API说明请参考[dropdown](https://docs.lvgl.io/8.1/widgets/core/dropdown.html)。

## img - 图片
* 显示图片文件的基础控件，支持设置图片的透明、着色、修改大小、马赛克等。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_image.png width=30%/>
</div>

* 示例代码：<br>
本示例用到的名为img_cogwheel_argb的[图片链接](../../images/img_cogwheel_argb.png)：
    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    # copy img_cogwheel_argb.png in the RESOURCE_ROOT
    RESOURCES_ROOT = "S:/data/pyamp/"

    img1 = lv.img(lv.scr_act())
    img1.set_src(RESOURCES_ROOT + "img_cogwheel_argb.png")
    img1.align(lv.ALIGN.CENTER, 0, -20)
    img1.set_size(200, 200)

    img2 = lv.img(lv.scr_act())
    img2.set_src(lv.SYMBOL.OK + "Accept")
    img2.align_to(img1, lv.ALIGN.OUT_BOTTOM_MID, 0, 20)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[img](https://docs.lvgl.io/8.1/widgets/core/img.html#overview)。

## label - 标签
* 标签是用于显示文字的基础控件

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_label.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    #
    # Show line wrap, re-color, line align and text scrolling.
    #
    label1 = lv.label(lv.scr_act())
    label1.set_long_mode(lv.label.LONG.WRAP)      # Break the long lines*/
    label1.set_recolor(True)                      # Enable re-coloring by commands in the text
    label1.set_text("#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center"
                                "and  wrap long text automatically.")
    label1.set_width(150)                         # Set smaller width to make the lines wrap
    label1.set_style_text_align(lv.ALIGN.CENTER, 0)
    label1.align(lv.ALIGN.CENTER, 0, -40)

    label2 = lv.label(lv.scr_act())
    label2.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR) # Circular scroll
    label2.set_width(150)
    label2.set_text("It is a circularly scrolling text. ")
    label2.align(lv.ALIGN.CENTER, 0, 40)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[label](https://docs.lvgl.io/8.1/widgets/core/label.html)。

## line - 直线
* 在指定点之间绘制直线

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_line.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    # Create an array for the points of the line
    line_points = [ {"x":5, "y":5}, 
                    {"x":70, "y":70}, 
                    {"x":120, "y":10}, 
                    {"x":180, "y":60}, 
                    {"x":240, "y":10}]

    # Create style
    style_line = lv.style_t()
    style_line.init()
    style_line.set_line_width(8)
    style_line.set_line_color(lv.palette_main(lv.PALETTE.BLUE))
    style_line.set_line_rounded(True)

    # Create a line and apply the new style
    line1 = lv.line(lv.scr_act())
    line1.set_points(line_points, 5)     # Set the points
    line1.add_style(style_line, 0)
    line1.center()
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[line](https://docs.lvgl.io/8.1/widgets/core/line.html#)。

## roller - 滚动选择框
* 允许你通过滚动从列表中选择一个选项。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_roller.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def event_handler(e):
        code = e.get_code()
        obj = e.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            option = " "*10
            obj.get_selected_str(option, len(option))
            print("Selected month: " + option.strip())

    #
    # An infinite roller with the name of the months
    #

    roller1 = lv.roller(lv.scr_act())
    roller1.set_options("\n".join([
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"]),lv.roller.MODE.INFINITE)

    roller1.set_visible_row_count(4)
    roller1.center()
    roller1.add_event_cb(event_handler, lv.EVENT.ALL, None)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[roller](https://docs.lvgl.io/8.1/widgets/core/roller.html)。

## Slider - 滑块
* 滑块是一个带有指示器的条，可以通过拖动指示器设置滑块的数值；滑块可以是水平的，也可以是垂直的；

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_slider.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    # 
    # A default slider with a label displaying the current value
    #
    def slider_event_cb(e):

        slider = e.get_target()
        slider_label.set_text("{:d}%".format(slider.get_value()))
        slider_label.align_to(slider, lv.ALIGN.OUT_BOTTOM_MID, 0, 10)

    # Create a slider in the center of the display
    slider = lv.slider(lv.scr_act())
    slider.center()
    slider.add_event_cb(slider_event_cb, lv.EVENT.VALUE_CHANGED, None)

    # Create a label below the slider
    slider_label = lv.label(lv.scr_act())
    slider_label.set_text("0%")

    slider_label.align_to(slider, lv.ALIGN.OUT_BOTTOM_MID, 0, 10)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[slider](https://docs.lvgl.io/8.1/widgets/core/slider.html)。

## switch - 开关按钮
* 选择按钮的一种，只有打开或关闭两种状态。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_switch.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def event_handler(e):
        code = e.get_code()
        obj = e.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if obj.has_state(lv.STATE.CHECKED):
                print("State: on")
            else:
                print("State: off")

    lv.scr_act().set_flex_flow(lv.FLEX_FLOW.COLUMN)
    lv.scr_act().set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)

    sw = lv.switch(lv.scr_act())
    sw.add_event_cb(event_handler,lv.EVENT.ALL, None)

    sw = lv.switch(lv.scr_act())
    sw.add_state(lv.STATE.CHECKED)
    sw.add_event_cb(event_handler, lv.EVENT.ALL, None)

    sw = lv.switch(lv.scr_act())
    sw.add_state(lv.STATE.DISABLED)
    sw.add_event_cb(event_handler, lv.EVENT.ALL, None)

    sw = lv.switch(lv.scr_act())
    sw.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
    sw.add_event_cb(event_handler, lv.EVENT.ALL, None)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[switch](https://docs.lvgl.io/8.1/widgets/core/switch.html)。

## table - 表格
* table由包含文本的行、列和单元格构建；表格是一个可编辑的对象，允许选择带可编辑的单元格。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_table.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def draw_part_event_cb(e):
        obj = e.get_target()
        dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
        # If the cells are drawn../
        if dsc.part == lv.PART.ITEMS:
            row = dsc.id //  obj.get_col_cnt()
            col = dsc.id - row * obj.get_col_cnt()

            # Make the texts in the first cell center aligned
            if row == 0:
                dsc.label_dsc.align = lv.TEXT_ALIGN.CENTER
                dsc.rect_dsc.bg_color = lv.palette_main(lv.PALETTE.BLUE).color_mix(dsc.rect_dsc.bg_color, lv.OPA._20)
                dsc.rect_dsc.bg_opa = lv.OPA.COVER

            # In the first column align the texts to the right
            elif col == 0:
                dsc.label_dsc.flag = lv.TEXT_ALIGN.RIGHT

            # Make every 2nd row grayish
            if row != 0 and (row % 2) == 0:
                dsc.rect_dsc.bg_color = lv.palette_main(lv.PALETTE.GREY).color_mix(dsc.rect_dsc.bg_color, lv.OPA._10)
                dsc.rect_dsc.bg_opa = lv.OPA.COVER

    table = lv.table(lv.scr_act())

    # Fill the first column
    table.set_cell_value(0, 0, "Name")
    table.set_cell_value(1, 0, "Apple")
    table.set_cell_value(2, 0, "Banana")
    table.set_cell_value(3, 0, "Lemon")
    table.set_cell_value(4, 0, "Grape")
    table.set_cell_value(5, 0, "Melon")
    table.set_cell_value(6, 0, "Peach")
    table.set_cell_value(7, 0, "Nuts")

    # Fill the second column
    table.set_cell_value(0, 1, "Price")
    table.set_cell_value(1, 1, "$7")
    table.set_cell_value(2, 1, "$4")
    table.set_cell_value(3, 1, "$6")
    table.set_cell_value(4, 1, "$2")
    table.set_cell_value(5, 1, "$5")
    table.set_cell_value(6, 1, "$1")
    table.set_cell_value(7, 1, "$9")

    # Set a smaller height to the table. It'll make it scrollable
    table.set_height(200)
    table.center()

    # Add an event callback to apply some custom drawing
    table.add_event_cb(draw_part_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[table](https://docs.lvgl.io/8.1/widgets/core/table.html)。

## textarea - 文本框
* 文本框是带有标签和光标的基础对象，文本过长时，可以滚动文本区域，支持单行模式和密码模式。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_text_area.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def textarea_event_handler(e, ta):
        print("Enter was pressed. The current text is: " + ta.get_text())

    def btnm_event_handler(e, ta):
        obj = e.get_target()
        txt = obj.get_btn_text(obj.get_selected_btn())
        if txt == lv.SYMBOL.BACKSPACE:
            ta.del_char()
        elif txt == lv.SYMBOL.NEW_LINE:
            lv.event_send(ta, lv.EVENT.READY, None)
        elif txt:
            ta.add_text(txt)

    ta = lv.textarea(lv.scr_act())
    ta.set_one_line(True)
    ta.align(lv.ALIGN.TOP_MID, 0, 10)
    ta.add_event_cb(lambda e: textarea_event_handler(e, ta), lv.EVENT.READY, None)
    ta.add_state(lv.STATE.FOCUSED)   # To be sure the cursor is visible

    btnm_map = ["1", "2", "3", "\n",
                "4", "5", "6", "\n",
                "7", "8", "9", "\n",
                lv.SYMBOL.BACKSPACE, "0", lv.SYMBOL.NEW_LINE, ""]

    btnm = lv.btnmatrix(lv.scr_act())
    btnm.set_size(200, 150)
    btnm.align(lv.ALIGN.BOTTOM_MID, 0, -10)
    btnm.add_event_cb(lambda e: btnm_event_handler(e, ta), lv.EVENT.VALUE_CHANGED, None)
    btnm.clear_flag(lv.obj.FLAG.CLICK_FOCUSABLE)    # To keep the text area focused on button clicks
    btnm.set_map(btnm_map)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[textarea](https://docs.lvgl.io/8.1/widgets/core/textarea.html)。

## calendar - 日历控件
* 显示日历并获取用户选择的日期

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_calender.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def event_handler(evt):
        code = evt.get_code()

        if code == lv.EVENT.VALUE_CHANGED:
            source = evt.get_current_target()
            date = lv.calendar_date_t()
            if source.get_pressed_date(date) == lv.RES.OK:        
                calendar.set_today_date(date.year, date.month, date.day)
                print("Clicked date: %02d.%02d.%02d"%(date.day, date.month, date.year))

    calendar = lv.calendar(lv.scr_act())
    calendar.set_size(200, 200)
    calendar.align(lv.ALIGN.CENTER, 0, 20)
    calendar.add_event_cb(event_handler, lv.EVENT.ALL, None)

    calendar.set_today_date(2021, 02, 23)
    calendar.set_showed_date(2021, 02)

    # Highlight a few days
    highlighted_days=[
        lv.calendar_date_t({'year':2021, 'month':2, 'day':6}),
        lv.calendar_date_t({'year':2021, 'month':2, 'day':11}),
        lv.calendar_date_t({'year':2021, 'month':2, 'day':22})
    ]

    calendar.set_highlighted_dates(highlighted_days, len(highlighted_days))

    lv.calendar_header_dropdown(calendar)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[calender](https://docs.lvgl.io/8.1/widgets/extra/calendar.html)。

## chart - 图表
* Chart是数据可视化的基本对象，支持折线图和条形图。

* 折线图运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_chart.png width=30%/>
</div>

* 折线图示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    # Create a chart
    chart = lv.chart(lv.scr_act())
    chart.set_size(200, 150)
    chart.center()
    chart.set_type(lv.chart.TYPE.LINE)   # Show lines and points too

    # Add two data series
    ser1 = chart.add_series(lv.palette_main(lv.PALETTE.RED), lv.chart.AXIS.PRIMARY_Y)
    ser2 = chart.add_series(lv.palette_main(lv.PALETTE.GREEN), lv.chart.AXIS.SECONDARY_Y)
    print(ser2)
    # Set next points on ser1
    chart.set_next_value(ser1,10)
    chart.set_next_value(ser1,10)
    chart.set_next_value(ser1,10)
    chart.set_next_value(ser1,10)
    chart.set_next_value(ser1,10)
    chart.set_next_value(ser1,10)
    chart.set_next_value(ser1,10)
    chart.set_next_value(ser1,30)
    chart.set_next_value(ser1,70)
    chart.set_next_value(ser1,90)

    # Directly set points on 'ser2'
    ser2.y_points = [90, 70, 65, 65, 65, 65, 65, 65, 65, 65]
    chart.refresh()      #  Required after direct set
    ```
    </details>

* 条形图运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_chart1.png width=30%/>
</div>

* 条形图示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def draw_event_cb(e):

        dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
        if dsc.part == lv.PART.TICKS and dsc.id == lv.chart.AXIS.PRIMARY_X: 
            month = ["Jan", "Febr", "March", "Apr", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec"]
            # dsc.text is defined char text[16], I must therefore convert the Python string to a byte_array
            dsc.text = bytes(month[dsc.value],"ascii") 
    #
    # Add ticks and labels to the axis and demonstrate scrolling
    #

    # Create a chart
    chart = lv.chart(lv.scr_act())
    chart.set_size(200, 150)
    chart.center()
    chart.set_type(lv.chart.TYPE.BAR)
    chart.set_range(lv.chart.AXIS.PRIMARY_Y, 0, 100)
    chart.set_range(lv.chart.AXIS.SECONDARY_Y, 0, 400)
    chart.set_point_count(12)
    chart.add_event_cb(draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

    # Add ticks and label to every axis
    chart.set_axis_tick(lv.chart.AXIS.PRIMARY_X, 10, 5, 12, 3, True, 40)
    chart.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 10, 5, 6, 2, True, 50)
    chart.set_axis_tick(lv.chart.AXIS.SECONDARY_Y, 10, 5, 3, 4,True, 50)

    # Zoom in a little in X
    chart.set_zoom_x(800)

    # Add two data series
    ser1 = lv.chart.add_series(chart, lv.palette_lighten(lv.PALETTE.GREEN, 2), lv.chart.AXIS.PRIMARY_Y)
    ser2 = lv.chart.add_series(chart, lv.palette_darken(lv.PALETTE.GREEN, 2), lv.chart.AXIS.SECONDARY_Y)

    # Set the next points on 'ser1'
    chart.set_next_value(ser1, 31)
    chart.set_next_value(ser1, 66)
    chart.set_next_value(ser1, 10)
    chart.set_next_value(ser1, 89)
    chart.set_next_value(ser1, 63)
    chart.set_next_value(ser1, 56)
    chart.set_next_value(ser1, 32)
    chart.set_next_value(ser1, 35)
    chart.set_next_value(ser1, 57)
    chart.set_next_value(ser1, 85)
    chart.set_next_value(ser1, 22)
    chart.set_next_value(ser1, 58)

    # Directly set points on 'ser2'
    ser2.y_points = [92,71,61,15,21,35,35,58,31,53,33,73]

    chart.refresh()  # Required after direct set
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[chart](https://docs.lvgl.io/8.1/widgets/extra/chart.html)。

## colorwheel - 颜色选择器
* 允许用户选择颜色。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_colorwheel.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    cw = lv.colorwheel(lv.scr_act(), True)
    cw.set_size(200, 200)
    cw.center()
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[colorwheel](https://docs.lvgl.io/8.1/widgets/extra/colorwheel.html)。

## imgbtn - 图片按钮
* Image button和Button非常相似，它在每个状态下显示用户自定义的图片，而不是绘制矩形；可以设置左、右和中心的图片，中心的图片将重复绘制以匹配对象的宽度。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_image_button.png width=30%/>
</div>

* 示例代码：<br>
示例代码中使用到的三张图片链接：[imgbtn_left](../../images/imgbtn_left.png)、[imgbtn_mid](../../images/imgbtn_mid.png)、[imgbtn_right](../../images/imgbtn_right.png)。<br>
    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()
    RESOURCES_ROOT = "S:/data/pyamp/"

    # Create a transition animation on width transformation and recolor.
    tr_prop = [lv.STYLE.TRANSFORM_WIDTH, lv.STYLE.IMG_RECOLOR_OPA, 0]
    tr = lv.style_transition_dsc_t()
    tr.init(tr_prop, lv.anim_t.path_linear, 200, 0, None)

    style_def = lv.style_t()
    style_def.init()
    style_def.set_text_color(lv.color_white())
    style_def.set_transition(tr)

    # Darken the button when pressed and make it wider
    style_pr = lv.style_t()
    style_pr.init()
    style_pr.set_img_recolor_opa(lv.OPA._30)
    style_pr.set_img_recolor(lv.color_black())
    style_pr.set_transform_width(20)

    # Create an image button
    #
    # make sure that the image in the specify path
    # 
    imgbtn1 = lv.imgbtn(lv.scr_act())
    imgbtn1.set_src(lv.imgbtn.STATE.RELEASED,
                    RESOURCES_ROOT + "imgbtn_left.png",
                    RESOURCES_ROOT + "imgbtn_mid.png",
                    RESOURCES_ROOT + "imgbtn_right.png")
    imgbtn1.add_style(style_def, 0)
    imgbtn1.add_style(style_pr, lv.STATE.PRESSED)

    imgbtn1.align(lv.ALIGN.CENTER, 0, 0)

    # Create a label on the image button
    label = lv.label(imgbtn1)
    label.set_text("Button")
    label.align(lv.ALIGN.CENTER, 0, -4)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[imgbtn](https://docs.lvgl.io/8.1/widgets/extra/imgbtn.html#overview)。

## keyboard - 软键盘
* 显示键盘并获取用户点击的键值

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_keyboard.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def ta_event_cb(e,kb):
        code = e.get_code()
        ta = e.get_target()
        if code == lv.EVENT.FOCUSED:
            kb.set_textarea(ta)
            kb.clear_flag(lv.obj.FLAG.HIDDEN)

        if code == lv.EVENT.DEFOCUSED:
            kb.set_textarea(None)
            kb.add_flag(lv.obj.FLAG.HIDDEN)

    # Create a keyboard to use it with one of the text areas
    kb = lv.keyboard(lv.scr_act())

    # Create a text area. The keyboard will write here
    ta = lv.textarea(lv.scr_act())
    ta.set_width(200)
    ta.align(lv.ALIGN.TOP_LEFT, 10, 10)
    ta.add_event_cb(lambda e: ta_event_cb(e,kb), lv.EVENT.ALL, None)
    ta.set_placeholder_text("Hello")

    ta = lv.textarea(lv.scr_act())
    ta.set_width(200)
    ta.align(lv.ALIGN.TOP_RIGHT, -10, 10)
    ta.add_event_cb(lambda e: ta_event_cb(e,kb), lv.EVENT.ALL, None)

    kb.set_textarea(ta)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[keyboard](https://docs.lvgl.io/8.1/widgets/extra/keyboard.html)。

## led - 发光体
* LED是矩形（或圆形）物体，其亮度可以调节。亮度越低，LED的颜色越暗。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_led.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    #
    # Create LED's with different brightness and color
    #

    # Create a LED and switch it OFF
    led1  = lv.led(lv.scr_act())
    led1.align(lv.ALIGN.CENTER, -80, 0)
    led1.off()

    # Copy the previous LED and set a brightness
    led2  = lv.led(lv.scr_act())
    led2.align(lv.ALIGN.CENTER, 0, 0)
    led2.set_brightness(150)
    led2.set_color(lv.palette_main(lv.PALETTE.RED))

    # Copy the previous LED and switch it ON
    led3  = lv.led(lv.scr_act())
    led3.align(lv.ALIGN.CENTER, 80, 0)
    led3.on()
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[led](https://docs.lvgl.io/8.1/widgets/extra/led.html)。

## list - 列表
* List基本上是一个垂直布局的矩形，可添加按钮和文本。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_list.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def event_handler(e):
        code = e.get_code()
        obj = e.get_target()
        if code == lv.EVENT.CLICKED:
                print("Clicked: list1." + list1.get_btn_text(obj))

    # Create a list
    list1 = lv.list(lv.scr_act())
    list1.set_size(180, 220)
    list1.center()

    # Add buttons to the list
    list1.add_text("File")
    btn_new = list1.add_btn(lv.SYMBOL.FILE, "New")
    btn_new.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_open = list1.add_btn(lv.SYMBOL.DIRECTORY, "Open")
    btn_open.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_save = list1.add_btn(lv.SYMBOL.SAVE, "Save")
    btn_save.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_delete = list1.add_btn(lv.SYMBOL.CLOSE, "Delete")
    btn_delete.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_edit = list1.add_btn(lv.SYMBOL.EDIT, "Edit")
    btn_edit.add_event_cb(event_handler,lv.EVENT.ALL, None)

    list1.add_text("Connectivity")
    btn_bluetooth = list1.add_btn(lv.SYMBOL.BLUETOOTH, "Bluetooth")
    btn_bluetooth.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_navig = list1.add_btn(lv.SYMBOL.GPS, "Navigation")
    btn_navig.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_USB = list1.add_btn(lv.SYMBOL.USB, "USB")
    btn_USB.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_battery = list1.add_btn(lv.SYMBOL.BATTERY_FULL, "Battery")
    btn_battery.add_event_cb(event_handler,lv.EVENT.ALL, None)

    list1.add_text("Exit")
    btn_apply = list1.add_btn(lv.SYMBOL.OK, "Apply")
    btn_apply.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_close = list1.add_btn(lv.SYMBOL.CLOSE, "Close")
    btn_close.add_event_cb(event_handler,lv.EVENT.ALL, None)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[list](https://docs.lvgl.io/8.1/widgets/extra/list.html)。

## meter - 表盘
* Meter可以灵活的显示可视化数据，可显示圆弧、指针、刻度线和标签。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_meter.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def set_value(indic, v):
        meter.set_indicator_value(indic, v)

    #
    # A simple meter
    #
    meter = lv.meter(lv.scr_act())
    meter.center()
    meter.set_size(200, 200)

    # Add a scale first
    scale = meter.add_scale()
    meter.set_scale_ticks(scale, 51, 2, 10, lv.palette_main(lv.PALETTE.GREY))
    meter.set_scale_major_ticks(scale, 10, 4, 15, lv.color_black(), 10)

    indic = lv.meter_indicator_t()

    # Add a blue arc to the start
    indic = meter.add_arc(scale, 3, lv.palette_main(lv.PALETTE.BLUE), 0)
    meter.set_indicator_start_value(indic, 0)
    meter.set_indicator_end_value(indic, 20)

    # Make the tick lines blue at the start of the scale
    indic = meter.add_scale_lines(scale, lv.palette_main(lv.PALETTE.BLUE), lv.palette_main(lv.PALETTE.BLUE), False, 0)
    meter.set_indicator_start_value(indic, 0)
    meter.set_indicator_end_value(indic, 20)

    # Add a red arc to the end
    indic = meter.add_arc(scale, 3, lv.palette_main(lv.PALETTE.RED), 0)
    meter.set_indicator_start_value(indic, 80)
    meter.set_indicator_end_value(indic, 100)

    # Make the tick lines red at the end of the scale
    indic = meter.add_scale_lines(scale, lv.palette_main(lv.PALETTE.RED), lv.palette_main(lv.PALETTE.RED), False, 0)
    meter.set_indicator_start_value(indic, 80)
    meter.set_indicator_end_value(indic, 100)

    # Add a needle line indicator
    indic = meter.add_needle_line(scale, 4, lv.palette_main(lv.PALETTE.GREY), -10)

    # Create an animation to set the value
    a = lv.anim_t()
    a.init()
    a.set_var(indic)
    a.set_values(0, 100)
    a.set_time(2000)
    a.set_repeat_delay(100)
    a.set_playback_time(500)
    a.set_playback_delay(100)
    a.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
    a.set_custom_exec_cb(lambda a,val: set_value(indic,val))
    lv.anim_t.start(a)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[meter](https://docs.lvgl.io/8.1/widgets/extra/meter.html)。

## msgbox - 弹窗
* Message Box是弹出窗口，由背景容器、标题、可选关闭按钮、文本和可选按钮构建；可以是模态或非模态。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_message_box.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def event_cb(e):
        mbox = e.get_current_target()
        print("Button %s clicked" % mbox.get_active_btn_text())

    btns = ["Apply", "Close", ""]

    mbox1 = lv.msgbox(lv.scr_act(), "Hello", "This is a message box with two buttons.", btns, True)
    mbox1.add_event_cb(event_cb, lv.EVENT.VALUE_CHANGED, None)
    mbox1.center()
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[msgbox](https://docs.lvgl.io/8.1/widgets/extra/msgbox.html)。

## spans - 富文本
* Span用于显示富文本对象。可以讲不同字体、颜色和大小的文本样式呈现到Span中。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_span.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    #
    # Create span
    #
    style = lv.style_t()
    style.init()
    style.set_border_width(1)
    style.set_border_color(lv.palette_main(lv.PALETTE.ORANGE))
    style.set_pad_all(2)

    spans = lv.spangroup(lv.scr_act())
    spans.set_width(300)
    spans.set_height(300)
    spans.center()
    spans.add_style(style, 0)

    spans.set_align(lv.TEXT_ALIGN.LEFT)
    spans.set_overflow(lv.SPAN_OVERFLOW.CLIP)
    spans.set_indent(20)
    spans.set_mode(lv.SPAN_MODE.BREAK)

    span = spans.new_span()
    span.set_text("china is a beautiful country.")
    span.style.set_text_color(lv.palette_main(lv.PALETTE.RED))
    span.style.set_text_decor(lv.TEXT_DECOR.STRIKETHROUGH | lv.TEXT_DECOR.UNDERLINE)
    span.style.set_text_opa(lv.OPA._30)

    span = spans.new_span()
    span.set_text_static("good good study, day day up.")
    #if LV_FONT_MONTSERRAT_24
    #    lv_style_set_text_font(&span->style,  &lv_font_montserrat_24);
    #endif
    span.style.set_text_color(lv.palette_main(lv.PALETTE.GREEN))

    span = spans.new_span()
    span.set_text_static("LVGL is an open-source graphics library.")
    span.style.set_text_color(lv.palette_main(lv.PALETTE.BLUE))

    span = spans.new_span()
    span.set_text_static("the boy no name.")
    span.style.set_text_color(lv.palette_main(lv.PALETTE.GREEN))
    #if LV_FONT_MONTSERRAT_20
    #    lv_style_set_text_font(&span->style, &lv_font_montserrat_20);
    #endif
    span.style.set_text_decor(lv.TEXT_DECOR.UNDERLINE)

    span = spans.new_span()
    span.set_text("I have a dream that hope to come true.")

    spans.refr_mode()
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[spans](https://docs.lvgl.io/8.1/widgets/extra/span.html#)。

## spinbox - 文本数字
* Spinbox包含一个文本数字，可以通过键或API函数增加或减少。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_spinbox.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def increment_event_cb(e):
        code = e.get_code()
        if code == lv.EVENT.SHORT_CLICKED or code  == lv.EVENT.LONG_PRESSED_REPEAT:
            spinbox.increment()

    def decrement_event_cb(e):
        code = e.get_code()
        if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
            spinbox.decrement()

    spinbox = lv.spinbox(lv.scr_act())
    spinbox.set_range(-1000, 25000)
    spinbox.set_digit_format(5, 2)
    spinbox.step_prev()
    spinbox.set_width(100)
    spinbox.center()

    h = spinbox.get_height()

    btn = lv.btn(lv.scr_act())
    btn.set_size(h, h)
    btn.align_to(spinbox, lv.ALIGN.OUT_RIGHT_MID, 5, 0)
    btn.set_style_bg_img_src(lv.SYMBOL.PLUS, 0)
    btn.add_event_cb(increment_event_cb, lv.EVENT.ALL,  None)

    btn = lv.btn(lv.scr_act())
    btn.set_size(h, h)
    btn.align_to(spinbox, lv.ALIGN.OUT_LEFT_MID, -5, 0)
    btn.set_style_bg_img_src(lv.SYMBOL.MINUS, 0)
    btn.add_event_cb(decrement_event_cb, lv.EVENT.ALL, None)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[spinbox](https://docs.lvgl.io/8.1/widgets/extra/spinbox.html#)。

## spinner - 旋转弧
* Spinner是环上的旋转弧。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_spinner.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    # Create a spinner
    spinner = lv.spinner(lv.scr_act(), 1000, 60)
    spinner.set_size(100, 100)
    spinner.center()
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[spinner](https://docs.lvgl.io/8.1/widgets/extra/spinner.html)。

## tabview - 选项卡容器
* Tab view可用于组织选项卡中的内容，选项卡由其他控件组成。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_tabview.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    # Create a Tab view object
    tabview = lv.tabview(lv.scr_act(), lv.DIR.TOP, 50)

    # Add 3 tabs (the tabs are page (lv_page) and can be scrolled
    tab1 = tabview.add_tab("Tab 1")
    tab2 = tabview.add_tab("Tab 2")
    tab3 = tabview.add_tab("Tab 3")

    # Add content to the tabs
    label = lv.label(tab1)
    label.set_text("""This the first tab

    If the content
    of a tab
    becomes too
    longer
    than the
    container
    then it
    automatically
    becomes
    scrollable.

    Can you see it?""")

    label = lv.label(tab2)
    label.set_text("Second tab")

    label = lv.label(tab3)
    label.set_text("Third tab");

    label.scroll_to_view_recursive(lv.ANIM.ON)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[tableview](https://docs.lvgl.io/8.1/widgets/extra/tabview.html)。

## tileview - 网格容器
* Tile view是一个容器对象，其元素可以以网格形式排列，用户可以通过滑动在Tile之间切换，也可以禁止Tile在任何方向的滑动。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_tileview.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    #
    # Create a 2x2 tile view and allow scrolling only in an "L" shape.
    # Demonstrate scroll chaining with a long list that
    # scrolls the tile view when it can't be scrolled further.
    #
    tv = lv.tileview(lv.scr_act())

    # Tile1: just a label
    tile1 = tv.add_tile(0, 0, lv.DIR.BOTTOM)
    label = lv.label(tile1)
    label.set_text("Scroll down")
    label.center()

    # Tile2: a button
    tile2 = tv.add_tile(0, 1, lv.DIR.TOP | lv.DIR.RIGHT)

    btn = lv.btn(tile2)

    label = lv.label(btn)
    label.set_text("Scroll up or right")

    btn.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
    btn.center()

    # Tile3: a list
    tile3 = tv.add_tile(1, 1, lv.DIR.LEFT)
    list = lv.list(tile3)
    list.set_size(lv.pct(100), lv.pct(100))

    list.add_btn(None, "One")
    list.add_btn(None, "Two")
    list.add_btn(None, "Three")
    list.add_btn(None, "Four")
    list.add_btn(None, "Five")
    list.add_btn(None, "Six")
    list.add_btn(None, "Seven")
    list.add_btn(None, "Eight")
    list.add_btn(None, "Nine")
    list.add_btn(None, "Ten")
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[tileview](https://docs.lvgl.io/8.1/widgets/extra/tileview.html)。

## win - 窗口
* 窗口是由标题、按钮和内容区域构建的容器对象。

* 运行效果：
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaS_UI_Lite_window.png width=30%/>
</div>

* 示例代码：

    <details>
    <summary> 查看代码</summary>

    ```python
    import lvgl as lv
    from display import LVGL as disp

    lv.init()
    disp.init()

    def event_handler(e):
        code = e.get_code()
        obj = e.get_target()
        if code == lv.EVENT.CLICKED:
            print("Button {:d} clicked".format(obj.get_child_id()))

    win = lv.win(lv.scr_act(), 60)
    btn1 = win.add_btn(lv.SYMBOL.LEFT, 40)
    btn1.add_event_cb(event_handler, lv.EVENT.ALL, None)
    win.add_title("A title")
    btn2=win.add_btn(lv.SYMBOL.RIGHT, 40)
    btn2.add_event_cb(event_handler, lv.EVENT.ALL, None)
    btn3 = win.add_btn(lv.SYMBOL.CLOSE, 60)
    btn3.add_event_cb(event_handler, lv.EVENT.ALL, None)

    cont = win.get_content()  # Content can be added here
    label = lv.label(cont)
    label.set_text("""This is
    a pretty
    long text
    to see how
    the window
    becomes
    scrollable.

    We need
    quite some text
    and we will 
    even put
    some more
    text to be
    sure it
    overflows.
    """)
    ```
    </details>

&emsp;&emsp;
详细API说明请参考[win](https://docs.lvgl.io/8.1/widgets/extra/win.html)。
