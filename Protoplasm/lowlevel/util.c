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

#include <util.h>
#include <string.h>

uint32_t system_delay_us_mult = 100;


void memcpy_volatile(volatile void *dst, void *src, size_t len) {
	volatile uint8_t *_dst = (volatile uint8_t *) dst;
	uint8_t *_src = (uint8_t *) src;

	while (len--) {
		*(_dst++) = *(_src++);
	}
}

