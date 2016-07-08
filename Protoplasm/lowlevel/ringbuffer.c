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

#include <defines.h>

#include "ringbuffer.h"

uint16_t ringbuffer_peek(struct ringbuffer_s *rb, uint8_t *dst,
			 uint16_t n) {
	if (n == 0) return 0;

	uint16_t i = rb->readpos;
	uint16_t read = 0;

	while (n - read > 0 && i != rb->writepos) {
		if (dst)
			*dst++ = rb->buffer[i++];
		else
			i++;
		read++;
		if (i >= rb->size) i = 0;
	}

	return read;
}

uint16_t ringbuffer_read(struct ringbuffer_s *rb, uint8_t *dst,
			 uint16_t n) {
	if (n == 0) return 0;

	uint16_t i = rb->readpos;
	uint16_t read = 0;

	while (n - read > 0 && i != rb->writepos) {
		if (dst)
			*dst++ = rb->buffer[i++];
		else
			i++;
		read++;
		if (i >= rb->size) i = 0;
	}

	rb->readpos = i;

	if (rb->readpos == rb->writepos && !rb->writelock) {
		rb->readpos = 0;
		rb->writepos = 0;
	}

	return read;
}

uint16_t ringbuffer_write(struct ringbuffer_s *rb, const uint8_t *src,
			  uint16_t n) {
	if (n == 0) return 0;

	uint16_t i = rb->writepos;
	uint16_t written = 0;

	while (n - written > 0
	       && (i != (rb->readpos > 0 ? rb->readpos - 1 : rb->size - 1))) {
		if (src)
			rb->buffer[i++] = *src++;
		else
			i++;
		written++;
		if (i >= rb->size) i = 0;
	}

	rb->writepos = i;

	rb->writelock = false;

	return written;
}

uint16_t ringbuffer_getreadbuffer(struct ringbuffer_s *rb, uint8_t **chunk,
				  uint16_t *n) {
	*chunk = &rb->buffer[rb->readpos];
	*n = min(*n, ringbuffer_toread(rb));
	*n = min(*n, (uint16_t) (rb->size - rb->readpos));

	return *n;
}

uint16_t ringbuffer_getwritebuffer(struct ringbuffer_s *rb,
				   uint8_t **chunk,
				   uint16_t *n) {
	rb->writelock = true;

	*chunk = &rb->buffer[rb->writepos];
	*n = min(*n, ringbuffer_towrite(rb));
	*n = min(*n, (uint16_t) (rb->size - rb->writepos));

	return *n;
}


uint16_t ringbuffer_toread(struct ringbuffer_s *rb) {
	if (rb->readpos > rb->writepos)
		return rb->writepos + rb->size - rb->readpos;
	else
		return rb->writepos - rb->readpos;
}

uint16_t ringbuffer_towrite(struct ringbuffer_s *rb) {
	if (rb->writepos >= rb->readpos)
		return (uint16_t) (rb->readpos + rb->size - rb->writepos - 1);
	else
		return (uint16_t) (rb->readpos - rb->writepos - 1);
}
