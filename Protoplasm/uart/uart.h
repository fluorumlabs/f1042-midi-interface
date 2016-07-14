//
// Created by Artem Godin on 14/07/16.
//

#ifndef F1042_MIDI_INTERFACE_UART_H
#define F1042_MIDI_INTERFACE_UART_H

#include <stm32f0xx_hal_conf.h>
#include <ringbuffer.h>

struct uart_handle_s {
    UART_HandleTypeDef *uartHandleTypeDef;
    struct ringbuffer_s *bufferOut;

    // internal
    uint16_t _send;
};

#define UART_BUFFER(name, size) struct ringbuffer_s name = RINGBUFFER( size )
#define UART_DEFINE(...) struct uart_handle_s _uart_handle[] = { __VA_ARGS__ }; uint8_t _uart_handle_count = sizeof(_uart_handle)/sizeof(struct uart_handle_s);

void uart_flush(struct ringbuffer_s *bufferOut);

#endif //F1042_MIDI_INTERFACE_UART_H
