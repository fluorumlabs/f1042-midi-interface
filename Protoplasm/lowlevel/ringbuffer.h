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
/// @file				ringbuffer.h
///
/// @addtogroup lowlevel		Low-Level API
/// @{
/// @defgroup lowlevel_ringbuffer	Ring Buffer
/// @{

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <defines.h>

/**
	Main structure definition for ring buffers
*/
struct ringbuffer_s {
    uint16_t size;                ///< Size of buffer, bytes
    volatile uint16_t readpos;        ///< Current read position
    volatile uint16_t writepos;        ///< Current write position
    bool writelock;                        ///< getwritebuffer in progress
    uint8_t buffer[];                ///< Actual buffer
};

/**
	Helper macros to simplify creation of new ring buffers

	@param [in] size Desired size of ring buffer

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	struct ringbuffer_s buffer = RINGBUFFER( 1024 );
	// This will create a new ring buffer with capacity of 1024 bytes
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define RINGBUFFER(size) { size, 0, 0, false, { [size-1]=0 } }

/**
	Get amount of data available for reading

	@param [in] rb Pointer to ring buffer structure

	@return Bytes in buffer ready to be read
*/
uint16_t ringbuffer_toread(struct ringbuffer_s *rb);

/**
	Get amount of space available for writing of new data

	@param [in] rb Pointer to ring buffer structure

	@return Available space in buffer ready to be filled with new data
*/
uint16_t ringbuffer_towrite(struct ringbuffer_s *rb);

/**
	Read data from ring buffer without changing reading position.

	@param [in] rb Pointer to ring buffer structure
	@param [out] dst Pointer to the buffer in which data will be read
	@param [in] n Desired size of data chunk to be read.

	@return Actual size of read data. Can be even zero, if there is no readable
	data in buffer.

	@warning There are no thread synchronization, so you must take measures
	so that only one thread at the time attempts to read data.
	@note It is possible to read and write in different threads simultaneously.

	@see ringbuffer_read()
*/
uint16_t ringbuffer_peek(struct ringbuffer_s *rb, uint8_t *dst,
			 uint16_t n);

/**
	Read data from ring buffer and move reading position.

	@param [in] rb Pointer to ring buffer structure
	@param [out] dst Pointer to the buffer in which data will be read. If `null`
	will only move reading position.
	@param [in] n Desired size of data chunk to be read.

	@return Actual size of read data. Can be even zero, if there is no readable
	data in buffer.

	@warning There are no thread synchronization, so you must take measures
	so that only one thread at the time attempts to read data.
	@note It is possible to read and write in different threads simultaneously.

	@see ringbuffer_peek()
*/
uint16_t ringbuffer_read(struct ringbuffer_s *rb, uint8_t *dst,
			 uint16_t n);

/**
	Write data to ring buffer.

	@param [in] rb Pointer to ring buffer structure
	@param [in] src Pointer to the data to write. If `null` will only move reading
	position.
	@param [in] n Size of data to write.

	@return Actual size of written data. Can be even zero, if there is no available
	space in buffer.

	@warning There are no thread synchronization, so you must take measures
	so that only one thread at the time attempts to write data.
	@note It is possible to read and write in different threads simultaneously.
*/
uint16_t ringbuffer_write(struct ringbuffer_s *rb, const uint8_t *src,
			  uint16_t n);

/**
	Get pointer to continuous chunk of buffer, from which data can be read.

	@param [in] rb Pointer to ring buffer structure.
	@param [out] chunk Pointer to variable to store pointer to continuous chunk of
	readable buffer.
	@param [in,out] n Size of continuous chunk. Can be as low as zero and as high as
	a requested size.

	@return Size of continuous chunk.

	@warning There are no thread synchronization, so you must take measures
	so that only one thread at the time attempts to read data.
	@note It is possible to read and write in different threads simultaneously.
*/
uint16_t ringbuffer_getreadbuffer(struct ringbuffer_s *rb, uint8_t **chunk,
				  uint16_t *n);

/**
	Get pointer to continuous chunk of buffer, to which data can be written.

	@param [in] rb Pointer to ring buffer structure.
	@param [out] chunk Pointer to variable to store pointer to continuous chunk of
	readable buffer.
	@param [in,out] n Size of continuous chunk. Can be as low as zero and as high as

	@return Size of continuous chunk.

	@warning There are no thread synchronization, so you must take measures
	so that only one thread at the time attempts to write data.
	@note It is possible to read and write in different threads simultaneously.
*/
uint16_t ringbuffer_getwritebuffer(struct ringbuffer_s *rb,
				   uint8_t **chunk,
				   uint16_t *n);

#endif // RINGBUFFER_H

/// @}
/// @}
///////////////////////////////////////////////////////////////////////////////
