# Tools [[中文]](./README_cn.md)

Since ESP-DL is a repository without peripheral drivers, when writing [Examples](../../examples) in the model zoo, we use pixel arrays converted from images. The results can only show in Terminal. For more intuitive user experience, we provide the following tools for image conversion and display.

## Image Converter convert_to_u8.py

This tool allows you to convert your customized image into C/C++ arrays. The table below lists descriptions of arguments:

| Argument       | Type |Value                  |
| :------------- | :-: |:--------------------- |
| -i \| --input  | string | The path of input image |
| -o \| --output | string | The path of output file |



**Example:**

Assume that

- the path of the customized image is my_album/my_image.png
- the output file is saved to esp-dl/examples/human_face_detect/main, the directory of the human face detection project

then the command to execute would be:

```shell
python convert_to_u8.py -i my_album/my_image.png -o ESP-DL/examples/human_face_detect/main/image.hpp
```

> Note: The command above is only for reference and not effective.



## Display Tool display_image.py

This tool draws boxes and keypoints for detection on the image. The table below lists descriptions of arguments:

| Argument              | Type |Value                                                           |
| :---------------- | :---:| :----------------------------------------------------------- |
| -i \| --image     | string |Image path                                             |
| -b \| --box       | string | A box in (x1, y1, x2, y2) format, where (x1, y1) is the upper left point of the box, and(x2, y2) is the lower right point of the box. <br/>If dropped<sup>[1](#note1)</sup>, no box will be drawn. |
| -k \| --keypoints | string | Keypoints in (x1, y1, x2, y2, ... , xn, yn) format, where each (x, y) stands for a point.<br/> If dropped, no keypoint will be drawn. |

> <a name="note1">1</a>: **dropped**: to leave a specific argument empty.

**Example:**

Assume that

- the path of the image is my_album/my_image.jpg
- the upper left point of the box is (137, 75)
- the lower right point of the box is (246, 215)
- point 1 is (157, 131)
- point 2 is (158, 177)
- point 3 is (170, 163)

then the command to execute would be:

```shell
python display_image.py -i my_album/my_image.jpg -b "(137, 75, 246, 215)" -k "(157, 131, 158, 177, 170, 163)"
```

> Note: The command above is only for reference and not effective.
