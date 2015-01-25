/*
//	Weikarczaena's Mandelbrot Set Generator main.cpp file.
//
//		This file is where all the core execution is laid out. All other files
//		simply provide the function definitions and variables used here.
//
//	-------------------------
//
//	Version Info
//
//		Current Version:	1.0.3 (2/4/2014)
//
//		Info:
//
//			Version 1.0.X asks the user for the number of pixels they want the
//			bitmap to be and also asks them for the number of iterations
//			they want for each pixel. It then runs through the iterations
//			on a range found below denoted by "xStart", "xEnd", "yStart", "yEnd"
//			It does this row by row each time writing a series of RGB
//			values to a 1D buffer array which it then writes to the file.
//
//		Version History:
//
//			1.0.3 -	Fixed issue with main calculation loop where sometimes it
//					would try to access items of the buffers that we did not
//					allocate memory for (main.cpp).
//
//					Also fixed an issue with the RGB conversion where sometimes
//					the pixel would be painted black in the bmp when it wasn't
//					part of the set (RGB.cpp, main.cpp).
//
//					Also fixed an issue with the function that tells the user
//					how big the file size will be in MB, GB, etc (main.cpp).
//
//			1.0.2 -	Changed user input method to string manipulation to eliminate
//					the possibility of execution doing strange things when
//					the user input a non-numeric character or a number
//					that was too large for the variable type (main.cpp).
//	
//			1.0.1 -	Fixed issue with inaccurate file size determination
//					for large resolutions (BMP.cpp).
//	
//			1.0.0 -	Execution works and is stable. Yay!
//
//	-------------------------
//
//
//	Notes/Todo (for current version)
//
//		- 	In BMP.cpp, the file type is defined to be 0x4D42 
//			(the "BM" at the beginning of almost every BMP), but "BM"
//			SHOULD BE 0x424D. In fact, it is written to the file
//			as 0x424D instead of 0x4D42, which is -- needless to
//			say -- strange. It is unclear why this is happening
//			because the rest of the header is written appropriately...
//			It needs to be looked into, but right now the execution works
//			on Ubuntu 12.04 64 bit and Windows 7 Home Premium 64 Bit.
//
//		-	Right now the application does not use a lot of memory which
//			means the buffer arrays could be made bigger depending on
//			how much free memory the user has at the time. Implimenting
//			this would be cool so there's an optimal amount of disk I/O
//			and memory usage.
//
//		- 	The main calculation loop can definitely be optimized.
//			Right now the code only knows if a pixel is a member
//			of the set if the max iterations are hit. But we know that
//			the block of pixels in the center region of the complex
//			plane are always members of the set, so ideally we'd
//			want to skip the calculations for these values.
//
//		- 	The string manipulation for getting the user input may be 
//			ugly, but it deals with non-number inputs nicely.
//			If there's a better way to do it, please go ahead.
//
//		-	Right now the application on a whole is rather limited.
//			It would be cool to have an interactive GUI that refreshes
//			the set as you zoom in. The first step toward that I think
//			will be getting a user to input coordinates to zoom in on
//			each time execution is run.
//
//		-	Multithreading the calculation loop would be awesome.
//
//		-	Actually .BMP files are read from bottom to top, so we are
//			flipping the image about the X axis, but it doesn't matter since
//			the set is symmetric about the X axis. Also, .BMP reads
//			BGR instead of RGB so we are flipping the color scheme when
//			writing to the bitmap. This could be fixed, but since the RGB 
//			conversion spits out red for small hues and blue for large values,
//			we'd have to flip the values in the color buffer AND flip the order
//			in which we write to the file which is not good for computation times.
//			So we are abusing the BGR interpretation to skip these steps.
//			It's not the "right" way to do it, but it saves a bit of run time.
//
//	-------------------------
//
//	DISCLAIMER: This software has not been thouroughly tested on different systems.
//				On the computer that compiled the code, the application is stable,
//				but by no means do we guarantee this to work on any machine.
//				Moreover, with how resource intensive this application can be,
//				running it may cause instability on your computer. USE AT YOUR
//				OWN RISK.
//				
//	LEGAL:		I give this code out to the internet freely so long as you
//				cite me as the original author of the code in any future
//				renditions (yes, my screen name is fine... There is only one
//				Weikarczaena).
*/

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <string>
#include <new>
#include <cctype>
#include <limits>

#include "MandelbrotGenerator.h"

using namespace std;

/* Searches string for any non-digit characters, and returns false if there are non-digit characters */
bool isNumber (string str)
{
	return str.find_first_not_of("0123456789") == string::npos;
}

string fileSizeToString(unsigned int size)
{
	string str;
	int exp = (int)log10 (size);
	int ind = (int)exp / 3;
	float dec = (float)size / pow(10, 3 * ind);
	
	/* A switch-case would work here if we didn't have the ind > 8 at the very end there... */
	if (ind == 0)
		str = "Are you sure? This will take up approximately " + to_string(dec) + " Bytes of disk space [y|n] ";
	else if (ind == 1)
		str = "Are you sure? This will take up approximately " + to_string(dec) + " KB of disk space [y|n] ";
	else if (ind == 2)
		str = "Are you sure? This will take up approximately " + to_string(dec) + " MB of disk space [y|n] ";
	else if (ind == 3)
		str = "Are you sure? This will take up approximately " + to_string(dec) + " GB of disk space [y|n] ";
	else if (ind == 4)
		str = "Are you sure? This will take up approximately " + to_string(dec) + " TB (10^3 GB) of disk space [y|n] "; /* with .BMP, the largest file size allowed is 4.3 GB, but I'll keep the rest here for future changes */
	else if (ind == 5)
		str = "Are you sure? This will take up approximately " + to_string(dec) + " PB (10^6 GB) of disk space [y|n] ";
	else if (ind == 6)
		str = "Are you sure? This will take up approximately " + to_string(dec) + " EB (10^9 GB) of disk space [y|n] ";
	else if (ind == 7)
		str = "Are you sure? This will take up approximately " + to_string(dec) + " ZB (10^12 GB) of disk space [y|n] ";
	else if (ind == 8)
		str = "Are you sure? This will take up approximately " + to_string(dec) + " YB (10^15 GB) of disk space [y|n] ";
	else if (ind > 8)
		str = "There is literally no metric prefix large enough to describe the size of this file you are trying to create... You should really reconsider your input. Are you sure you want to continue [y/n] ";
	else
		str = "oops, something went wrong with the file size calculation... You should abort to be safe"; 
	
	return str;
}

/* MAIN FUNCTION */
int main()
{
	/* Forward Definitions of Variables */
	const string currentVersion = "1.0.3";

	bool outOfBounds, *escapeBuffer;
	string proceed = "n", size, userInput;
	char *colorBuffer;
	unsigned int i, pixelCount = 0xffffffff, xRes, yRes, bufferLength, count, *iterationBuffer, percent, iterations = 0xffffffff, k;
	double Z, Zi, Zp, Zip;
	const float  xStart = -2, xEnd = 2, yStart = -2, yEnd = 2; /* Can change these values */
	float xStep, yStep, xPos, yPos;

	BMP bitmapData;	/* Constructor is called from BMP.cpp */
	
	ofstream dataFile;
	
	printf("\n--------------------\n\n	Weikarczaena's Mandelbrot Set Generator\n\n	Version %s (Email limitAtInfinity11@gmail.com for bug reports and suggestions)\n\nDISCLAIMER: This software has not been thouroughly tested on different systems. On the computer that compiled the code, the application is stable, but by no means do we guarantee this to work on any machine. Moreover, with how resource intensive this application can be, running it may cause instability on your computer. USE AT YOUR OWN RISK.\n\nThis application will create a raw bitmap (.BMP) of the Mandelbrot Set (colored) within a complex radius of 2 from the origin.\n\nThe number of iterations and the resolution of the image will be up to you, but note that once you start picking large resolutions and high iteration values the computation time and image size increases DRAMATICALLY.\n\nHave fun with it! Press Ctrl+C at any time to abort the program.\n\n--------------------\n", currentVersion.c_str());
	while (proceed != "y" && proceed != "Y")
	{
		while (pixelCount > 37800)
		{
			pixelCount = 0xffffffff; /* Must reset this value every time in case 
										that user inputs a value, selects "no" on 
										the size confirmation, and then inputs something 
										like a non-character entry because otherwise it 
										would continue on the last known value of pixelCount 
										without accepting the user's input */
			
			printf("\nNumber of pixels wide (max is 37,800)? "); /* This ensures the file size cannot exceed the max value for an unsigned int (with some wiggle room) */
			cin >> userInput;
			
			if (cin.fail())
			{
				printf("The input stream failed to write to the string... That shouldn't ever happen but it did. Execution terminated. Press 'enter' to continue.\n");
				cin.sync();
				cin.ignore();
				return -1;
			}
			
			if (!isNumber (userInput))
				printf("\nOops, you can't include any non-number characters (remember, no commas or anything). Try again.\n");
			else
			{
				/* 	We need this if statement in the case that the user inputs something larger than the 
					max value for double. If we don't do this, stod will return nonsense values. */
				if (userInput.length() < 15) /* Max length of double is 15 digits in base-10 */
				{
					if (stod (userInput) < 0xffffffff)
						pixelCount = (unsigned int)stod (userInput);
					else
						pixelCount = 0xffffffff;
				}
				else
				{
					pixelCount = 0xffffffff;
				}
				
				if (pixelCount > 37800)
					printf("\nSorry, that number was too large. Try again.");
			}
		}
		
		xRes = pixelCount; 
		yRes = pixelCount * 1; /* This is redundant for equal x and y ranges, but it allows for a non-square picture and keeps the aspect ratio the same. Just change the '1' appropriately */
													
		/* Set the image dimensions based on the input */
		bitmapData = setDimensions(xRes, yRes, bitmapData);

		/* Get the length of the buffer so the data is padded to integer multiples of 4 bytes (which is necessary for .BMP) */
		bufferLength = getBufferLength(xRes, bitmapData);

		/* Set File and Image Sizes in bitmap struct */
		bitmapData = fileSize(bufferLength, bitmapData);

		size = fileSizeToString(bitmapData.BITMAPFILEHEADER.bfFileSize);

		printf("\n%s", size.c_str());
		cin >> proceed;
		
	}
	
	while (iterations > 4294967294)
	{
		iterations = 0xffffffff;
		
		printf("\nNumber of iterations (max is 4,294,967,294)? ");
		cin >> userInput;
		
		if (cin.fail())
		{
			printf("The input stream failed to write to the string... That shouldn't ever happen but it did. Execution terminated. Press 'enter' to continue.\n");
			cin.ignore();
			return -1;
		}
		
		if (!isNumber (userInput))
			printf("\nOops, you can't include any non-number characters. Try again.\n");
		else
		{
			/* 	We need this if statement in the case that the user inputs something larger than the max value 
				for double. Otherwise stod will return nonsense values. */
			if (userInput.length() < 15) /* Max length of double is 15 digits in base-10 */
			{
				if (stod (userInput) < 0xffffffff)
					iterations = (unsigned int)stod (userInput);
				else
					iterations = 0xffffffff;
			}
			else
			{
				iterations = 0xffffffff;
			}

			if (iterations > 4294967294)
				printf("\nSorry, that number was too large. Try again.");
		}
	}
	
	printf("\nOpening data stream to 'MandelbrotSet.bmp' (it will be in the same folder that you launched this application from)...\n");

	dataFile.open("MandelbrotSet.bmp", ofstream::out | ofstream::trunc | ofstream::binary);

	if (dataFile.fail())
		{
			printf("Could not open MandelbrotSet.bmp! Press 'enter' to exit.\n");
			cin.sync();
			cin.ignore();
			return -1;
		}

	printf("\nDone.\n");

	printf("\nAllocating memory for data arrays...\n");

	colorBuffer = 		new (nothrow) char [bufferLength];
	iterationBuffer =	new (nothrow) unsigned int [pixelCount];
	escapeBuffer = 		new (nothrow) bool [pixelCount];

	if (colorBuffer == 0 || iterationBuffer == 0)
	{
		printf("Error in memory allocation for data array. Execution terminated. Press 'enter' to exit\n");
		cin.sync();
		cin.ignore();
		return -1;
	}

	/* Initialize buffer with all zeroes.  This allows the padded bytes to be defined in the file. */
	for (i = 0; i < bufferLength; ++i)
	{
		colorBuffer[i] = 0x0;
	}
	
	for (i = 0; i < pixelCount; ++i)
	{
		iterationBuffer[i] = 0x0;
		escapeBuffer[i] = false;
	}
	
	printf("\nDone.\n\n");

	printf("Now executing calculations and compiling data into MandelbrotSet.bmp...\n\nNote that in the 30%% to 70%% range the program may seem to slow down; this is normal. This is due to the majority of the mandelbrot set being contained around the origin, so more iterations are needed toward the middle.\n\n");

	/* Write the file header */
	dataFile.seekp(ios_base::beg);
	dataFile.write(reinterpret_cast <char*>(&bitmapData), sizeof(bitmapData));

	/* Set miscellaneous values needed for calculation */
	xStep = (float)(xEnd - xStart)/xRes;
	yStep = (float)(yEnd - yStart)/yRes;
	
	yPos = yStart;

	/* Main calculation loop */
	for (i = 0; i < pixelCount; ++i)
	{
		percent = int(fabs(yPos-yStart)/(yEnd - yStart) * 100);
		printf("\r%d%%",percent);
		
		xPos = xStart;
		
		for (count = 0; count < pixelCount; ++count)
		{
			Z = 0;
			Zi = 0;
			k = 0;
			outOfBounds = true;
			
			while (k < iterations)
			{	
				++k; /* this needs to be first because sometimes the sum escapes on the first iteration */
				
				Zp = Z*Z - Zi*Zi + xPos;
				Zip = 2*Z*Zi + yPos;
			
				Z = Zp;
				Zi = Zip;
			
				if ((Z*Z + Zi*Zi) > 4)
				{
					iterationBuffer[count] = k;
					outOfBounds = false;
					k = iterations;
				}
				
			}
			
			if (outOfBounds)
				escapeBuffer[count] = false;
			else
				escapeBuffer[count] = true;
			
			xPos += xStep;
		}
		
		/* Normalize all iteration data to 255 for RGB conversion */
		normalize (iterationBuffer, escapeBuffer, pixelCount, iterations);
	
		/* Set rgb values to the buffer array */
		hueToRGB (colorBuffer, iterationBuffer, escapeBuffer, pixelCount, bufferLength);

		/* Call bitmap writer and write the data array to the file */
		writeBMP (colorBuffer, dataFile, bufferLength);
		
		yPos += yStep;
	}

	delete [] colorBuffer;
	delete [] iterationBuffer;
	delete [] escapeBuffer;
	dataFile.close();
	
	printf("\r%d%%\nDone!\n",100);

	return 0;
}
