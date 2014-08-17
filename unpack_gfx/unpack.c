#include "includes.h"
#include "unpack.h"


/*
 * Settlers 3 dat file format
 * File contains:
 * * main header which inform about file content
 * * subheaders with pointers to data sequences
 * * data sequenes which are:
 * * * header
 * * * count
 *
 * Only basic error handling are implemented!
 */

char* my_itoa(long long input)
{
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define snprintf _snprintf
#endif
	unsigned long long len = 1;
	long long bufInput = (input < 0) ? -input : input;

	//additional minus sign
	if (input < 0)
		++len;

	while (bufInput)
	{
		++len;
		bufInput /= 10;
	}

	char *bufStr = calloc(sizeof(char), ++len); //len+1 for ending zero
	snprintf(bufStr, len, "%lld", input);
	return bufStr;
}
FILE *file = NULL;

void exit_exe(void)
{
	if (file)
		fclose(file);
}

void read_image(uint32_t address, char* file_name)
{
	uint16_t x;
	uint16_t y;
	uint16_t unknown;
	uint16_t input;
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint32_t loc;

	uint8_t pxCount;
	int8_t dist;


	fseek(file, address, 0);

	fread(&x, sizeof(x), 1, file);
	fread(&y, sizeof(y), 1, file);
	fread(&unknown, sizeof(unknown), 1, file);

	loc=0;
	char *mem = calloc(x*y, 5);

	/*
	   fread(&pxCount, sizeof(pxCount), 1, file);
	   fread(&dist, sizeof(dist), 1, file);
	   printf("%dx%d %s\n", x, y, file_name);
	   printf("pxCnt=%d, dist=%d\n", pxCount, dist);

	   for (unsigned j=0; j<y; ++j)
	   {
	   for (unsigned i=0; i<x; ++i)
	   {
	// Pixel is stored as uint16_t which is rgb565 format.
	// that means subpixels are stored as below:
	// |R|R|R|R|R|G|G|G|G|G|G|B|B|B|B|B|
	// Red - 5 bits, Green - 6 bits, Blue - 5 bits
	// here we use bitwise operations to extract these values

	fread(&input, sizeof(input), 1, file);

	blue = (input&0x1f) << 3;
	green = (input&0x7e0) >> 3;
	red = (input&0xf800) >> 8;

	 *(mem+loc++) = blue;
	 *(mem+loc++) = green;
	 *(mem+loc++) = red;
	 }
	 if (j != y-1)
	 {
	 fread(&pxCount, sizeof(pxCount), 1, file);
	 fread(&dist, sizeof(dist), 1, file);
	 printf("pxCnt=%d, dist=%d\n", pxCount, dist);
	 }
	 }
	 */

	unsigned pixels = 0;
	do//for (unsigned height=0; height<y*2; ++height)
	{
		fread(&pxCount, sizeof(pxCount), 1, file);
		fread(&dist, sizeof(dist), 1, file);
		if (dist < 0) dist+=128;
		//printf("pxCnt=%d, dist=%d\n", pxCount, dist);
		//
		/*
		for (unsigned pixel=0; pixel<dist; ++pixel)
		{
			*(mem+loc++) = 0x00;
			*(mem+loc++) = 0x00;
			*(mem+loc++) = 0x00;
			*(mem+loc++) = 0x00;
			++pixels;
		}
*/
		//loc += dist*3;
		loc += dist*4; ///!!!!!!!!!!
		pixels += dist;
		for (unsigned pixel=0; pixel<pxCount; ++pixel)
		{
			fread(&input, sizeof(input), 1, file);

			blue = (input&0x1f) << 3;
			green = (input&0x7e0) >> 3;
			red = (input&0xf800) >> 8;

			*(mem+loc++) = 0xff;
			*(mem+loc++) = blue;
			*(mem+loc++) = green;
			*(mem+loc++) = red;
			++pixels;
		}


	}
	while (pixels < x*y);

	write_rgba_bitmap_to_file(file_name, x, y, mem);

	free(mem);
}

int main(int argc, char **argv)
{

	s3_dat_file_format file_data;

	atexit(exit_exe);
	printf("Settlers 3 dat unpacker v.0.1 by kkthx.pl\n");

	if (argc < 2)
	{
		printf("Usage: unpack <setters 3 dat filename>\n");
		return 1;
	}

	file = fopen(argv[1], "rb");

	if (!file)
	{
		printf("Cannot open file \"%s\": %s", argv[1], strerror(errno));
		return 1;
	}

	fread(&file_data.main_header, sizeof(s3_dat_main_header), 1, file);
	if (memcmp(file_data.main_header.header, "\x04\x13\x04\x00\x0c\x00\x00\x00", \
				sizeof(file_data.main_header.header)) != 0)
	{
		printf("%s: Unknown file\n", argv[1]);
		return 1;
	}

	fseek(file, file_data.main_header.terrain_ptr, 0);
	fread(&file_data.terrain_header, sizeof(s3_dat_subheader), 1, file);
	fseek(file, file_data.main_header.gui_ptr, 0);
	fread(&file_data.gui_header, sizeof(s3_dat_subheader), 1, file);
	fseek(file, file_data.main_header.objects_ptr, 0);
	fread(&file_data.objects_header, sizeof(s3_dat_subheader), 1, file);
	fseek(file, file_data.main_header.outfit_ptr, 0);
	fread(&file_data.outfit_header, sizeof(s3_dat_subheader), 1, file);
	fseek(file, file_data.main_header.shadows_ptr, 0);
	fread(&file_data.shadows_header, sizeof(s3_dat_subheader), 1, file);
	fseek(file, file_data.main_header.unknown_ptr_1, 0);
	fread(&file_data.unknown_header_1, sizeof(s3_dat_subheader), 1, file);
	fseek(file, file_data.main_header.unknown_ptr_2, 0);
	fread(&file_data.unknown_header_2, sizeof(s3_dat_subheader), 1, file);

	printf("%s  ", argv[1]);
	printf("%.2lfkB\n\n", (float)file_data.main_header.file_size/1024);
	printf("%3d Terrain\n", file_data.terrain_header.count);
	printf("%3d GUI\n", file_data.gui_header.count);
	printf("%3d Object\n", file_data.objects_header.count);
	printf("%3d Outfit\n", file_data.outfit_header.count);
	printf("%3d Shadow\n", file_data.shadows_header.count);
	printf("%3d Unknown 1\n", file_data.unknown_header_1.count);
	printf("%3d Unknown 2\n", file_data.unknown_header_2.count);


	uint32_t c;
	uint32_t address;
	c=0;
	for (unsigned i=0; i<file_data.terrain_header.count; ++i)
	{

		fseek(file, file_data.main_header.terrain_ptr+8+i*4, 0);
		fread(&address, sizeof(address), 1, file);
		char file_name[16] = "";
		strcat(file_name, my_itoa(i+1));
		strcat(file_name, "terr.bmp");
		//printf("adres=%d\n", file_data.main_header.terrain_ptr);
		//printf("adres=%d\n", address);
		read_image(address, file_name);
		++c;
	}
	printf("%d Terrains unpacked\n", c);



	c=0;
	for (unsigned i=0; i<file_data.gui_header.count; ++i)
	{

		fseek(file, file_data.main_header.gui_ptr+8+i*4, 0);
		fread(&address, sizeof(address), 1, file);
		char file_name[16] = "";
		strcat(file_name, my_itoa(i+1));
		strcat(file_name, "gui.bmp");
		//printf("adres=%d\n", file_data.main_header.terrain_ptr);
		//printf("adres=%d\n", address);
		read_image(address, file_name);
		printf("%d\r", c);
		++c;
		
	}
	printf("%d GUIs unpacked\n", c);




	return 0;
}
