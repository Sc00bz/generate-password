/*
	CSPRNG - A CSPRNG and modulo a random number without bias.

	Written in 2016-2018 Steve "Sc00bz" Thomas (steve at tobtu dot com)

	To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring
	rights to this software to the public domain worldwide. This software is distributed without any warranty.

	You should have received a copy of the CC0 Public Domain Dedication along with this software.
	If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#pragma once

#include <stdint.h>

void secureClearMemory(void *mem, size_t size);
int getRandom(void *buffer, size_t size);

uint8_t  randomUint8();
uint16_t randomUint16();
uint32_t randomUint32();
uint64_t randomUint64();

uint8_t  randomUint8 (uint8_t  max);
uint16_t randomUint16(uint16_t max);
uint32_t randomUint32(uint32_t max);
uint64_t randomUint64(uint64_t max);
