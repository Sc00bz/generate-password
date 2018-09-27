/*
	CSPRNG - A CSPRNG and modulo a random number without bias.

	Written in 2016-2018 Steve "Sc00bz" Thomas (steve at tobtu dot com)

    To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring
	rights to this software to the public domain worldwide. This software is distributed without any warranty.

    You should have received a copy of the CC0 Public Domain Dedication along with this software.
	If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#define __STDC_WANT_LIB_EXT1__ 1
#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csprng.h"

/**
 * Clears memory.
 *
 * @param mem  - Pointer to data to clear.
 * @param size - Size of data.
 */
void secureClearMemory(void *mem, size_t size)
{
	if (size > 0)
	{
#ifdef __STDC_LIB_EXT1__
		memset_s(mem, size, 0, size);
#elif defined(_WIN32)
		SecureZeroMemory(mem, size);
#else
		volatile uint8_t *p = (volatile uint8_t*) mem;
		do
		{
			*p = 0;
			p++;
		} while (--size);
#endif
	}
}

/**
 * Fills buffer with random using a CSPRNG.
 *
 * @param buffer - Buffer to receive the random data.
 * @param size   - Size of buffer.
 * @return Zero on success, otherwise non-zero
 */
int getRandom(void *buffer, size_t size)
{
	if (size > 0)
	{
#ifdef _WIN32
		static HCRYPTPROV hCryptProv = NULL;
		const DWORD       DWORD_MAX  = (((DWORD) 1) << (8 * sizeof(DWORD) - 1)) - 1;

		if (hCryptProv == NULL && !CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
		{
			fprintf(stderr, "Error CryptAcquireContext\n");
			hCryptProv = NULL;
			return -1;
		}

		while (size > 0)
		{
			DWORD curSize = (DWORD) size;

			if ((size_t) curSize != size || curSize < 0)
			{
				curSize = DWORD_MAX;
			}
			if (!CryptGenRandom(hCryptProv, curSize, (BYTE*) buffer))
			{
				fprintf(stderr, "Error CryptGenRandom\n");
				return -1;
			}
			size -= (size_t) curSize;
			buffer = ((uint8_t*) buffer) + curSize;
		}
#else
		int fin = open("/dev/urandom", O_RDONLY);

		if (fin == -1)
		{
			fprintf(stderr, "Error can\'t open \"/dev/urandom\"\n");
			return -1;
		}

		while (size > 0)
		{
			ssize_t curSize = (ssize_t) size;

			if (size > (size_t) SSIZE_MAX)
			{
				curSize = SSIZE_MAX;
			}
			if (read(fin, buffer, curSize) != (ssize_t) curSize)
			{
				close(fin);
				fprintf(stderr, "Error reading \"/dev/urandom\"\n");
				return -1;
			}
			size -= (size_t) curSize;
			buffer = ((uint8_t*) buffer) + curSize;
		}
		close(fin);
#endif
	}

	return 0;
}

/**
 * Generates a random uint8_t using a CSPRNG and will exit(1) on error.
 *
 * @return A random value 0 to 2**8-1
 */
uint8_t randomUint8()
{
	uint8_t num;

	if (getRandom(&num, sizeof(num)))
	{
		exit(1);
	}
	return num;
}

/**
 * Generates a random uint16_t using a CSPRNG and will exit(1) on error.
 *
 * @return A random value 0 to 2**16-1
 */
uint16_t randomUint16()
{
	uint16_t num;

	if (getRandom(&num, sizeof(num)))
	{
		exit(1);
	}
	return num;
}

/**
 * Generates a random uint32_t using a CSPRNG and will exit(1) on error.
 *
 * @return A random value 0 to 2**32-1
 */
uint32_t randomUint32()
{
	uint32_t num;

	if (getRandom(&num, sizeof(num)))
	{
		exit(1);
	}
	return num;
}

/**
 * Generates a random uint64_t using a CSPRNG and will exit(1) on error.
 *
 * @return A random value 0 to 2**64-1
 */
uint64_t randomUint64()
{
	uint64_t num;

	if (getRandom(&num, sizeof(num)))
	{
		exit(1);
	}
	return num;
}

/**
 * Generates a random uint8_t with no modulo bias and will exit(1) on error.
 *
 * @param max - The maximum random value to output.
 * @return A random value 0 to max
 */
uint8_t randomUint8(uint8_t max)
{
	uint8_t ret;

	// If nice base 2 modulo (ie 2**n)
	if (((max + 1) & max) == 0)
	{
		ret = randomUint8() & max;
	}
	else
	{
		// skip is "n*(max+1)" for the largest n such that "n*(max+1) < 2**8"
		uint8_t skip = UINT8_MAX - UINT8_MAX % (max + 1);

		// Loop until random number is less than skip then modulo.
		// Note worst case is max is 2**7+1 and chances of looping are just less
		// than 50%. The chances of looping 128 times is so low (<1 in 2**128)
		// that the heat death of the universe would happen before this.
		do
		{
			ret = randomUint8();
		} while (ret >= skip);
		ret %= max + 1;
	}
	return ret;
}

/**
 * Generates a random uint16_t with no modulo bias and will exit(1) on error.
 *
 * @param max - The maximum random value to output.
 * @return A random value 0 to max
 */
uint16_t randomUint16(uint16_t max)
{
	uint16_t ret;

	// If nice base 2 modulo (ie 2**n)
	if (((max + 1) & max) == 0)
	{
		ret = randomUint16() & max;
	}
	else
	{
		// skip is "n*(max+1)" for the largest n such that "n*(max+1) < 2**16"
		uint16_t skip = UINT16_MAX - UINT16_MAX % (max + 1);

		// Loop until random number is less than skip then modulo.
		// Note worst case is max is 2**15+1 and chances of looping are just less
		// than 50%. The chances of looping 128 times is so low (<1 in 2**128)
		// that the heat death of the universe would happen before this.
		do
		{
			ret = randomUint16();
		} while (ret >= skip);
		ret %= max + 1;
	}
	return ret;
}

/**
 * Generates a random uint32_t with no modulo bias and will exit(1) on error.
 *
 * @param max - The maximum random value to output.
 * @return A random value 0 to max
 */
uint32_t randomUint32(uint32_t max)
{
	uint32_t ret;

	// If nice base 2 modulo (ie 2**n)
	if (((max + 1) & max) == 0)
	{
		ret = randomUint32() & max;
	}
	else
	{
		// skip is "n*(max+1)" for the largest n such that "n*(max+1) < 2**32"
		uint32_t skip = UINT32_MAX - UINT32_MAX % (max + 1);

		// Loop until random number is less than skip then modulo.
		// Note worst case is max is 2**31+1 and chances of looping are just less
		// than 50%. The chances of looping 128 times is so low (<1 in 2**128)
		// that the heat death of the universe would happen before this.
		do
		{
			ret = randomUint32();
		} while (ret >= skip);
		ret %= max + 1;
	}
	return ret;
}

/**
 * Generates a random uint64_t with no modulo bias and will exit(1) on error.
 *
 * @param max - The maximum random value to output.
 * @return A random value 0 to max
 */
uint64_t randomUint64(uint64_t max)
{
	uint64_t ret;

	// If nice base 2 modulo (ie 2**n)
	if (((max + 1) & max) == 0)
	{
		ret = randomUint64() & max;
	}
	else
	{
		// skip is "n*(max+1)" for the largest n such that "n*(max+1) < 2**64"
		uint64_t skip = UINT64_MAX - UINT64_MAX % (max + UINT64_C(1));

		// Loop until random number is less than skip then modulo.
		// Note worst case is max is 2**63+1 and chances of looping are just less
		// than 50%. The chances of looping 128 times is so low (<1 in 2**128)
		// that the heat death of the universe would happen before this.
		do
		{
			ret = randomUint64();
		} while (ret >= skip);
		ret %= max + 1;
	}
	return ret;
}
