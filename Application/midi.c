//
// Created by Artem Godin on 14/07/16.
//

#include <ringbuffer.h>
#include <app_task.h>
#include <common.h>
#include <i2c.h>
#include "application.h"
#include "midi.h"

enum midi_state_e {
    STATE_RESET = 0,
    STATE_SYSEX
};

static enum midi_state_e _midi_state = STATE_RESET;
static uint8_t _midi_offset = 0;
static uint8_t _midi_args = 0;
static uint32_t _midi = 0;

extern struct ringbuffer_s BUFFER_MIDI_TX;
extern struct ringbuffer_s I2C_MIDI_RX;
extern struct ringbuffer_s I2C_MIDI_TX;

extern bool usb_connected;
extern bool bus_connected;

void midi_reset() {
	_midi_state = STATE_RESET;
	_midi = 0;
	_midi_offset = 0;
	_midi_args = 0;
}

void midi_callback(struct ringbuffer_s *appbuffer) {
	uint16_t chunk_size = ringbuffer_toread(appbuffer);
	uint8_t *chunk;

	if (appbuffer == &I2C_MIDI_RX) {
		application_connection_bus(true);
	}

	while (ringbuffer_toread(appbuffer) > 0) {
		ringbuffer_getreadbuffer(appbuffer, &chunk, &chunk_size);
		// Echo back to host for debugging
		ringbuffer_write(&BUFFER_MIDI_TX, chunk, chunk_size);

		if (usb_connected && !bus_connected) {
			ringbuffer_write(&I2C_MIDI_TX, chunk, chunk_size);
		}

		for (uint32_t i = 0; i < chunk_size; i++) {
			uint8_t cmd = chunk[i];
			if (appbuffer == &I2C_MIDI_RX && cmd == MIDI_RESERVED_F4) {
				application_connection_bus(false);
			}
			if (_midi_state == STATE_SYSEX) {
				//FIXME Allow RT messages within SysEx
				if (cmd == MIDI_SYSEX_END) {
					_midi_state = STATE_RESET;
				}
			} else {
				if (cmd == MIDI_SYSEX_START) {
					_midi_state = STATE_SYSEX;
				} else if ((cmd & MIDI_SYSEX_START) < MIDI_COMMAND_FIRST) {
					if (_midi_args > 0) {
						_midi |= cmd << _midi_offset;
						_midi_offset += 8;
						_midi_args--;
						if (_midi_args == 0) {
							app_pushevent(APP_EVENT_MIDI | _midi);
						}
					}
				} else {
					_midi = cmd;
					_midi_offset = 8;
					_midi_args = midi_length_lut[cmd];
					if (_midi_args == 0) {
						app_pushevent(APP_EVENT_MIDI | _midi);
					}
				}
			}
		}
		ringbuffer_read(appbuffer, 0, chunk_size);
	}
	// Echo back to host for debugging
	usb_flush(&BUFFER_MIDI_TX);
	if (usb_connected && !bus_connected) {
		// Send to bus
		i2c_flush(&I2C_MIDI_TX);
	}
}
