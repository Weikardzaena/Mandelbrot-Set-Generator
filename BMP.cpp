/*
//	Weikarczaena's Mandelbrot Set Generator BMP.cpp file.
//
//		All methods, structs, etc, for dealing with
//		the bitmap I/O is contained in this file.
*/
#include <cmath>
#include <iostream>

#include "MandelbrotGenerator.h"

using namespace std;

/* BMP constructor */
BMP::BMP()
{
	BITMAPFILEHEADER.bfType = 0x4D42;			/* "BM" (for some reason this is written to the file backwards... not sure why) */
												/* 		The real value on file is 0x424D. */
	BITMAPFILEHEADER.bfFileSize = 0x0;			/* Will reset this number after input recieved */
	BITMAPFILEHEADER.bfReserved1 = 0x0;
	BITMAPFILEHEADER.bfReserved2 = 0x0;
	BITMAPFILEHEADER.bfDataOffset = 0x36;		/* 40 + 14 bytes */
	BITMAPFILEINFO.biInfoSize = 0x28;			/* 40 bytes */
	BITMAPFILEINFO.biWidth = 0x0;				/* Will resize this when input recieved */
	BITMAPFILEINFO.biHeight = 0x0;				/* "									" */
	BITMAPFILEINFO.biPlanes = 0x1;
	BITMAPFILEINFO.biColorDepth = 0x18;			/* 1 byte per color, 3 colors per pixel = 24 bits / pixel */
	BITMAPFILEINFO.biCompression = 0x0;			/* no compression */
	BITMAPFILEINFO.biImageSize = 0x0;			/* Will get this from user */
	BITMAPFILEINFO.biHorizRes = 0x130B;			/* 2853 pixels per meter (72 DPI) */
	BITMAPFILEINFO.biVertRes = 0x130B;			/* "							" */
	BITMAPFILEINFO.biColorPalette = 0x0;
	BITMAPFILEINFO.biImportantColors = 0x0;
}

BMP bmp;

unsigned int getBufferLength (unsigned int pixelCount, BMP bmp)
{
	/* This calculates the least number of pixels needed to have an integer multiple of 4 bytes per row. */
	unsigned int rowSize = (unsigned int)floor((float)(bmp.BITMAPFILEINFO.biColorDepth * bmp.BITMAPFILEINFO.biWidth + 31) / 32) * 4;
	
	return rowSize;
}

/* SET DIMENSIONS */

BMP setDimensions(unsigned int xRes, unsigned int yRes, BMP bmp)
{
	/* Get input on size of file */
	bmp.BITMAPFILEINFO.biWidth = xRes;
	bmp.BITMAPFILEINFO.biHeight = yRes;
	
	return bmp;
}

/* SET FILE SIZE  */
BMP fileSize (unsigned int rowSize, BMP bmp)
{
	/* Set image and file sizes in structs */
	bmp.BITMAPFILEINFO.biImageSize = rowSize * abs((long long)bmp.BITMAPFILEINFO.biHeight); /* Overload this because of how big biHeight can be */
	bmp.BITMAPFILEHEADER.bfFileSize = sizeof(bmp) + bmp.BITMAPFILEINFO.biImageSize;
		
	return bmp;
}

/* 	WRITE BUFFER TO BMP
//	NOTE:	.BMP file types read pixels as BGR not RGB, so we are flipping the color scheme here,
// 			but I like the blue for the set more than the red that it would be if we corrected for it
//			so I'm leaving it alone.
*/
void writeBMP(char *buffer, ofstream &bmpPtr, unsigned int bufferLength)
{
	bmpPtr.write(buffer, bufferLength);
}
