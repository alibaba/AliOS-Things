# convert.py 使用说明 [[English]](./README.md)

[convert.py](./convert.py) 脚本将 .npy 文件中的浮点系数量化为 C/C++ 代码，存储到 .cpp 和 .hpp 文件中。该脚本还会转换系数的元素顺序，从而加速操作。

convert.py 根据 config.json 文件运行。该文件是模型必要的配置文件。有关如何写 config.json file 文件，请参考 [config.json 配置规范](./specification_of_config_json_cn.md)。

注意，convert.py 需在 Python 3.7 或更高版本中运行。

## 实参描述

运行 convert.py 时需填写以下实参：

| 实参            | 值                                        |
| :------------------ | :------------------------------------------- |
| -t \| --target_chip | esp32 \| esp32s2 \|esp32s3 \| esp32c3        |
| -i \| --input_root  | npy 文件和 json 文件所在目录         |
| -j \| --json_file_name  | json 文件名 <br/>(默认: config.json)         |
| -n \| --name        | 输出文件名        |
| -o \| --output_root | 输出文件所在目录 |
| -q \| --quant | 量化颗粒度 <br/> 0(默认) 代表按层量化, 1 代表按通道量化 |



## 示例

假设：

- convert.py 的相对路径为 **./convert.py**
- 目标芯片为 **esp32s3**
- npy 文件和 config.json 文件在 **./my_input_directory** 目录中
- 输出文件名为 **my_coefficient**
- 输出文件将存放在 **./my_output_directory** 目录

运行如下命令：

```sh
python ./convert.py -t esp32s3 -i ./my_input_directory -n my_coefficient -o ./my_output_directory
```

之后将生成 `my_coefficient.cpp` 和 `my_coefficient.hpp` 文件，存放在 `./my_output_directory` 目录中。
