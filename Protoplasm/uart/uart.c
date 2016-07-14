//
// Created by Artem Godin on 14/07/16.
//
#include "uart.h"

extern struct uart_handle_s _uart_handle[];
extern uint8_t _uart_handle_count;

void uart_flush(struct ringbuffer_s *bufferOut) {
	for (uint8_t i = 0; i < _uart_handle_count; i++) {
		if (_uart_handle[i].bufferOut == bufferOut) {
			struct uart_handle_s *uart = &_uart_handle[i];
			if (uart->_send == 0) {
				uint8_t *chunk;
				uint16_t chunk_size = uart->bufferOut->size;
				ringbuffer_getreadbuffer(uart->bufferOut, &chunk, &chunk_size);
				if (chunk_size > 0) {
					uart->_send = chunk_size;
					HAL_UART_Transmit_IT(uart->uartHandleTypeDef, chunk, chunk_size);
				}
			}
			return;
		}
	}

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle) {
	for (uint8_t i = 0; i < _uart_handle_count; i++) {
		if (_uart_handle[i].uartHandleTypeDef == UartHandle) {
			struct uart_handle_s *uart = &_uart_handle[i];
			if (uart->_send != 0) {
				ringbuffer_read(uart->bufferOut, null, uart->_send);
				uart->_send = 0;
			}
			uint8_t *chunk;
			uint16_t chunk_size = uart->bufferOut->size;
			ringbuffer_getreadbuffer(uart->bufferOut, &chunk, &chunk_size);
			if (chunk_size > 0) {
				uart->_send = chunk_size;
				HAL_UART_Transmit_IT(UartHandle, chunk, chunk_size);
			}
			return;
		}
	}
}

