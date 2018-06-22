#ifndef _CAMERA_IMAGES_H
#define _CAMERA_IMAGES_H


#define ST7789_WIDTH ((uint16_t)240)
#define ST7789_HEIGHT ((uint16_t)240)

/*************
��һ����    λͼ�ļ�ͷ
�ýṹ�ĳ����ǹ̶��ģ�Ϊ14���ֽڣ���������������£�
    2byte   ���ļ����ͣ�������0x4d42�����ַ���"BM"��
    4byte   �������ļ���С
    4byte   �������֣�Ϊ0
    4byte   �����ļ�ͷ��ʵ�ʵ�λͼͼ�����ݵ�ƫ���ֽ�����
*************/
typedef struct
{
    uint32_t imageSize;
    uint32_t blank;
    uint32_t startPosition;
}BmpHead;

/*********************
�ڶ�����    λͼ��Ϣͷ
�ýṹ�ĳ���Ҳ�ǹ̶��ģ�Ϊ40���ֽڣ������������˵�����£�
    4byte   �����ṹ�ĳ��ȣ�ֵΪ40
    4byte   ��ͼ��Ŀ���Ƕ������ء�
    4byte   ��ͼ��ĸ߶��Ƕ������ء�
    2Byte   ��������1��
    2Byte   ����ʾ��ɫʱ�õ���λ�������õ�ֵΪ1(�ڰ׶�ɫͼ)��4(16ɫͼ)��8(256ɫͼ)��24(���ɫͼ)��
    4byte   ��ָ��λͼ�Ƿ�ѹ������ЧֵΪBI_RGB��BI_RLE8��BI_RLE4��BI_BITFIELDS��Windowsλͼ�ɲ���RLE4��RLE8��ѹ����ʽ��BI_RGB��ʾ��ѹ����
    4byte   ��ָ��ʵ�ʵ�λͼͼ������ռ�õ��ֽ������������µĹ�ʽ���������
     ͼ������ = Width' * Height * ��ʾÿ��������ɫռ�õ�byte��(����ɫλ��/8,24bitͼΪ3��256ɫΪ1��
     Ҫע����ǣ�������ʽ�е�biWidth'������4��������(����biWidth�����Ǵ��ڻ����biWidth����С4��������)��
     ���biCompressionΪBI_RGB����������Ϊ0��
    4byte   ��Ŀ���豸��ˮƽ�ֱ��ʡ�
    4byte   ��Ŀ���豸�Ĵ�ֱ�ֱ��ʡ�
    4byte   ����ͼ��ʵ���õ�����ɫ���������ֵΪ0�����õ�����ɫ��Ϊ2��(��ɫλ��)����,����ɫλ��Ϊ8��2^8=256,��256ɫ��λͼ
    4byte   ��ָ����ͼ������Ҫ����ɫ���������ֵΪ0������Ϊ���е���ɫ������Ҫ�ġ�
***********************************/

typedef struct
{
    uint32_t    Length;
    uint32_t    width;
    uint32_t    height;
    uint16_t    colorPlane;
    uint16_t    bitColor;
    uint32_t    zipFormat;
    uint32_t    realSize;
    uint32_t    xPels;
    uint32_t    yPels;
    uint32_t    colorUse;
    uint32_t    colorImportant;
}InfoHead;

/***************************
    ��������    ��ɫ�̽ṹ  ��ɫ��
    ����256ɫBMPλͼ����ɫλ��Ϊ8����Ҫ2^8 = 256����ɫ�̣�
    ����24bitBMPλͼ��������RGBֱֵ�ӱ�����ͼ��������������Ҫ��ɫ�̣������ڵ�ɫ����
    rgbBlue��   ����ɫ����ɫ������
    rgbGreen��  ����ɫ����ɫ������
    rgbRed��    ����ɫ�ĺ�ɫ������
    rgbReserved������ֵ��
************************/
typedef struct
{         
	  uint8_t   rgbBlue;
         uint8_t   rgbGreen;
         uint8_t   rgbRed;
         uint8_t   rgbReserved;
}RGBMixPlate;		 

void CameraDEMO_Init(uint16_t *buff, uint32_t size);
void enable_camera_display(uint8_t on);

#endif  /* Avoid multiple inclusion */

