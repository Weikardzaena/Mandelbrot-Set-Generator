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

/**
 * TODO (for current version):
 *
 *	There's a small improvement that can be made concerning
 *	the user input. Right now it just counts any string of
 *	characters longer than a certain length as an out of
 *	range value. What if the user input a string of 20 zeros?
 *	 we need to make that work.
 *
 * TODO (General):
 *
 *	Add exception handles for if the bitmap can't be opened
 *	to catch the computed data in a dump file so on next
 *	execution it just reads the data and puts into a bitmap.
 *
 *	Clean up the user input part of the execution.
 *	One function perhaps?
 *
 *	In BMP.cpp, the file type is defined to be 0x4D42
 *	(the "BM" at the beginning of almost every BMP), but "BM"
 *	SHOULD BE 0x424D. In fact, it is written to the file
 *	as 0x424D instead of 0x4D42, which is -- needless to
 *	say -- strange. It is unclear why this is happening
 *	because the rest of the header is written appropriately...
 *	It needs to be looked into, but right now the execution works
 *	on Ubuntu 12.04 64 bit and Windows 7 Home Premium 64 Bit.
 *
 *	The main calculation loop can probably be optimized
 *
 *	Multithreading the calculation loop would be awesome.
 */

/**
 * There's a silly error about fopen not being safe or something when compiling
 * on windows, so we need this to tell the compiler to ignore it. 
 */
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

/**
 * Grabs a line from the language file
 */
void getStringFromFile()
{
	ifstream file;
	file.open("en-US.ini");
	if (!file.good())
		return;
	char c;
	vector<char> buffer;
	file.seekg(ios::beg);
	while (file.get(c))
		buffer.push_back(c);
	for (int i = 0; i < buffer.size(); ++i)
		cout << buffer[i];
}

/**
 * Searches string for any non-digit characters, and returns false if there are
 * non-digit characters 
 */
bool isNumber (string str)
{
	return str.find_first_not_of("0123456789") == string::npos;
}

/**
 * Searches string for any non-float characters, and returns false if there are
 * non-float characters 
 */
bool isFloat (string str)
{
	return str.find_first_not_of("0123456789.-") == string::npos;
}

/**
 * Application Entry Point
 */
int main(int argc, char *argv[])
{
	/* Forward Definitions of Variables */
	const string currentVersion = "1.2.0";
	const string cinFail        = "The input stream failed to write to \
                                       the string. Execution terminated. \
                                       Press 'enter' to continue.";
	const string floatWarning   = "Oops, you can't include any non-number \
                                       characters (except decimal points).\
				       Try again.";
	const string decWarning     = "Oops, you can't include any non-number \
                                       characters (remember, no commas or \
                                       anything). Try again.";
	const string rangeWarning   = "Sorry, that number was out of the range.\
                                       Try again.";

	bool outOfBounds; /*!< Tells if the sum escaped or not */
	bool repeat = true;
	bool proceed;

	/* string size; do we need this?? */
	string userInput;
	string fileName;

	unsigned int i; /*!< Iteration integer declaration */
	unsigned int j; /*!< Iteration integer declaration */
	unsigned int k; /*!< Iteration integer declaration */
	unsigned int pixelCount = 1200; /*!< Number of pixels in both the x and y directions */
	unsigned int bufferLength; /*!< How long the array of calculated data must be */
	/* unsigned int count; do we need this?? */
	unsigned int percent; /*!< The value that is displayed telling the user the progress of the render */
	unsigned int iterations; /*!< Number of iterations before escape for each pixel */
	unsigned int usignInput [] = {0}; /*!< If the previous user input file doesn't exist, use this to create a new one. */

	double Z; /*!< Real part of the complex argument */
	double Zi; /*!< Imaginary part of the complex argument */
	double Zp; /*!< Temporary variable for the real part of Z*Z */
	double Zip; /*!< Temporary variable for the imaginary part of Z*Z */
	double xStep; /*!< Width of each horizontal pixel in the complex plane */
	double yStep; /*!< Height of each vertical pixel in the complex plane */
	double xCent; /*!< X center coordinate for the image in the complex plane */
	double yCent; /*!< Y center coordinate for the image in the complex plane */
	double xPos; /*!< Horizontal position of current pixel being calculated */
	double yPos; /*!< Vertical position of current pixel being calculated */
	double xStart; /*!< Starting horizontal complex coordinate */
	double xEnd; /*!< Ending horizontal complex coordinate */
	double yStart; /*!< Starting vertical complex coordinate */
	double yEnd; /*!< Ending vertical complex coordinate */
	double radius; /*!< distance from center point to edge of image (aka zoom level) */
	double floatInput [] = {0, 0, 0}; /*!< Array that will contain previous user input */

	BMP bitmapData;	// Constructor is called from BMP.cpp

	getStringFromFile();

	ofstream	dataFile; /*!< The output stream to the bitmap file */
	FILE		*prevInputFile; /*!< Input stream to the data file containing the user's previous input */
	
	/**
	 * Checks to make sure the data file exists
	 */
	prevInputFile = fopen ("UserInput.bin", "r");
	
	if (prevInputFile == NULL) {
		prevInputFile = fopen("UserInput.bin", "w+b");
		fwrite(floatInput, sizeof(double), 3, prevInputFile);
		fwrite(usignInput, sizeof(unsigned int), 1, prevInputFile);
	} else {
		fclose (prevInputFile);
		prevInputFile = fopen ("UserInput.bin", "r+b");
	}
	
	rewind (prevInputFile);
	
	fread(&xCent, 1, sizeof(double), prevInputFile);
	fread(&yCent, 1, sizeof(double), prevInputFile);
	fread(&radius, 1, sizeof(double), prevInputFile);
	fread(&iterations, 1, sizeof(unsigned int), prevInputFile);

	rewind(prevInputFile);
			
	/**
	 * Sets the image dimensions
	 */
	bitmapData = setDimensions(pixelCount, pixelCount, bitmapData);

	/**
	 * Gets the length of the buffer so the data is padded to integer
	 * multiples of 4 bytes (which is necessary for .BMP)
	 */
	bufferLength = getBufferLength(pixelCount, bitmapData);

	/** 
	 * Set File and Image Sizes in bitmap struct
	 */
	bitmapData = fileSize(bufferLength, bitmapData);
	
	/**
	 * Initializes buffers with all zeroes.  This ensures the padded bytes are
	 * defined in the file
	 */
	vector<vector<bool>> escapeBuffer (pixelCount, vector<bool>(pixelCount, false));
	vector<vector<char>> colorBuffer (pixelCount, vector<char>(bufferLength, 0x0));
	vector<vector<unsigned int>> iterationBuffer (pixelCount,
			vector<unsigned int>(pixelCount, 0x0));

	printf("        Weikardzaena's Mandelbrot Set Generator\n\n"

"        Copyright (C) 2015 G. Nikolai Kotula\n\n"

"        Version %s\n\n"

"This program comes with ABSOLUTELY NO WARRENTY. This is a very resource-\n"
"intensive application and by no means do we guarantee this program to operate\n"
"safely on any machine.\n\n"

"        LICENSE\n\n"

"This program is distributed under the GNU General Public License (GPL) v3.\n"
"You should have recieved a copy of the GNU Public License along with this\n"
"program.  If not, see <http://www.gnu.org/licenses/>\n\n"

"By using this application you agree to the terms and conditions set forth\n"
"by this license.\n\n"

"Do you wish to continue? ('y' | 'n' | 'l' to view license):\n\n"

"Go to https://github.com/Weikardzaena/Mandelbrot-Set-Generator/issues for bug\n"
"reports and suggestions.\n\n"

"        ABOUT\n\n"

"The Mandelbrot Set is a mathematical relationship in the complex plane that is\n"
"defined by the recursive expression\n\n"

"    Z_(n+1) = Z(n)^2 + C\n\n"

"For each complex number C, if this recursive relationship diverges (goes to\n"
"infinity), that number does not belong in the set.  If the number C causes the\n"
"sum to \"orbit\" 0, it is a member.  If you can imagine a picture as a set of\n"
"complex coordinates with the reals along the x-axis and the imaginaries along\n"
"the y-axis, then each pixel either belongs to the set or doesn't.  Coloring\n"
"each pixel according to it's membership creates a visual representation of the\n"
"set that turns out to be one of the most beautiful mathematical relationships.\n"
"Moreover, since the Mandelbrot Set is a fractal, the set is infinitely divisible\n"
"meaning you can zoom in as far as you want and still see very intricate patterns\n"

"Press 'enter' to continue...\n\n"

"       INSTRUCTIONS\n\n"

"This application will generate a raw bitmap (.BMP) of the Mandelbrot Set centered\n"
"at a point of your choice.  You will also be able to pick the complex radius of\n"
"the rendered image:  I.E. a smaller radius = larger zoom.  You can also pick the\n"
"number of iterations the application will go through when rendering each image.\n"
"The higher the iterations, the finer the detail you can resolve.\n\n"

"HOWEVER increasing the iterations increases the render time exponentially. It\n"
"could be HOURS at some values...  Basically anything below about 500 will be\n"
"fine for all but the smallest details, but for enthusiasts the number can be\n"
"up to 1 million iterations per pixel.\n\n"

"Have fun with it! Press ctrl-c at any time to quit.\n\n", currentVersion.c_str());

	/**
	 * The main loop for user interaction
	 */
	while (repeat) {
		repeat = false;
		proceed = false;
		
		printf ("\nLast values:\n\nReal = %f, Imaginary = %f, Radius = %f,\n"
			"Iterations = %u\n", xCent, yCent, radius, iterations);
		
		/**
		 * These initial values are to ensure that the while loops controlling
		 * user input are actually jumped into initially
		 */
		xCent = 3;
		yCent = 3;
		radius = 3;
		iterations = 0xffffffff;
		
		while (xCent >= 2 || xCent <= -2) {
			xCent = 3;
		
			printf("\nThis first part lets choose your center coordinate. Pick a number between\n"
				"-2 and 2 for both the Real and Imaginary coordinate (can be a floating\n"
				"point number).\n\nReal: ");
			cin >> userInput;
	
			if (cin.fail()) {
				printf("\n%s\n", cinFail.c_str());
				cin.sync();
				cin.ignore();
				return -1;
			}
	
			if (!isFloat (userInput)) {
				printf ("\n%s\n", floatWarning.c_str());
			} else {
				if (userInput.length() < 10) {
					if (stof (userInput) < 2 && stof (userInput) > -2)
						xCent = stof (userInput);
					else
						xCent = 3;
				} else {
					xCent = 3;
				}
		
				if (xCent >= 2 || xCent <= -2)
					printf("\n%s\n", rangeWarning.c_str());
			}
		}

		while (yCent >= 2 || yCent <= -2) {
			yCent = 3;
		
			printf("\nImaginary: ");
			cin >> userInput;
	
			if (cin.fail()) {
				printf("\n%s", cinFail.c_str());
				cin.sync();
				cin.ignore();
				return -1;
			}
	
			if (!isFloat (userInput)) {
				printf ("\n%s\n", floatWarning.c_str());
			} else {
				if (userInput.length() < 10) {
					if (stof(userInput) < 2 && stof(userInput) > -2) {
						yCent = stof(userInput);
/* Only write this to the file once the user has submitted the next one which
   make sure they want to keep the last value they entered */
						fwrite(&xCent, sizeof(double), 1,
								prevInputFile);
					} else {
						yCent = 3;
					}
				} else {
					yCent = 3;
				}
		
				if (yCent >= 2 || yCent <= -2)
					printf("\n%s", rangeWarning.c_str());
			}
		}

		while (radius > 2) {
			radius = 3;
		
			printf("\nRadius (make it less than 2): ");
			cin >> userInput;
	
			if (cin.fail()) {
				printf("%s\n", cinFail.c_str());
				cin.sync();
				cin.ignore();
				return -1;
			}
	
			if (!isFloat (userInput)) {
				printf ("\n%s\n", floatWarning.c_str());
			} else {
				if (userInput.length() < 10) {
					if (stof(userInput) <= 2) {
						fwrite(&yCent, sizeof(double), 1, prevInputFile);
						radius = stof(userInput);
					} else {
						radius = 3;
					}
				} else {
					radius = 3;
				}
		
				if (radius > 2)
					printf("\n%s", rangeWarning.c_str());
			}
		}

		xStart	= xCent - radius;
		xEnd	= xCent + radius;
		yStart	= yCent - radius;
		yEnd	= yCent + radius;
		xPos	= xStart;
		yPos	= yStart;
		
		fileName = "MandelbrotSet_" + to_string(xCent) + "_" +
				to_string(yCent) + "_" + to_string(radius) + ".bmp";
		
		while (iterations > 4294967294) {
			iterations = 0xffffffff;
		
			printf("\nNumber of iterations (max is 4,294,967,294): ");
			cin >> userInput;
		
			if (cin.fail()) {
				printf("\n%s\n", cinFail.c_str());
				cin.ignore();
				return -1;
			}
		
			if (!isNumber (userInput)) {
				printf("\n%s\n", decWarning.c_str());
			} else {
/* We need this if statement in the case that the user inputs something larger
 * than the max value for double. Otherwise stod will return nonsense values.
 * NOTE: the max length of double is 15 digits in base-10 */
				if (userInput.length() < 15) {
					if (stod(userInput) < 0xffffffff) {
						fwrite(&radius, sizeof(double), 1, prevInputFile);
						iterations = (unsigned int)stod(userInput);
					} else {
						iterations = 0xffffffff;
					}
				} else {
					iterations = 0xffffffff;
				}

				if (iterations > 4294967294)
					printf("\n%s", rangeWarning.c_str());
			}
		}
	
		fwrite(&iterations, sizeof(unsigned int), 1, prevInputFile);

		printf("\nOpening data stream to '%s' (it will be in the same folder that you launched this application from)...\n", fileName.c_str());

		dataFile.open(fileName.c_str(), ofstream::out | ofstream::trunc |
				ofstream::binary);

		if (dataFile.fail()) {
			printf("Could not open the file! Press 'enter' to exit\n");
			cin.sync();
			cin.ignore();
			return -1;
		}

		printf("\nDone.\n");

		printf("Now executing calculations...\n\n");

		/**
		 * Write the bitmap header
		 */
		dataFile.seekp(ios_base::beg);
		dataFile.write(reinterpret_cast <char*>(&bitmapData), sizeof(bitmapData));

		/* Set miscellaneous values needed for calculation */
		xStep = (double)(xEnd - xStart)/pixelCount;
		yStep = (double)(yEnd - yStart)/pixelCount;

		/**
		 * Main calculation loop
		 */
		for (j = 0; j < pixelCount; ++j) {
			percent = int(fabs(yPos-yStart)/(yEnd - yStart) * 100);
			printf("\r%d%%",percent);
		
			xPos = xStart;
		
			for (i = 0; i < pixelCount; ++i) {
				Z = 0;
				Zi = 0;
				k = 0;
				outOfBounds = true;
			
				while (k < iterations) {
					/**
					 * The ++k needs to be first because sometimes 
					 * the sum escapes on the first iteration,
					 * and we don't want k to be zero if it
					 * escapes because that's how we tell if
					 * the pixel is a member of the set.
					 */
					++k;
				
					Zp = Z*Z - Zi*Zi + xPos;
					Zip = 2*Z*Zi + yPos;

					Z = Zp;
					Zi = Zip;
			
					if ((Z*Z + Zi*Zi) > 4) {
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

		/**
		 * Set rgb values in the buffer array
		 */
		hsvToRGB (colorBuffer, iterationBuffer, escapeBuffer, pixelCount);

		/**
		 * Write the data array to the bitmap file
		 */
		writeBMP (colorBuffer, dataFile, bufferLength, pixelCount);

		dataFile.close();
	
		printf("\r%d%%\nDone!\n\nGo again [y|n]? ",100);
		cin >> userInput;
		
		if (userInput == "n" || userInput == "N") {
			repeat = false;
		} else if (userInput == "y" || userInput == "Y") {
			repeat = true;
			rewind(prevInputFile);
		} else {
			repeat = false;
		}
	}
	
	fclose(prevInputFile);
	
	return 0;
}
