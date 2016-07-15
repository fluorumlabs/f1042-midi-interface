/*
	ProtoPlasm: DIY digital pro-audio platform
	Copyright (C) 2013  Artem Godin

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see [http://www.gnu.org/licenses/].
*/

///////////////////////////////////////////////////////////////////////////////
/// @file				util.h
///
/// @addtogroup lowlevel		Low-Level API
/// @{
/// @defgroup lowlevel_util		Utilities
/// @{

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#include <defines.h>

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name Service functions
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	64-bit unsigned integer division.

	This is the binary version of long division.

	@param [in] n Nominator.
	@param [in] d Denominator.
	@return @f$ n/d @f$

*/
static inline uint64_t div64(register uint64_t n, register uint32_t d) {
	register uint64_t q = 0;
	register uint64_t r = 0;

	if ((n >> 32) == 0) return todword(n) / d;

	for (unsigned int i = 64; i > 1; i--) {
		r <<= 1;
		r |= (n >> 63) & 1;
		n <<= 1;
		if (r >= d) {
			r -= d;
			q |= 1;
		}
		q <<= 1;
	}

	r <<= 1;
	r |= n & 1;

	if (r >= d) {
		q |= 1;
	}

	return q;
}


//{
//	uint64_t rem = n;
//	uint64_t b = base;
//	uint64_t res, d = 1;
//	uint32_t high = todword( rem >> 32 );

//	/* Reduce the thing a bit first */
//	res = 0;
//	if (high >= base) {
//		high /= base;
//		res = (uint64_t) high << 32;
//		rem -= (uint64_t) (high*base) << 32;
//	}

//	while ((int64_t)b > 0 && b < rem) {
//		b = b+b;
//		d = d+d;
//	}

//	do {
//		if (rem >= b) {
//			rem -= b;
//			res += d;
//		}
//		b >>= 1;
//		d >>= 1;
//	} while (d);

//	return res;
//}

/**
	Copy data to volatile destination. Pretty similiar to `memcpy`
	otherwise.

	@param [out] dst Pointer to destination volatile memory location.
	@param [in] src Pointer to source data.
	@param [in] len Size of data to copy.
*/
void memcpy_volatile(volatile void *dst, void *src, size_t len);

/// @}

#endif // UTIL_H

/// @}
/// @}
///////////////////////////////////////////////////////////////////////////////
