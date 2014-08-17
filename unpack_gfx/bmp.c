#include "includes.h"


#pragma pack(push)
#pragma pack(1)

typedef struct
{
	uint16_t bfType;  //specifies the file type
	uint32_t bfSize;  //specifies the size in bytes of the bitmap file
	uint16_t bfReserved1;  //reserved; must be 0
	uint16_t bfReserved2;  //reserved; must be 0
	uint32_t bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
} BITMAPFILEHEADER;


typedef struct
{
	uint32_t biSize;  //size of this header
	uint32_t biWidth;  //specifies width in pixels
	uint32_t biHeight;  //species height in pixels
	uint16_t biPlanes; //specifies the number of color planes, must be 1
	uint16_t biBitCount; //specifies the number of bit per pixel
	uint32_t biCompression;//spcifies the type of compression
	uint32_t biSizeImage;  //size of image in bytes
	uint32_t biXPelsPerMeter;  //number of pixels per meter in x axis
	uint32_t biYPelsPerMeter;  //number of pixels per meter in y axis
	uint32_t biClrUsed;  //number of colors used by th ebitmap
	uint32_t biClrImportant;  //number of colors that are important
} BITMAPINFOHEADER;

typedef struct
{
	uint32_t biSize;  //size of this header
	uint32_t biWidth;  //specifies width in pixels
	uint32_t biHeight;  //species height in pixels
	uint16_t biPlanes; //specifies the number of color planes, must be 1
	uint16_t biBitCount; //specifies the number of bit per pixel
	uint32_t biCompression;//spcifies the type of compression
	uint32_t biSizeImage;  //size of image in bytes
	uint32_t biXPelsPerMeter;  //number of pixels per meter in x axis
	uint32_t biYPelsPerMeter;  //number of pixels per meter in y axis
	uint32_t biClrUsed;  //number of colors used by th ebitmap
	uint32_t biClrImportant;  //number of colors that are important
	uint32_t biRedMask; //mask for red color, 0x00 0x00 0x00 0xff
	uint32_t biGreenMask; //mask for green color, 0x00 0x00 0xff 0x00
	uint32_t biBlueMask; //mask for blue color, 0x00 0xff 0x00 0x00
	uint32_t biAlphaMask; //mask for alpha color, 0xff 0x00 0x00 0x00
} BITMAPV3INFOHEADER;

#pragma pack(pop)


//standard 24-bit rgb bitmap
bool write_rgb_bitmap_to_file
(char *file_name, uint16_t width, uint16_t height, void *bitmap_ptr)
{
	BITMAPFILEHEADER bmf;
	BITMAPINFOHEADER bmi;
	FILE *bitmap = fopen(file_name, "wb");

	if (!bitmap)
	{
		printf("Error opening %s: %s\n", file_name, strerror(errno));
		return false;
	}


	//init bitmap beader structures
	memset(&bmf, 0, sizeof(BITMAPFILEHEADER));
	memset(&bmi, 0, sizeof(BITMAPINFOHEADER));

	bmf.bfType = 0x4d42;
	bmf.bfSize = width*height*4+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bmf.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = width;
	bmi.biHeight = height;
	bmi.biPlanes = 1;
	bmi.biBitCount = 24;


	//write headers
	fwrite(&bmf, sizeof(BITMAPFILEHEADER), 1, bitmap);
	fwrite(&bmi, sizeof(BITMAPINFOHEADER), 1, bitmap);

	//write pixel lines in reverse order
	//printf("width szerokosc=%d\nheight wysokosc=%d\n", width, height);
	for (int i=height*width-width; i>=0; i-=width)
	{
		//printf("i=%d, bitmap_ptr+i=%d, width*4=%d\n", i*4, (char*)bitmap_ptr+i*4, width*4);
		fwrite((char*)bitmap_ptr+i*3, 3, width, bitmap);
	}

	//pixel format in file: BGR

	if (bitmap)
		fclose(bitmap);

	return true;
}





//32-bit bitmap with 8-bit alpha channel
bool write_rgba_bitmap_to_file
(char *file_name, uint16_t width, uint16_t height, void *bitmap_ptr)
{
	BITMAPFILEHEADER bmf;
	BITMAPV3INFOHEADER bmi;
	FILE *bitmap = fopen(file_name, "wb");

	if (!bitmap)
	{
		printf("Error opening %s: %s\n", file_name, strerror(errno));
		return false;
	}


	//init bitmap beader structures
	memset(&bmf, 0, sizeof(BITMAPFILEHEADER));
	memset(&bmi, 0, sizeof(BITMAPV3INFOHEADER));

	bmf.bfType = 0x4d42;
	bmf.bfSize = width*height*4+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPV3INFOHEADER);
	bmf.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPV3INFOHEADER);

	bmi.biSize = sizeof(BITMAPV3INFOHEADER);
	bmi.biWidth = width;
	bmi.biHeight = height;
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biCompression = 3;
	bmi.biRedMask = 0xff000000;
	bmi.biGreenMask = 0x00ff0000;
	bmi.biBlueMask = 0x0000ff00;
	bmi.biAlphaMask = 0x000000ff;

	//write headers
	fwrite(&bmf, sizeof(BITMAPFILEHEADER), 1, bitmap);
	fwrite(&bmi, sizeof(BITMAPV3INFOHEADER), 1, bitmap);

	//write pixel lines in reverse order
	//printf("width szerokosc=%d\nheight wysokosc=%d\n", width, height);
	for (int i=height*width-width; i>=0; i-=width)
	{
		//printf("i=%d, bitmap_ptr+i=%d, width*4=%d\n", i*4, (char*)bitmap_ptr+i*4, width*4);
		fwrite((char*)bitmap_ptr+i*4, 4, width, bitmap);
	}

	//pixel format in file: ABGR

	if (bitmap)
		fclose(bitmap);

	return true;
}


