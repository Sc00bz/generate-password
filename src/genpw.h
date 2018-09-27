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

#pragma once

#include <stdint.h>

const int GEN_PASSWORD_FLAG_NEED_UPPERCASE = 1;
const int GEN_PASSWORD_FLAG_NEED_SYMBOL    = 2;

double   genPassword_getBitStrength(uint32_t baseLength);
uint32_t genPassword_getBaseLength (uint32_t bitStrength);
int genPassword      (char password[38], uint32_t bitStrength, int flags = 0);
int genPasswordLength(char password[38], uint32_t baseLength,  int flags = 0);
