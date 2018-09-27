/*
	Copyright (c) 2018 Steve "Sc00bz" Thomas (steve at tobtu dot com)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include <stdio.h>
#include <algorithm>
#include "genpw.h"
#include "csprng.h"

/**
 * Returns the bit strength of a password generated by genPassword() and genPasswordLength().
 *
 * @param baseLength - Length of base password.
 * @return The bit strength on success, otherwise zero.
 */
double genPassword_getBitStrength(uint32_t baseLength)
{
	// "Only" accurate to 15 sig figs
	const double BIT_STRENGTHS[] = {
		 39.1200670699161,  43.9960948472761,  48.7440594403916,  53.4676318261445,  58.1764480484353,  62.7665238278741,
		 67.2404445074635,  71.7042336853753,  76.1312709240191,  80.4365604507374,  84.6200519662894,  88.8587015762423,
		 92.9647168662745,  96.9323853082289, 100.856654071890,  104.721219076432,  108.426520544345,  111.970933247131,
		115.534965175914,  118.907050614862,  122.065883975916,  125.129460745934,  128.052654673660,  130.687036809517,
		132.984838512222,  135.221877995274,  136.987412956791,  138.094328330543,  138.094328478112};

	if (baseLength < 8 || baseLength > 36)
	{
		return 0;
	}
	return BIT_STRENGTHS[baseLength - 8];
}

/**
 * Returns the base password length given a desired bit strength of at most 128 bits.
 *
 * There are 8 targets: 48, 56, 64, 72, 80, 96, 100, and 128. Actual strengths are slightly higher: 48.7441, 58.1764, 67.2404,
 * 76.1313, 80.4366, 96.9324, 100.8567, and 128.0527. `bitStrength` can be set to any value 128 or less and it will select the
 * correct target. It is suggested you stick with the original 8 target bit strengths and display them as such (ie 56 or 56+ vs
 * 58 or 58.1764).
 *
 * @param bitStrength - Bit strength of the password.
 * @return Base password length on success, otherwise zero.
 */
uint32_t genPassword_getBaseLength(uint32_t bitStrength)
{
#ifdef GEN_PASSWORD_FULL_RANGE_BIT_STRENGTH
	// No one will see this... meh
	#pragma message("*****************************************************************************************")
	#pragma message("*****************************************************************************************")
	#pragma message("**  WARNING defining GEN_PASSWORD_FULL_RANGE_BIT_STRENGTH is strongly *NOT* suggested  **")
	#pragma message("*****************************************************************************************")
	#pragma message("*****************************************************************************************")
	const uint32_t BIT_STRENGTHS[] = {39, 43, 48, 53, 58, 62, 67, 71, 76, 80, 84, 88, 92, 96, 100, 104, 108, 111, 115, 118, 122, 125, 128, 130, 132, 135, 136, 138};
#else
	const uint32_t BIT_STRENGTHS[] = {48, 58, 67, 76, 80, 96, 100, 128};
	const uint32_t LENGTHS[]       = {10, 12, 14, 16, 17, 21,  22,  30};
#endif
	const size_t   COUNT           = sizeof(BIT_STRENGTHS) / sizeof(uint32_t);

	if (bitStrength > BIT_STRENGTHS[COUNT - 1])
	{
		return 0;
	}

	// Note the "- 1" in "BIT_STRENGTHS + COUNT - 1"
	uint32_t length = (uint32_t) (std::lower_bound(BIT_STRENGTHS, BIT_STRENGTHS + COUNT - 1, bitStrength) - BIT_STRENGTHS);

#ifdef GEN_PASSWORD_FULL_RANGE_BIT_STRENGTH
	length += 8;
#else
	length = LENGTHS[length];
#endif

	return length;
}

/**
 * Generates a random password using a CSPRNG given a desired bit strength of at most 128 bits.
 *
 * The passwords that are generated are lowercase numeric with floor(length*10.125/36) digits, (length-digits-1) letters, and
 * one more letter or digit. Digits are noncontiguous and no repeated characters. There are 8 targets: 48, 56, 64, 72, 80, 96,
 * 100, and 128. Actual strengths are slightly higher: 48.7441, 58.1764, 67.2404, 76.1313, 80.4366, 96.9324, 100.8567, and
 * 128.0527. `bitStrength` can be set to any value 128 or less and it will select the correct target. It is suggested you stick
 * with the original 8 target bit strengths and display them as such (ie 56 or 56+ vs 58 or 58.1764).
 *
 * @param password    - Password buffer of at least 38 characters.
 * @param bitStrength - Bit strength of the password.
 * @param flags       - Flags for uppercase and/or symbol.
 * @return Zero on success, otherwise non-zero.
 */
int genPassword(char password[38], uint32_t bitStrength, int flags)
{
	return genPasswordLength(password, genPassword_getBaseLength(bitStrength), flags);
}

/**
 * Generates a random password using a CSPRNG given a desired *base password* length of at most 36 characters.
 *
 * The passwords that are generated are lowercase numeric with floor(length*10.125/36) digits, (length-digits-1) letters, and
 * one more letter or digit. Digits are noncontiguous and no repeated characters.
 *
 * `baseLength`: bit strength
 *  8:  39.1201,  9:  43.9961, 10:  48.7441, 11:  53.4676, 12:  58.1764, 13:  62.7665, 14:  67.2404, 15:  71.7042,
 * 16:  76.1313, 17:  80.4366, 18:  84.6201, 19:  88.8587, 20:  92.9647, 21:  96.9324, 22: 100.8567, 23: 104.7212,
 * 24: 108.4265, 25: 111.9709, 26: 115.5350, 27: 118.9071, 28: 122.0659, 29: 125.1295, 30: 128.0527, 31: 130.6870,
 * 32: 132.9848, 33: 135.2219, 34: 136.9874, 35: 138.0943, 36: 138.0943
 *
 * @param password   - Password buffer of at least (`baseLength`+2) characters (or use char[38] as that's the max).
 * @param baseLength - Length of base password (8 to 36).
 * @param flags      - Flags for uppercase and/or symbol.
 * @return Zero on success, otherwise non-zero.
 */
int genPasswordLength(char password[38], uint32_t baseLength, int flags)
{
	// n >= 8 and n <= 36
	// n characters: d=floor(length*10.125/36) digits, l=(length-digits-1) lowercase, and 1 lowercase or digit
	//     for n=36: d=10 digits, l=26 lowercase
	// Noncontiguous digits
	// No repeated characters
	//
	// Key Space
	//   (26 P l+1) * (10 P d  ) * ((n C d  ) - (l+2)) +
	//   (26 P l  ) * (10 P d+1) * ((n C d+1) - (l+1))
	//
	//   for n=36: (26 P l) * (10 P d) * ((n C d) - (l+1))

	if (baseLength < 8 || baseLength > 36)
	{
		return 1;
	}

	// Determine numDigits and numLetters
	// numDigits = (10.125 * baseLength) / 36
	// Doing 10.125 vs 10 adds an extra digit to base lengths 25 and 32.
	// This increases key space of those by 0.7617% and 0.8001% respectively.
	// Note if you do change this to "(10 * baseLength) / 36" also change EXTRA_LETTER[], EXTRA_DIGIT[], and EXTRA_CHAR_MAX[].
	uint32_t numDigits  = (10 * baseLength + baseLength / 8) / 36;
	uint32_t numLetters = baseLength - numDigits - 1;
	if (baseLength == 36)
	{
		numLetters++;
	}
	else
	{
		// a = (26 P l+1) * (10 P d  ) * ((n C d  ) - (l+2))
		// b = (26 P l  ) * (10 P d+1) * ((n C d+1) - (l+1))
		// keySpace = a + b
		// aProbability = a / keySpace
		// bProbability = b / keySpace
		// x = a / GDC(a, b)
		// y = b / GDC(a, b)
		// aProbability == x / (x + y)
		// bProbability == y / (x + y)

		// Note that "x/(x+y) == EXTRA_LETTER[]/(EXTRA_CHAR_MAX[]+1)", but "x == EXTRA_LETTER[]" is not always true.
#ifdef GEN_PASSWORD_CALCULATE_PROBABILITY
		// 26!/(26-l-1)! * 10!/(10-d  )! * (n!/(n-d  )!/(d  )! - (l+2))
		// 26!/(26-l  )! * 10!/(10-d-1)! * (n!/(n-d-1)!/(d+1)! - (l+1))
		//
		// 26!/(26-l-1)!        * 10!/(10-d-1)!/(10-d) * (n!/(n-d  )!/(d  )! - (l+2))
		// 26!/(26-l-1)!/(26-l) * 10!/(10-d-1)!        * (n!/(n-d-1)!/(d+1)! - (l+1))
		//
		// Enough common factors are removed that it fits in a 32 bit int
		// and "n!/(n-d)!" fits in a 64 bit int
		// x' = (26-l) * (n!/(n-d  )!/(d  )! - (l+2))
		// y' = (10-d) * (n!/(n-d-1)!/(d+1)! - (l+1))
		// aProbability == x' / (x' + y')
		// bProbability == y' / (x' + y')

		// tmp = n!/(n-d)!
		uint64_t tmp = baseLength - numDigits + 1;
		for (uint32_t i = baseLength - numDigits + 2; i <= baseLength; i++)
		{
			uint64_t tmp2 = tmp;
			tmp *= i;
		}

		// tmp2 = d!
		uint32_t tmp2 = 2;
		for (uint32_t i = 3; i <= numDigits; i++)
		{
			uint64_t tmp3 = tmp2;
			tmp2 *= i;
		}

		// tmp2 = n!/(n-d)!/d!
		tmp2 = (uint32_t) (tmp / tmp2);

		// x = (26-l) * (n!/(n-d  )!/(d  )! - (l+2))
		// y = (10-d) * (n!/(n-d-1)!/(d+1)! - (l+1))
		uint32_t x = (26 - numLetters) * (tmp2 - numLetters - 2);
		uint32_t y = (10 - numDigits)  * ((tmp2 * (baseLength - numDigits)) / (numDigits + 1) - numLetters - 1);

		if (randomUint32(x + y - 1) < x)
#else
		// Old values for "numDigits = (10 * baseLength) / 36"
		// const uint32_t EXTRA_LETTER[]   = {441, 10, 171, 1482, 10,  935, 256, 41,  9035, 1274,  9212, 1659,  61952,  55913, 205139,  5937, 44859, 18640,  5770, 2072021,  592009, 4292123, 1330205, 342987, 10518275, 38567075, 20980492,  5431341};
		// // const uint32_t EXTRA_DIGIT[] = {400, 11, 224, 1127,  9,  987, 315, 34,  8712, 1425,  7135, 1490,  64575,  67810, 170528,  5768, 51272, 25299,  5139, 2220055,  777021, 3338328, 1300648, 438262, 18699184, 30853672, 26225623, 14121490};
		// const uint32_t EXTRA_CHAR_MAX[] = {840, 20, 394, 2608, 18, 1921, 570, 74, 17746, 2698, 16346, 3148, 126526, 123722, 375666, 11704, 96130, 43938, 10908, 4292075, 1369029, 7630450, 2630852, 781248, 29217458, 69420746, 47206114, 19552830};

		// New values for "numDigits = (10 * baseLength + baseLength / 8) / 36"
		const uint32_t EXTRA_LETTER[]   = {441, 10, 171, 1482, 10,  935, 256, 41,  9035, 1274,  9212, 1659,  61952,  55913, 205139,  5937, 44859, 160227,  5770, 2072021,  592009, 4292123, 1330205, 342987,  696864, 38567075, 20980492,  5431341};
		// const uint32_t EXTRA_DIGIT[] = {400, 11, 224, 1127,  9,  987, 315, 34,  8712, 1425,  7135, 1490,  64575,  67810, 170528,  5768, 51272, 120173,  5139, 2220055,  777021, 3338328, 1300648, 438262,  400697, 30853672, 26225623, 14121490};
		const uint32_t EXTRA_CHAR_MAX[] = {840, 20, 394, 2608, 18, 1921, 570, 74, 17746, 2698, 16346, 3148, 126526, 123722, 375666, 11704, 96130, 280399, 10908, 4292075, 1369029, 7630450, 2630852, 781248, 1097560, 69420746, 47206114, 19552830};

		if (randomUint32(EXTRA_CHAR_MAX[baseLength - 8]) < EXTRA_LETTER[baseLength - 8])
#endif
		{
			numLetters++;
		}
		else
		{
			numDigits++;
		}
	}

	// Generate password characters
	char letters[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
	char digits[]  = {'0','1','2','3','4','5','6','7','8','9'};
	uint32_t maxLetter = 25;
	uint32_t maxDigit  = 9;
	for (uint32_t i = 0, j; i < numLetters; i++)
	{
		j = randomUint8((uint8_t) maxLetter);
		password[i] = letters[j];
		letters[j] = letters[maxLetter];
		maxLetter--;
	}
	secureClearMemory(letters, sizeof(letters));
	for (uint32_t i = 0, j; i < numDigits; i++)
	{
		j = randomUint8((uint8_t) maxDigit);
		password[i + numLetters] = digits[j];
		digits[j] = digits[maxDigit];
		maxDigit--;
	}
	secureClearMemory(digits, sizeof(digits));
	password[baseLength] = 0;

	// Randomize order of characters
	uint32_t digitsInARow;
	do
	{
		// Randomize order
		for (uint32_t i = 0, j; i < baseLength; i++)
		{
			j = randomUint8((uint8_t) (baseLength - 1));
			if (i != j)
			{
				// Swap pw[i] and pw[j]
				password[i] ^= password[j];
				password[j] ^= password[i];
				password[i] ^= password[j];
			}
		}

		// Check for contiguous digits
		digitsInARow = 0;
		if (password[0] <= '9')
		{
			digitsInARow++;
		}
		for (uint32_t i = 1; i < baseLength; i++)
		{
			if (password[i] <= '9')
			{
				if (password[i-1] > '9')
				{
					digitsInARow = 0;
				}
				digitsInARow++;
			}
		}
	} while (digitsInARow == numDigits);

	// You don't need these unless required by password policy
	if ((flags & GEN_PASSWORD_FLAG_NEED_UPPERCASE) != 0)
	{
		for (uint32_t i = 0; i < baseLength; i++)
		{
			if (password[i] >= 'a')
			{
				password[i] -= 32;
				break;
			}
		}
	}
	if ((flags & GEN_PASSWORD_FLAG_NEED_SYMBOL) != 0)
	{
		password[baseLength] = '!';
		password[baseLength + 1] = 0;
	}

	return 0;
}
