/*
//	Weikarczaena's Mandelbrot Set Generator RGB.cpp file.
//
//		All the functions for dealing with RGB calculations
//		and conversions are contained in this file.
*/

#include <cmath>

using namespace std;

/* FUNCTION FOR NORMALIZING COLOR VALUES TO 0-255 */

void normalize(unsigned int *data, bool *escape, unsigned int length, unsigned int iterations)
{
	for (unsigned int i = 0; i < length; ++i)
	{
		/*	don't bother with the normalization if we're just going
			to skip that hue to rgb conversion later */
		if (escape[i])
			data[i] = int((float(data[i])) / iterations * 255);
		else
			data[i] = 0;
	}
}

/*	CONVERT HUE TO RGB
//
//	This algorithm takes in a value from 0 to 255 and interprets it as a hue in HSV space
//	converting it to the corresponding RGB representation (with max saturation and value) 
*/
void hueToRGB(char *colorData, unsigned int *hue, bool *escape, unsigned int pixelCount, unsigned int bufferLength)
{
	float hueP, X;
	
	for (unsigned int i = 0; i < pixelCount; ++i)
	{
		/*
			This escape flag is needed because sometimes the RGB
			conversion spits out 0, so we can't rely on purely
			that hue value to determine if the point is in the
			set or not
		*/
		if (escape[i])
		{
			/* Set parameters needed to calculate color */
			hueP = (float)hue[i] / 60;
			X = 1 - fabs(fmod(hueP, 2) - 1);
	
			if (hueP >= 0 && hueP < 1)
			{
				colorData[i * 3] = 255;
				colorData[i * 3 + 1] = X * 255;
				colorData[i * 3 + 2] = 0;
			}
				else if (hueP >= 1 && hueP < 2)
			{
				colorData[i * 3] = X * 255;
				colorData[i * 3 + 1] = 255;
				colorData[i * 3 + 2] = 0;
			}
				else if (hueP >= 2 && hueP < 3)
			{
				colorData[i * 3] = 0;
				colorData[i * 3 + 1] = 255;
				colorData[i * 3 + 2] = X * 255;
			}
				else if (hueP >= 3 && hueP < 4)
			{
				colorData[i * 3] = 0;
				colorData[i * 3 + 1] = X * 255;
				colorData[i * 3 + 2] = 255;
			}
				else if (hueP >= 4 && hueP < 5)
			{
				colorData[i * 3] = X * 255;
				colorData[i * 3 + 1] = 0;
				colorData[i * 3 + 2] = 255;
			}
				else if (hueP >= 5 && hueP < 6)
			{
				colorData[i * 3] = 255;
				colorData[i * 3 + 1] = 0;
				colorData[i * 3 + 2] = X * 255;
			}
		}
		
		else
		{
			colorData[i * 3] = 0;
			colorData[i * 3 + 1] = 0;
			colorData[i * 3 + 2] = 0;
		}
	}
}

