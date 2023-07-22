
#include"bmp-show.h"
char * bmpname[] = {"/mnt/linux-project/picture/6.bmp","/mnt/linux-project/picture/3.bmp","/mnt/linux-project/picture/4.bmp"};




extern FB_INFO fb_info;
extern IMG_INFO img_info;

int show_bmp(char *img_name);

static int cursor_bitmap_format_convert(char *dst,char *src, unsigned long img_len_one_line)
{
	int img_len ,fb_len ;
	char *p;
	__u32 val;
	PIXEL pix;

	p = (char *)&val;

	img_len = img_info.width; /*一行图片的长度*/
	fb_len = fb_info.xres; /*一行显示屏的长度*/

	/*进行x轴的偏移*/
	dst += fb_info.xoffset * (fb_info.bpp / 8);
	fb_len -= fb_info.xoffset;

	/*bmp 数据是上下左右颠倒的，这里只进行左右的处理*/
	/*先定位到图片的最后一个像素的地址，然后往第一个像素的方向处理，进行左右颠倒的处理*/
	src += img_len_one_line - 1;

	/*处理一行要显示的数据*/
	while(1) {
		if (img_info.bpp == 32)
			pix.reserved = *(src--);
		pix.red   = *(src--);
		pix.green = *(src--);
		pix.blue  = *(src--);

		val = 0x00;
		val |= (pix.red >> (8 - fb_info.red.length)) << fb_info.red.offset;
		val |= (pix.green >> (8 - fb_info.green.length)) << fb_info.green.offset;
		val |= (pix.blue >> (8 - fb_info.blue.length)) << fb_info.blue.offset;


		if (fb_info.bpp == 16) {
			*(dst++) = *(p + 0);
			*(dst++) = *(p + 1);
		}
		else if (fb_info.bpp == 24) {
			*(dst++) = *(p + 0);
			*(dst++) = *(p + 1);
			*(dst++) = *(p + 2);
		}
		else if (fb_info.bpp == 32) {
			*(dst++) = *(p + 0);
			*(dst++) = *(p + 1);
			*(dst++) = *(p + 2);
			*(dst++) = *(p + 3);
		}

		/*超过图片长度或显示屏长度认为一行处理完了*/
		img_len--;
		fb_len--;
		if (img_len <= 0 || fb_len <= 0)
			break;
	}
#if 0
	printf("r = %d\n", pix.red);
	printf("g = %d\n", pix.green);
	printf("b = %d\n", pix.blue);
#endif
	return 0;
}

int show_bmp(char *img_name)
{

	FILE *fp;
	int ret = 0;
	BITMAPFILEHEADER FileHead;
	BITMAPINFOHEADER InfoHead;

	if(img_name == NULL) {
		printf("img_name is null\n");
		return -1;
	}

	fp = fopen( img_name, "rb" );
	if(fp == NULL) {
		printf("img[%s] open failed\n", img_name);
		ret = -1;
		goto err_showbmp;
	}

	/* 移位到文件头部 */
	fseek(fp, 0, SEEK_SET);

	ret = fread(&FileHead, sizeof(BITMAPFILEHEADER), 1, fp);
	if ( ret != 1) {
		printf("img read failed\n");
		ret = -1;
		goto err_showbmp;
	}

	//检测是否是bmp图像
	if (memcmp(FileHead.cfType, "BM", 2) != 0) {
		printf("it's not a BMP file[%c%c]\n", FileHead.cfType[0], FileHead.cfType[1]);
		ret = -1;
		goto err_showbmp;
	}

	ret = fread( (char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
	if ( ret != 1) {
		printf("read infoheader error!\n");
		ret = -1;
		goto err_showbmp;
	}

	img_info.width       = InfoHead.ciWidth;
	img_info.height      = InfoHead.ciHeight;
	img_info.bpp         = InfoHead.ciBitCount;
	img_info.size        = FileHead.cfSize;
	img_info.data_offset = FileHead.cfoffBits;

	//printf("img info w[%d] h[%d] bpp[%d] size[%ld] offset[%d]\n", img_info.width, img_info.height, img_info.bpp, img_info.size, img_info.data_offset);

	if (img_info.bpp != 24 && img_info.bpp != 32) {
		printf("img bpp is not 24 or 32\n");
		ret = -1;
		goto err_showbmp;
	}


	/*
	 *一行行处理
	 */
	char *buf_img_one_line;
	char *buf_fb_one_line;
	char *p;
	int fb_height;

	long img_len_one_line = img_info.width * (img_info.bpp / 8);
	long fb_len_one_line = fb_info.line_length;

	printf("img_len_one_line = %d\n", img_len_one_line);
	printf("fb_len_one_line = %d\n", fb_info.line_length);

	buf_img_one_line = (char *)calloc(1, img_len_one_line + 256);
	if(buf_img_one_line == NULL) {
		printf("alloc failed\n");
		ret = -1;
		goto err_showbmp;
	}

	buf_fb_one_line = (char *)calloc(1, fb_len_one_line + 256);
	if(buf_fb_one_line == NULL) {
		printf("alloc failed\n");
		ret = -1;
		goto err_showbmp;
	}


	fseek(fp, img_info.data_offset, SEEK_SET);

	p = fb_info.fbp + fb_info.yoffset * fb_info.line_length; /*进行y轴的偏移*/
	fb_height = fb_info.yres;
	while (1) {
		memset(buf_img_one_line, 0, img_len_one_line);
		memset(buf_fb_one_line, 0, fb_len_one_line);
		ret = fread(buf_img_one_line, 1, img_len_one_line, fp);
		if (ret < img_len_one_line) {
			/*图片读取完成，则图片显示完成*/
			printf("read to end of img file\n");
			cursor_bitmap_format_convert(buf_fb_one_line, buf_img_one_line, img_len_one_line); /*数据转换*/
			memcpy(fb_info.fbp, buf_fb_one_line, fb_len_one_line);
			break;
		}

		cursor_bitmap_format_convert(buf_fb_one_line, buf_img_one_line, img_len_one_line); /*数据转换*/
		memcpy(p, buf_fb_one_line, fb_len_one_line); /*显示一行*/
		p += fb_len_one_line;

		/*超过显示屏宽度认为图片显示完成*/
		fb_height--;
		if (fb_height <= 0)
			break;
	}

	free(buf_img_one_line);
	free(buf_fb_one_line);

	fclose(fp);
	return ret;
err_showbmp:
	if (fp)
		fclose(fp);
	return ret;
}

int show_picture(char *img_name)
{
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	if (fb_info.fbfd <= -1) {
		printf("fb open fialed\n");
		return -1;
	}

	if (ioctl(fb_info.fbfd, FBIOGET_FSCREENINFO, &finfo)) {
		printf("fb ioctl fialed\n");
		return -1;
	}

	if (ioctl(fb_info.fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf("fb ioctl fialed\n");
		return -1;
	}

	fb_info.xres = vinfo.xres;
	fb_info.yres = vinfo.yres;
	fb_info.xres_virtual = vinfo.xres_virtual;
	fb_info.yres_virtual = vinfo.yres_virtual;
	fb_info.xoffset = vinfo.xoffset;
	fb_info.yoffset = vinfo.yoffset;
	fb_info.bpp  = vinfo.bits_per_pixel;
	fb_info.line_length = finfo.line_length;
	fb_info.size = finfo.smem_len;

	memcpy(&fb_info.red, &vinfo.red, sizeof(struct fb_bitfield));
	memcpy(&fb_info.green, &vinfo.green, sizeof(struct fb_bitfield));
	memcpy(&fb_info.blue, &vinfo.blue, sizeof(struct fb_bitfield));

	// printf("fb info x[%d] y[%d] x_v[%d] y_v[%d] xoffset[%d] yoffset[%d] bpp[%d] line_length[%ld] size[%ld]\n", fb_info.xres, fb_info.yres, fb_info.xres_virtual, fb_info.yres_virtual, fb_info.xoffset, fb_info.yoffset, fb_info.bpp, fb_info.line_length, fb_info.size);

	// printf("fb info red off[%d] len[%d] msb[%d]\n", fb_info.red.offset, fb_info.red.length, fb_info.red.msb_right);
	// printf("fb info green off[%d] len[%d] msb[%d]\n", fb_info.green.offset, fb_info.green.length, fb_info.green.msb_right);
	// printf("fb info blue off[%d] len[%d] msb[%d]\n", fb_info.blue.offset, fb_info.blue.length, fb_info.blue.msb_right);

	if (fb_info.bpp != 16 && fb_info.bpp != 24 && fb_info.bpp != 32) {
		printf("fb bpp is not 16,24 or 32\n");
		return -1;
	}

	if (fb_info.red.length > 8 || fb_info.green.length > 8 || fb_info.blue.length > 8) {
		printf("fb red|green|blue length is invalid\n");
		return -1;
	}

	// 内存映射
	fb_info.fbp = (char *)mmap(0, fb_info.size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_info.fbfd, 0);
	if (fb_info.fbp == (char *)-1) {
		printf("mmap fialed\n");
		return -1;
	}

	show_bmp(img_name);

	//删除映射
	munmap(fb_info.fbp, fb_info.size);

	return 0;
}

void *photo_thread(void){
	
	int rs = 0;
    int i = 0;
	
	while(1){
		
		usleep(50000);
		rs = GetDirection();
        printf("%d\n",rs);
        if (1 == rs || 3 == rs)
        {
            if (3 == i)
            {
                i = 0;
            }
            else
                ++i;
            show_picture(bmpname[i]);
			
            
        }
      
        else if (2 == rs || 4 == rs)
        {
            if (0 == i)
            {
                i = 3;
            }
            else
                --i;
            // bmp_player(i);
            show_picture(bmpname[i]);
        }




	 }
}

