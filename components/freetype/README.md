@page freetype freetype

[更正文档](https://gitee.com/alios-things/freetype/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
对于嵌入式系统应用中，尽管点阵字体在时间和空间性能上都有较佳的表现，但是由于缺乏灵活性，无法改变字体的大小、字形和风格，为解决这些问题，引入freetype2.5.3，FreeType库是一个开源的、可移植的字体引擎，它功能强大，简单说就是支持字体的花样显示。
组件支持以下功能：
- 支持多种字体格式文件，并提供了统一的访问接口；
- 支持单色位图、反走样位图渲染；
- 采用面向对象思想设计，用户可以灵活的根据需要裁剪。

## 版权信息
> FTL 一个类似BSD风格的license

## 目录结构
```tree
├── FTL.TXT    # license描述文件
├── devel      # DEBUG级别配置文件
├── src        # freetype实现文件
├── include    # freetype头文件
├── package.yaml     # 编译配置文件
```

## 依赖组件
* vfs

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> 编译时允许引用内部头文件: 必须允许, 如果不允许会导致编译不过，在yaml中修改FT2_BUILD_LIBRARY配置
```yaml
def_config:
  FT2_BUILD_LIBRARY: y
```

# API说明
freetype API用法与开源版本一致，最重要最常用的API如下，其他API可以在网上搜索其用法。
## FT_Init_FreeType初始化
```C
FT_Library pFTLib;
//  Init FreeType Lib to manage memory
error = FT_Init_FreeType(&pFTLib);
if (error)
{
    pFTLib = 0;
    printf("There is some error when Init Library");
    return -1;
}
```

## FT_New_Face装载一个字体
```C
FT_Face pFTFace;
//  create font face from font file
error = FT_New_Face(pFTLib, "arialuni.ttf", 0, &pFTFace);
```

## FT_Set_Char_Size和FT_Set_Pixel_Sizes设置当前字体大小跟像素尺寸
当一个新的face对象建立时，所有成员都在初始化阶段设为0。调用FT_Set_Char_Size和FT_Set_Pixel_Sizes对这个结构进行赋值。
```C
FT_Set_Char_Size(face, /* handle to face object */ 
                         0, /* char_width in 1/64th of points */
                         16*64, /* char_height in 1/64th of points */
                         300, /* horizontal device resolution */
                         300 ); /* vertical device resolution */
 
FT_Set_Pixel_Sizes(face, /* handle to face object */
                   0, /* pixel_width */
                   16 ); /* pixel_height */
```

## FT_Load_Glyph加载字符的glyph
```C
glyph_index = FT_Get_Char_Index(face, charcode);
FT_Load_Glyph(face, /* handle to face object */
              glyph_index, /* glyph index */
              load_flags ); /* load flags, see below */
```

## FT_Set_Transform字体变换（旋转跟缩放）
```C
FT_Set_Transform(face, /* target face object */
                  &matrix, /* pointer to 2x2 matrix */
                  &delta); /* pointer to 2d vector */
```

# 使用示例
用freetype显示一个字符的主要步骤：
0.初始化FT_Init_Freetype()
1.给定一个文字的ASCII码（例如‘A’是0x41）
2.提供一个字体文件
3.根据编码值到字体文件加载对应的glyph(glyph含有关键点，相对位置。实际上是根据字符的编码类型和编码值找到glyph，不同的编码类型的字符，要用不同的charmap来寻找其glyph,一个charmap支持一种编码,即有多个charmap以支持多种编码）可理解为字体文件含有charmap和字的对应glyph
4.设置字体大小
5.用某些函数把glyph里的关键点缩放为这个字体大小
6.转换为位图点阵（最终还是点阵）
7.在LCD上显示出来

由于freetype是对矢量字体做显示，需要显示设备的配合，目前已经在SDL2中调用了freetype的接口去链接ttf字库，然后在屏幕上显示中英文字符，具体使用情况可以查看components/SDL2/src/ttf/SDL_TTF.c


# FAQ
Q1： 什么是charmap？
> 答：字符影射表(charmap)。字符对应的字体数据称为glyph，字体文件中通常带有一个字符映射表，用来把字符映射到对应glyph的索引值。因为字符集的编码方式有多种，所以可以存在多个子映射表，以支持从不同编码的字符到glyph索引的映射。如果某个字符没有对应的glyph，返回索引 0 ，glyph 0 通常显示一个方块或者空格。

Q2： freetype支持哪些字体格式？
> 答：freetype提供统一的接口来访问多种字体格式文件，包括TrueType, OpenType, Type1, CID, CFF, Windows FON/FNT, X11 PCF等。
