//
// Created by Artem Godin on 14/07/16.
//

#ifndef F1042_MIDI_INTERFACE_I2C_H
#define F1042_MIDI_INTERFACE_I2C_H

#include <stm32f0xx_hal_conf.h>
#include <ringbuffer.h>

struct i2c_handle_s {
    I2C_HandleTypeDef *i2cHandleTypeDef;
    struct ringbuffer_s *bufferOut;
    struct ringbuffer_s *bufferIn;

    void ( *callback )(struct ringbuffer_s *appbuffer);

    // internal
    uint16_t _send;
    uint16_t _receive;
};

#define I2C_BUFFER(name, size) struct ringbuffer_s name = RINGBUFFER( size )
#define I2C_DEFINE(...) struct i2c_handle_s _i2c_handle[] = { __VA_ARGS__ }; uint8_t _i2c_handle_count = sizeof(_i2c_handle)/sizeof(struct i2c_handle_s);


void i2c_abort(I2C_HandleTypeDef *I2cHandle);

void i2c_flush(struct ringbuffer_s *bufferOut);

void i2c_start(struct ringbuffer_s *bufferOut);

#endif //F1042_MIDI_INTERFACE_I2C_H
