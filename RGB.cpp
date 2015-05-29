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
#include <vector>
#include <cstdio>

using namespace std;

/**
 * Normalize colors to 0-360
 */

void normalize(vector<vector<unsigned int>> &data, vector<vector<bool>> &escape, unsigned int length, unsigned int iterations)
{
	for (unsigned int j = 0; j < length; ++j) {
		for (unsigned int i = 0; i < length; ++i) {
			/* don't bother with the normalization if we're just going
			 * to skip that hue to rgb conversion later */
			if (escape.at(j).at(i))
				data.at(j).at(i) = int((float(data.at(j).at(i))) / iterations * 360);
		}
	}
}

/**
 * CONVERTS HSV TO RGB
 *
 * This algorithm takes in the number of iterations it took to escape and
 * interprets it as a hue in HSV space converting it to the corresponding RGB
 * representation (with max value).
 *
 * See <http://en.wikipedia.org/wiki/HSL_and_HSV#Converting_to_RGB> for an
 * explanation of the algorithm. */

void hsvToRGB(vector<vector<char>> &colorData, vector<vector<unsigned int>> &hue, vector<vector<bool>> &escape, unsigned int pixelCount)
{
	const float V = 1;
	float hueP, X, S, C;

	for (unsigned int j = 0; j < pixelCount; ++j) {
		for (unsigned int i = 0; i < pixelCount; ++i) {

			/**
			 * This escape flag is needed because sometimes the RGB
			 * conversion spits out 0, so we can't rely on purely
			 * that hue value to determine if the point is in the
			 * set or not
			 */

			if (escape.at(j).at(i)) {
				// Set parameters needed to calculate color
				hueP = (float)(hue.at(j).at(i) % 360) / 60;
				S = 1;
				X = S * (1 - fabs(fmod(hueP, 2) - 1));
				C = V - S;
				
				if (hueP >= 0 && hueP < 1) {
					colorData.at(j).at(i * 3) = (S + C) * 255;
					colorData.at(j).at(i * 3 + 1) = (X + C) * 255;
					colorData.at(j).at(i * 3 + 2) = C * 255;
				} else if (hueP >= 1 && hueP < 2) {
					colorData.at(j).at(i * 3) = (X + C) * 255;
					colorData.at(j).at(i * 3 + 1) = (S + C) * 255;
					colorData.at(j).at(i * 3 + 2) = C * 255;
				} else if (hueP >= 2 && hueP < 3) {
					colorData.at(j).at(i * 3) = C * 255;
					colorData.at(j).at(i * 3 + 1) = (S + C) * 255;
					colorData.at(j).at(i * 3 + 2) = (X + C) * 255;
				} else if (hueP >= 3 && hueP < 4) {
					colorData.at(j).at(i * 3) = C * 255;
					colorData.at(j).at(i * 3 + 1) = (X + C) * 255;
					colorData.at(j).at(i * 3 + 2) = (S + C) * 255;
				} else if (hueP >= 4 && hueP < 5) {
					colorData.at(j).at(i * 3) = (X + C) * 255;
					colorData.at(j).at(i * 3 + 1) = C * 255;
					colorData.at(j).at(i * 3 + 2) = (S + C) * 255;
				} else if (hueP >= 5 && hueP < 6) {
					colorData.at(j).at(i * 3) = (S + C) * 255;
					colorData.at(j).at(i * 3 + 1) = C * 255;
					colorData.at(j).at(i * 3 + 2) = (X + C) * 255;
				}
			} else {
				colorData.at(j).at(i * 3) = 0;
				colorData.at(j).at(i * 3 + 1) = 0;
				colorData.at(j).at(i * 3 + 2) = 0;
			}
		}
	}
}

