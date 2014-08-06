#ifndef _UNPACK_H
#define _UNPACK_H

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	char header[8];			//always the same
	char unknown_1[24]; 	//always the same
	char file_type[12]; 	//used to determine rgb565 or rgb555 format
	char unknown_2[4];		//always the same
	uint32_t file_size;		//file size in bytes
	char unknown_3[4];		//always the same
	uint32_t terrain_ptr;	//ptr to terrain header
	uint32_t gui_ptr;		//ptr to gui images
	uint32_t objects_ptr;	//ptr to objects header
	uint32_t outfit_ptr;	//ptr to outfit images
	uint32_t shadows_ptr;	//ptr to outfit images' shadows
	uint32_t unknown_ptr_1;	//ptr to unknown header
	uint32_t unknown_ptr_2;	//ptr to unknown header
} s3_dat_main_header;

typedef struct
{
	char header[6];
	uint16_t count;	//this number determines how many addresses
	//are after this header (in uint32_t format)
	// == how many images/animations
} s3_dat_subheader;

typedef struct
{
	s3_dat_main_header main_header;
	s3_dat_subheader terrain_header;
	s3_dat_subheader gui_header;
	s3_dat_subheader objects_header;
	s3_dat_subheader outfit_header;
	s3_dat_subheader shadows_header;
	s3_dat_subheader unknown_header_1;
	s3_dat_subheader unknown_header_2;
} s3_dat_file_format;


#pragma pack(pop)


#endif

