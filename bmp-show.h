#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/types.h>
#ifndef __BMP_SHOW_H_
#define __BMP_SHOW_H_

//14byte文件头
typedef struct
{
	char cfType[2];  //文件类型，"BM"(0x4D42)
	int  cfSize;     //文件大小（字节）
	int  cfReserved; //保留，值为0
	int  cfoffBits;  //数据区相对于文件头的偏移量（字节）
}__attribute__((packed)) BITMAPFILEHEADER;
//__attribute__((packed))的作用是告诉编译器取消结构在编译过程中的优化对齐

//40byte信息头
typedef struct
{
	char ciSize[4];          //BITMAPFILEHEADER所占的字节数
	int  ciWidth;            //宽度
	int  ciHeight;           //高度
	char ciPlanes[2];        //目标设备的位平面数，值为1
	int  ciBitCount;         //每个像素的位数
	char ciCompress[4];      //压缩说明
	char ciSizeImage[4];     //用字节表示的图像大小，该数据必须是4的倍数
	char ciXPelsPerMeter[4]; //目标设备的水平像素数/米
	char ciYPelsPerMeter[4]; //目标设备的垂直像素数/米
	char ciClrUsed[4];       //位图使用调色板的颜色数
	char ciClrImportant[4];  //指定重要的颜色数，当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要
}__attribute__((packed)) BITMAPINFOHEADER;

typedef struct
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char reserved;
}__attribute__((packed)) PIXEL; //颜色模式RGB



typedef struct
{
	int          fbfd;
	char         *fbp;
	unsigned int xres;
	unsigned int yres;
	unsigned int xres_virtual;
	unsigned int yres_virtual;
	unsigned int xoffset;
	unsigned int yoffset;
	unsigned int bpp;
	unsigned long line_length;
	unsigned long size;

	struct fb_bitfield red;
	struct fb_bitfield green;
	struct fb_bitfield blue;
} FB_INFO;

typedef struct
{
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned long size;
	unsigned int data_offset;
} IMG_INFO;



//函数声明
int show_bmp(char *img_name);
int show_picture(char *img_name);
void *photo_thread(void);


#endif