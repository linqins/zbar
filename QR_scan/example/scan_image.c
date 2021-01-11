#include <stdio.h>
#include <stdlib.h>
#include "png.h"
#include "../include/zbar.h"


zbar_image_scanner_t *scanner = NULL;

/* to complete a runnable example, this abbreviated implementation of
 * get_data() will use libpng to read an image file. refer to libpng
 * documentation for details
 */

#if 1
 void get_data (const char *name,
                      int *width, int *height,
                      void **raw)
{
    FILE *file = fopen(name, "rb");
    if(!file) exit(2);
    png_structp png =
        png_create_read_struct(PNG_LIBPNG_VER_STRING,
                               NULL, NULL, NULL);
    if(!png) exit(3);
    if(setjmp(png_jmpbuf(png))) exit(4);
    png_infop info = png_create_info_struct(png);
    if(!info) exit(5);
    png_init_io(png, file);        //将png与文件相匹配
    png_read_info(png, info);      //png信息读到info中
    /* configure for 8bpp grayscale input */
    int color = png_get_color_type(png, info);
    int bits = png_get_bit_depth(png, info);
    if(color & PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if(color == PNG_COLOR_TYPE_GRAY && bits < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if(bits == 16)
        png_set_strip_16(png);
    if(color & PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png);
    if(color & PNG_COLOR_MASK_COLOR)
        png_set_rgb_to_gray_fixed(png, 1, -1, -1);
    /* allocate image */
    *width = png_get_image_width(png, info);
    *height = png_get_image_height(png, info);

	printf("w=%d H=%d \n",*width,*height);

	
    *raw = malloc(*width * *height);
    png_bytep rows[*height];
    int i;
    for(i = 0; i < *height; i++)
    {
        rows[i] = *raw + (*width * i);
		//printf("row[%d]=[%p]  \n",i,*raw+(*width * i));
		//printf("rows[%d]=[%p] \n",i,rows[i]);
    }
	
    png_read_image(png, rows);// png读取图像信息，将图像信息写入rows中          此处的rows[]与row指向同一片地址空间
}
#endif


int main (int argc, char **argv)
{

    int width = 0, height =0;
    void *raw = NULL;


    if(argc < 2) 
    	return(1);

    /* create a reader */
    scanner = zbar_image_scanner_create();   //创建阅读器

    /* configure the reader */
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);  //阅读器配置

    /* obtain image data */

	//raw = malloc(width * height);
    get_data(argv[1], &width, &height, &raw);  //获取图片信息
	printf("width = %d, height = %d\n", width, height);
	//printf("raw=%s \n",raw);

    /* wrap image data */
    zbar_image_t *image = zbar_image_create();  //创建图像
    zbar_image_set_format(image, *(int*)"Y800");  //设置图像格式
    zbar_image_set_size(image, width, height);  //设置图像的像素
    zbar_image_set_data(image, raw, width * height, zbar_image_free_data); //将获取的图片信息传入创建的图像中

    /* scan the image for barcodes */
    int n = zbar_scan_image(scanner, image); //扫描图像

    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);//提取图像扫描结果
    for(; symbol; symbol = zbar_symbol_next(symbol)) {
        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);//提取数据
        printf("decoded %s symbol:[%s] \n",
               zbar_get_symbol_name(typ), data);
    }

    /* clean up */
    zbar_image_destroy(image);//销毁图像
    zbar_image_scanner_destroy(scanner);//销毁阅读器

    return(0);
}
