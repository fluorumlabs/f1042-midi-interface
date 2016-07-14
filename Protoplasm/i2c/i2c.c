//
// Created by Artem Godin on 14/07/16.
//

#include <stm32f0xx_hal.h>
#include "i2c.h"

extern struct i2c_handle_s _i2c_handle[];
extern uint8_t _i2c_handle_count;

void i2c_abort(I2C_HandleTypeDef *I2cHandle) {
	HAL_I2C_DeInit(I2cHandle);
	for (uint8_t i = 0; i < _i2c_handle_count; i++) {
		if (_i2c_handle[i].i2cHandleTypeDef == I2cHandle) {
			struct i2c_handle_s *i2c = &_i2c_handle[i];
			// clear buffers
			ringbuffer_read(i2c->bufferOut, null, i2c->bufferOut->size);
			ringbuffer_read(i2c->bufferIn, null, i2c->bufferIn->size);
		}
	}
	HAL_I2C_Init(I2cHandle);
}

void i2c_start(struct ringbuffer_s *bufferIn) {
	for (uint8_t i = 0; i < _i2c_handle_count; i++) {
		if (_i2c_handle[i].bufferIn == bufferIn) {
			struct i2c_handle_s *i2c = &_i2c_handle[i];
			uint8_t *chunk;
			uint16_t chunk_size = 1; // One byte at a time
			ringbuffer_getwritebuffer(i2c->bufferIn, &chunk, &chunk_size);
			if (chunk_size > 0) {
				i2c->_receive = chunk_size;
				HAL_I2C_Slave_Receive_IT(i2c->i2cHandleTypeDef, chunk, chunk_size);
			}
			return;
		}
	}
}

void i2c_flush(struct ringbuffer_s *bufferOut) {
	for (uint8_t i = 0; i < _i2c_handle_count; i++) {
		if (_i2c_handle[i].bufferOut == bufferOut) {
			struct i2c_handle_s *i2c = &_i2c_handle[i];
			if (i2c->_send == 0) {
				uint8_t *chunk;
				uint16_t chunk_size = 1;
				ringbuffer_getreadbuffer(i2c->bufferOut, &chunk, &chunk_size);
				if (chunk_size > 0) {
					i2c->_send = chunk_size;
					HAL_I2C_Master_Transmit_IT(i2c->i2cHandleTypeDef, I2C_ADDRESS, chunk,
								   chunk_size);
				}
			}
			return;
		}
	}

}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle) {
	for (uint8_t i = 0; i < _i2c_handle_count; i++) {
		if (_i2c_handle[i].i2cHandleTypeDef == I2cHandle) {
			struct i2c_handle_s *i2c = &_i2c_handle[i];
			if (i2c->_send != 0) {
				ringbuffer_read(i2c->bufferOut, null, i2c->_send);
				i2c->_send = 0;
			}
			uint8_t *chunk;
			uint16_t chunk_size = 1; // One byte at a time
			ringbuffer_getreadbuffer(i2c->bufferOut, &chunk, &chunk_size);
			if (chunk_size > 0) {
				i2c->_send = chunk_size;
				HAL_I2C_Master_Transmit_IT(I2cHandle, I2C_ADDRESS, chunk, chunk_size);
			}
			return;

		}
	}
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *I2cHandle) {
	for (uint8_t i = 0; i < _i2c_handle_count; i++) {
		if (_i2c_handle[i].i2cHandleTypeDef == I2cHandle) {
			struct i2c_handle_s *i2c = &_i2c_handle[i];
			if (i2c->_receive != 0) {
				ringbuffer_write(i2c->bufferIn, null, i2c->_receive);
				i2c->_receive = 0;
			}
			uint8_t *chunk;
			uint16_t chunk_size = 1; // One byte at a time
			ringbuffer_getwritebuffer(i2c->bufferIn, &chunk, &chunk_size);
			if (chunk_size > 0) {
				i2c->_receive = chunk_size;
				HAL_I2C_Slave_Receive_IT(I2cHandle, chunk, chunk_size);
			}
			invoke(i2c->callback)(i2c->bufferIn);
			return;

		}
	}
}