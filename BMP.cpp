/*******************************************************************************

	Copyright (C) 2015 by G. Nikolai "Weikardzaena" Kotula
	<limitatinfinity11@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include <cmath>
#include <iostream>
#include <vector>

#include "MandelbrotGenerator.h"

using namespace std;

/* BMP constructor */
BMP::BMP()
{
	BITMAPFILEHEADER.bfType = 0x4D42; // Characters "BM" to denote "bitmap"
	BITMAPFILEHEADER.bfFileSize = 0x0; // Size of file in bytes
	BITMAPFILEHEADER.bfReserved1 = 0x0; // Application specific
	BITMAPFILEHEADER.bfReserved2 = 0x0; // Applicatino specific
	BITMAPFILEHEADER.bfDataOffset = 0x36; // How far from BOF to Data
	BITMAPFILEINFO.biInfoSize = 0x28; // Number of bytes in the info section
	BITMAPFILEINFO.biWidth = 0x0; // Width of image in pixels
	BITMAPFILEINFO.biHeight = 0x0; // Height of image in pixels
	BITMAPFILEINFO.biPlanes = 0x1; // Number of color planes (almost always 1)
	BITMAPFILEINFO.biColorDepth = 0x18; // 1 byte per color, 3 colors per pixel = 24 bits / pixel
	BITMAPFILEINFO.biCompression = 0x0; // Compression level (0 = no compression)
	BITMAPFILEINFO.biImageSize = 0x0; // Size of the image in bytes
	BITMAPFILEINFO.biHorizRes = 0x130B; // 2853 pixels per meter = 72 DPI
	BITMAPFILEINFO.biVertRes = 0x130B; // 2853 pixels per meter = 72 DPI
	BITMAPFILEINFO.biColorPalette = 0x0; // No special color palette
	BITMAPFILEINFO.biImportantColors = 0x0; // No important colors
}

BMP bmp;

unsigned int getBufferLength (unsigned int pixelCount, BMP bmp)
{
	/* This calculates the least number of pixels needed to have an integer
	 * multiple of 4 bytes per row. */
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
	/* Overload this because of how big biHeight can be */
	bmp.BITMAPFILEINFO.biImageSize = rowSize * abs((long long)bmp.BITMAPFILEINFO.biHeight);
	bmp.BITMAPFILEHEADER.bfFileSize = sizeof(bmp) + bmp.BITMAPFILEINFO.biImageSize;

	return bmp;
}

/**
 * WRITES BUFFER TO BMP
 * NOTE: BMP file types read pixels as BGR not RGB, so we are flipping the color
 * scheme here, but I like the blue for the set more than the red that it would
 * be if we corrected for it so I'm leaving it alone.
 */
void writeBMP(vector<vector<char>> &buffer, ofstream &bmpPtr, unsigned int bufferLength, unsigned int pixelCount)
{
	for (unsigned int j = 0; j < pixelCount; ++j) {
		for (unsigned int i = 0; i < bufferLength; ++i) {
			bmpPtr.write(&buffer.at(j).at(i), 1);
		}
	}
}
