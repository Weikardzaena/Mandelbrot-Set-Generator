/*
//	Weikarczaena's Mandelbrot Set Generator MandelbrotGenerator.h file.
//
//		Definitions of functions and objects used throughout execution
//		are contained in this file.
*/
#ifndef MANDELBROTGENERATOR_H
#define MANDELBROTGENERATOR_H

#include <fstream>
#include <vector>

using namespace std;

#pragma pack(push, 1)	/* Pushes the current packing alignment value on the internal compiler stack, and sets the current packing alignment value to 1*/
						/* This is necessary for how we will access the header information */

struct BMP
{
	BMP();										/* Constructor */

	struct
	{
		unsigned short		bfType;				/* Defines the type of bitmap.  99% of the time it will be 0x424D ("BM") */
		unsigned int		bfFileSize;			/* Size of the entire file in bytes. */
		unsigned short		bfReserved1;		/* Application specific */
		unsigned short		bfReserved2;		/*			" 			*/
		unsigned int		bfDataOffset;		/* Number of bytes from beginning of file to pixel data array */
	} BITMAPFILEHEADER;

	struct
	{
		unsigned int		biInfoSize;			/* Size of the info header (40 bytes if only non-optional information is included) */
		int					biWidth;			/* Image width in pixels (can be negative if you want to write image data from opposite side of image first) */
		int					biHeight;			/* Image height in pixels (										"										) */
		unsigned short		biPlanes;			/* Number of color planes (must be 1) */
		unsigned short		biColorDepth;		/* Bits per pixel */
		unsigned int		biCompression;		/* Type of compression to use */
		unsigned int		biImageSize;		/* Size of image data array in bytes (NOT file size) */
		int					biHorizRes;			/* Horizontal pixels per meter */
		int					biVertRes;			/* Vertical pixels per meter */
		unsigned int		biColorPalette;		/* Number of colors used */
		unsigned int		biImportantColors;	/* Number of important colors (0 means all are important). Generally ignored. */
	} BITMAPFILEINFO;
};

#pragma pack(pop)							/* Removes the record from the top of the internal compiler stack */

/* Function Prototypes */
unsigned int 		getBufferLength (unsigned int xRes, BMP bmp);
BMP					setDimensions (unsigned int xRes, unsigned int yRes, BMP bmp);
BMP 				fileSize (unsigned int rowSize, BMP bmp);
void				normalize (vector<vector<unsigned int>> &data, vector<vector<bool>> &escape, unsigned int length, unsigned int iterations);
void				hsvToRGB (vector<vector<char>> &colorData, vector<vector<unsigned int>> &hue, vector<vector<bool>> &escape, unsigned int pixelCount);
void 				writeBMP (vector<vector<char>> &buffer, ofstream &bmpPtr, unsigned int bufferLength, unsigned int pixelCount);
string 				fileSizeToString(unsigned int size);

#endif //MANDELBROTGENERATOR_H

