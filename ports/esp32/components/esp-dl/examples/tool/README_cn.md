# 工具 [[English]](./README.md)

ESP-DL 是不包含外设驱动的仓库，在编写模型库的[示例](../../examples)时使用了数组保存像素值来表示图片，运行结果只能显示在终端中。为使您更充分地体验 ESP-DL，我们提供了以下工具用于图片的转换和显示。

## 图片转换工具 convert_to_u8.py

该转换工具可将自定义图片转换成 C/C++ 的数组形式。配置说明如下：

| 参数           | 类型 |值                     |
| :------------- | :-: | :-------------------- |
| -i \| --input  | string | 输入图片的路径 |
| -o \| --output | string | 输出文件的路经 |



**示例：**

假设，

- 自定义图片路经为 my_album/my_image.png
- 输出文件存放至人脸检测项目文件夹 esp-dl/examples/human_face_detect/main 中

则，

```shell
python convert_to_u8.py -i my_album/my_image.png -o ESP-DL/examples/human_face_detect/main/image.hpp
```

> 注意：以上代码只作示例说明，并非有效代码。



## 显示工具 display_image.py

该显示工具可在图片上绘制用于检测的框和点。配置说明如下：

| 参数              | 类型 | 值                                                           |
| :---------------- | :-: | :----------------------------------------------------------- |
| -i \| --image     | string | 图片路径                                             |
| -b \| --box       | string | 输入依照格式 (x1, y1, x2, y2)，其中 (x1, y1) 表示框的左上角坐标，(x2, y2) 表示框的右下角坐标<br />不配置： 不绘制框 |
| -k \| --keypoints | string | 输入依照格式 (x1, y1, x2, y2, ... , xn, yn)，其中每对 (x, y) 表示一个点<br />不配置： 不绘制点 |



**示例：**

假设，

- 图片路经为 my_album/my_image.jpg
- 框的左上角坐标： (137, 75)
- 框的右下角坐标： (246, 215)
- 点 1 的坐标： (157, 131)
- 点 2 的坐标： (158, 177)
- 点 3 的坐标： (170, 163)

则，

```shell
python display_image.py -i my_album/my_image.jpg -b "(137, 75, 246, 215)" -k "(157, 131, 158, 177, 170, 163)"
```

> 注意：以上代码只作示例说明，并非有效代码。