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
//		Current Version:	1.2.0 (17/4/2014)
//
//		Info:
//
//			Version 1.2.X asks the user for the center coordinates for the
//			render as well as the zoom level (radius). They can choose to repeat
//			the rendering with different parameters at the end of the render
//			and the application will remind them what their previous inputs were.
//			It then stores the render in the file format
//			"MandelbrotSet_Real_Imaginary_Radius.bmp".
//
//		Version History:
//
//			1.2.0 -	Changed the framework to accomodate vectors rather than
//					data arrays. They are easier to resize and just safer
//					to use in general.
//
//					Also set the image dimensions as a fixed number (1200)
//					in preparation for the zoomable GUI that I will be building.
//
//			1.1.1 -	Changed the data types for the position and step variables
//					to double to accomodate larger zoom values. Now the smallest
//					radius is the smallest value a float can be printed as.
//
//			1.1.0 -	Changed the application to now accept user input on the
//					coordinates of the center position of the image. They can
//					also specify the zoom level. Because of this I decided to
//					change the file name to reflect the coordinates and zoom.
//
//					Added a feature that lets the user choose whether they want
//					to run the execution another time after the current render
//					is finished.
//
//					Also added a feature of reminding the user what their last
//					inputs were even across different application launches via
//					a .bin file.
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
//		-	There's a small improvement that can be made concerning
//			the user input. Right now it just counts any string of
//			characters longer than a certain length as an out of
//			range value. What if the user input a string of 20 zeros?
//			we need to make that work.
//
//	Notes/Todo (General)
//
//		-	Add exception handles for if the bitmap can't be opened
//			to catch the computed data in a dump file so on next
//			execution it just reads the data and puts into a bitmap.
//
//		-	Clean up the user input part of the execution.
//			One function perhaps?
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
//		- 	The main calculation loop can probably be optimized
//
//		- 	The string manipulation for getting the user input may not 
//			be efficient, but it deal with non-number inputs nicely.
//			If there's a better way to do it, please go ahead.
//
//		-	Multithreading the calculation loop would be awesome.
//
//		-	Actually .BMP files read BGR instead of RGB so we are flipping 
//			the color scheme when writing to the bitmap. This could be 
//			fixed, but since the RGB conversion spits out red for small
//			hues and blue for large values,	we'd have to flip the values
//			in the color buffer AND flip the order in which we write to
//			the file for our blue color scheme which isn't good for
//			computation times. So we are abusing the BGR interpretation 
//			to skip these steps. It's not the "right" way to do it, but
//			it saves a bit of run time.
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
//				Weikardzaena).
*/

/* There's a silly error about fopen not being safe or something when compiling on windows, so we need this to tell the compiler to ignore it. */
#ifdef	_MSC_VER
#define	_CRT_SECURE_NO_WARNINGS
#endif

#include <cstdio>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include "MandelbrotGenerator.h"

using namespace std;

/* Searches string for any non-digit characters, and returns false if there are non-digit characters */
bool isNumber (string str)
{
	return str.find_first_not_of("0123456789") == string::npos;
}

bool isFloat (string str)
{
	return str.find_first_not_of("0123456789.-") == string::npos;
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
	const string	currentVersion	=	"1.2.0";
	const string	cinFail			=	"The input stream failed to write to the string... That shouldn't ever happen but it did. Execution terminated. Press 'enter' to continue.";
	const string	floatWarning	=	"Oops, you can't include any non-number characters (except decimal points). Try again.";
	const string	decWarning		=	"Oops, you can't include any non-number characters (remember, no commas or anything). Try again.";
	const string	rangeWarning	=	"Sorry, that number was out of the range. Try again.";

	bool outOfBounds, repeat = true, proceed;
	string size, userInput, fileName;
	unsigned int i, j, k, pixelCount = 1200, bufferLength, count, percent, iterations, usignInput [] = {0};
	double Z, Zi, Zp, Zip, xStep, yStep, xCent, yCent, xPos, yPos, xStart, xEnd, yStart, yEnd, radius, floatInput [] = {0, 0, 0};

	BMP bitmapData;	/* Constructor is called from BMP.cpp */
	
	ofstream	dataFile;
	FILE		*prevInputFile;
	
	/* Check to make sure the data file exists */
	prevInputFile = fopen ("UserInput.bin", "r");
	
	if (prevInputFile == NULL)
	{
		prevInputFile = fopen("UserInput.bin", "w+b");
		fwrite(floatInput, sizeof(double), 3, prevInputFile);
		fwrite(usignInput, sizeof(unsigned int), 1, prevInputFile);
	}
	else
	{
		fclose (prevInputFile);
		prevInputFile = fopen ("UserInput.bin", "r+b");
	}
	
	rewind (prevInputFile);
	
	fread(&xCent, 1, sizeof(double), prevInputFile);
	fread(&yCent, 1, sizeof(double), prevInputFile);
	fread(&radius, 1, sizeof(double), prevInputFile);
	fread(&iterations, 1, sizeof(unsigned int), prevInputFile);

	rewind(prevInputFile);
			
	/* Set the image dimensions */
	bitmapData = setDimensions(pixelCount, pixelCount, bitmapData);

	/* Get the length of the buffer so the data is padded to integer multiples of 4 bytes (which is necessary for .BMP) */
	bufferLength = getBufferLength(pixelCount, bitmapData);

	/* Set File and Image Sizes in bitmap struct */
	bitmapData = fileSize(bufferLength, bitmapData);
	
	/* Initialize buffers with all zeroes.  This allows the padded bytes to be defined in the file. */
	vector<vector<bool>> escapeBuffer (pixelCount, vector<bool>(pixelCount, false));
	vector<vector<char>> colorBuffer (pixelCount, vector<char>(bufferLength, 0x0));
	vector<vector<unsigned int>> iterationBuffer (pixelCount, vector<unsigned int>(pixelCount, 0x0));

	printf("\n--------------------\n\n	Weikardzaena's Mandelbrot Set Generator\n\n	Version %s\n" 
		"(Email limitAtInfinity11@gmail.com for bug reports and suggestions)\n\n	DISCLAIMER:\n\n"
		"This software has not been thouroughly tested on different systems.\n"
		"On the computer that compiled the code, the application is stable,\n"
		"but by no means do we guarantee this to work on every machine.\n"
		"Moreover, with how resource intensive this application can be,\n"
		"running it may cause instability on your computer. USE AT YOUR OWN RISK."
		"\n\n--------------------\n	ABOUT: \n\n"
		"The Mandelbrot Set is a relationship in the complex plane that is\n"
		"generated by the recursive expression\n\nz(n+1) = z(n)^2+c\n\n"
		"For each complex number c, if the recursive sum diverges (goes to\n"
		"infinity) the number does not belong to the set whereas if a number\n"
		"causes the sum to \"orbit\" 0, it is a member. If you think of an\n"
		"image as a set of complex numbers (with the reals along the x axis\n"
		"and the imaginaries along the y axis) then each pixel either belongs\n"
		"to the set or doesn't. Coloring each pixel accordingly creates a\n"
		"visual representation of the set that turns out to be one of the\n"
		"most beautiful mathematical relationships. Moreover it turns out\n"
		"the set is infinitely divisible so you can zoom in to any length\n"
		"scale and observe intricate patterns.\n\n--------------------\n	INSTRUCTIONS:\n\n"
		"This application will generate a raw bitmap (.BMP) of the Mandelbrot\n"
		"Set centered at a point of your choice. You will also be able to\n"
		"pick the complex radius of the render which is another way of\n"
		"saying you get to pick the zoom. I.E. the smaller the radius, the\n"
		"larger the zoom. You will also be able to pick the number of iterations\n"
		"the program will go through to determine if a point is a part of\n"
		"the set. The more iterations, the finer detail you will be able to\n"
		"resolve.\n\n"
		"The file will be named in the following format:\n"
		"\"MandelbrotSet_Real_Imaginary_Radius.bmp\" and be stored in the\n"
		"same folder you launched this application from.\n\n	NOTE:\n\n"
		"Once you start picking large iteration values\n"
		"the computation time increases DRAMATICALLY\n"
		"(could be hours at some values)."
		"\n\nHave fun with it! Press Ctrl+C at any time to abort the program.\n\n--------------------\n", currentVersion.c_str());
	while (repeat) /* main loop */
	{
		repeat = false;
		proceed = false;
		
		printf ("\nLast values:\n\nReal = %f, Imaginary = %f, Radius = %f,\n"
			"Iterations = %u\n", xCent, yCent, radius, iterations);
		
		xCent = 3;
		yCent = 3;
		radius = 3;
		iterations = 0xffffffff;
		
		while (xCent >= 2 || xCent <= -2)
		{
			xCent = 3;
		
			printf("\nThis first part lets choose your center coordinate. Pick a number between\n"
				"-2 and 2 for both the Real and Imaginary coordinate (can be a floating\n"
				"point number).\n\nReal: ");
			cin >> userInput;
	
			if (cin.fail())
			{
				printf("\n%s\n", cinFail.c_str());
				cin.sync();
				cin.ignore();
				return -1;
			}
	
			if (!isFloat (userInput))
				printf ("\n%s\n", floatWarning.c_str());
			else
			{
				if (userInput.length() < 10)
				{
					if (stof (userInput) < 2 && stof (userInput) > -2)
						xCent = stof (userInput);
					else
						xCent = 3;
				}
				else
				{
					xCent = 3;
				}
		
				if (xCent >= 2 || xCent <= -2)
					printf("\n%s\n", rangeWarning.c_str());
			}
		}

		while (yCent >= 2 || yCent <= -2)
		{
			yCent = 3;
		
			printf("\nImaginary: ");
			cin >> userInput;
	
			if (cin.fail())
			{
				printf("\n%s", cinFail.c_str());
				cin.sync();
				cin.ignore();
				return -1;
			}
	
			if (!isFloat (userInput))
				printf ("\n%s\n", floatWarning.c_str());
			else
			{
				if (userInput.length() < 10)
				{
					if (stof(userInput) < 2 && stof(userInput) > -2)
					{
						yCent = stof(userInput);
						fwrite(&xCent, sizeof(double), 1, prevInputFile); /* Only write this to the file once the user has submitted the next one which make sure they want to keep the last value they entered */
					}
					else
						yCent = 3;
				}
				else
				{
					yCent = 3;
				}
		
				if (yCent >= 2 || yCent <= -2)
					printf("\n%s", rangeWarning.c_str());
			}
		}

		while (radius > 2)
		{
			radius = 3;
		
			printf("\nRadius (make it less than 2): ");
			cin >> userInput;
	
			if (cin.fail())
			{
				printf("%s\n", cinFail.c_str());
				cin.sync();
				cin.ignore();
				return -1;
			}
	
			if (!isFloat (userInput))
				printf ("\n%s\n", floatWarning.c_str());
			else
			{
				if (userInput.length() < 10)
				{
					if (stof(userInput) <= 2)
					{
						fwrite(&yCent, sizeof(double), 1, prevInputFile);
						radius = stof(userInput);
					}
					else
						radius = 3;
				}
				else
				{
					radius = 3;
				}
		
				if (radius > 2)
					printf("\n%s", rangeWarning.c_str());
			}
		}

		xStart	= 	xCent - radius;
		xEnd	= 	xCent + radius;
		yStart	=	yCent - radius;
		yEnd	=	yCent + radius;
		xPos	=	xStart;
		yPos	=	yStart;
		
		fileName = "MandelbrotSet_" + to_string(xCent) + "_" + to_string(yCent) + "_" + to_string(radius) + ".bmp";
		
		while (iterations > 4294967294)
		{
			iterations = 0xffffffff;
		
			printf("\nNumber of iterations (max is 4,294,967,294): ");
			cin >> userInput;
		
			if (cin.fail())
			{
				printf("\n%s\n", cinFail.c_str());
				cin.ignore();
				return -1;
			}
		
			if (!isNumber (userInput))
				printf("\n%s\n", decWarning.c_str());
			else
			{
				/* 	We need this if statement in the case that the user inputs something larger than the max value 
					for double. Otherwise stod will return nonsense values. */
				if (userInput.length() < 15) /* Max length of double is 15 digits in base-10 */
				{
					if (stod(userInput) < 0xffffffff)
					{
						fwrite(&radius, sizeof(double), 1, prevInputFile);
						iterations = (unsigned int)stod(userInput);
					}
					else
						iterations = 0xffffffff;
				}
				else
				{
					iterations = 0xffffffff;
				}

				if (iterations > 4294967294)
					printf("\n%s", rangeWarning.c_str());
			}
		}
	
		fwrite(&iterations, sizeof(unsigned int), 1, prevInputFile);

		printf("\nOpening data stream to '%s' (it will be in the same folder that you launched this application from)...\n", fileName.c_str());

		dataFile.open(fileName.c_str(), ofstream::out | ofstream::trunc | ofstream::binary);

		if (dataFile.fail())
			{
				printf("Could not open the file! Press 'enter' to exit.\n");
				cin.sync();
				cin.ignore();
				return -1;
			}

		printf("\nDone.\n");

		printf("Now executing calculations...\n\n");

		/* Write the file header */
		dataFile.seekp(ios_base::beg);
		dataFile.write(reinterpret_cast <char*>(&bitmapData), sizeof(bitmapData));

		/* Set miscellaneous values needed for calculation */
		xStep = (double)(xEnd - xStart)/pixelCount;
		yStep = (double)(yEnd - yStart)/pixelCount;

		/* Main calculation loop */
		for (j = 0; j < pixelCount; ++j)
		{
			percent = int(fabs(yPos-yStart)/(yEnd - yStart) * 100);
			printf("\r%d%%",percent);
		
			xPos = xStart;
		
			for (i = 0; i < pixelCount; ++i)
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
						iterationBuffer.at(j).at(i) = k;
						outOfBounds = false;
						k = iterations;
					}
				}
			
				if (outOfBounds)
					escapeBuffer.at(j).at(i) = false;
				else
					escapeBuffer.at(j).at(i) = true;
				xPos += xStep;
			}
		
			yPos += yStep;
		}

				
		/* Normalize all iteration data to 360 for HSV to RGB conversion */
		//normalize (iterationBuffer, escapeBuffer, pixelCount, iterations);

		/* Set rgb values to the buffer array */
		hsvToRGB (colorBuffer, iterationBuffer, escapeBuffer, pixelCount);

		/* Call bitmap writer and write the data array to the file */
		writeBMP (colorBuffer, dataFile, bufferLength, pixelCount);

		dataFile.close();
	
		printf("\r%d%%\nDone!\n\nGo again [y|n]? ",100);
		cin >> userInput;
		
		if (userInput == "n" || userInput == "N")
			repeat = false;
	
		else if (userInput == "y" || userInput == "Y")
		{
			repeat = true;
			rewind(prevInputFile);
		}
	
		else
			repeat = false;
	}
	
	fclose(prevInputFile);
	
	return 0;
}
