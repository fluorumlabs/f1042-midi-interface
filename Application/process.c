#include <sched.h>
#include <stdbool.h>
#include <defines.h>
#include <ringbuffer.h>
#include <uart.h>
#include "io.h"
#include "application.h"
#include "midi.h"

//
// Created by Artem Godin on 14/07/16.
//
extern uint8_t keyDowns;
extern bool normal;
extern enum operation_mode_e operation_mode;

extern struct ringbuffer_s UART_MIDI_TX;

void process_synth_noteoff() {
	keyDowns = 0;
	if (operation_mode == OPERATION_SYNTH) {
		gpio_set(GPIO_OUT, false);
	}
}

void process_midi(uint32_t midi) {
	if (MIDI_COMMAND(midi) == MIDI_NOTE_ON) {
		keyDowns++;
		if (normal) {
			led_set(LED_BLUE, true);
			led_blink(LED_RED, true, true);
		}
	} else if (MIDI_COMMAND(midi) == MIDI_NOTE_OFF) {
		if (keyDowns > 0) {
			keyDowns--;
		}
		if (keyDowns == 0 && normal) {
			led_set(LED_BLUE, false);
		}
	}
}

void process_midi_thru(uint32_t midi) {
	uint8_t command[3];
	uint16_t commandLen;

	process_midi(midi);

	command[0] = tobyte(MIDI_COMMAND_FULL(midi));
	commandLen = midi_length_lut[command[0]];
	if (commandLen > 2) return;
	switch (commandLen) {
	case 2:
		command[2] = tobyte(MIDI_DATA1(midi));
	case 1:
		command[1] = tobyte(MIDI_DATA0(midi));
	default:
		break;
	}
	ringbuffer_write(&UART_MIDI_TX, command, (uint16_t) (commandLen + 1));
	uart_flush(&UART_MIDI_TX);
}


void process_synth(uint32_t midi) {
	process_midi(midi);
	if (keyDowns > 0) {
		gpio_set(GPIO_OUT, true);
	} else {
		gpio_set(GPIO_OUT, false);
	}
}

void process_trigger(uint32_t midi) {
	process_midi(midi);
	uint8_t command = tobyte(MIDI_COMMAND(midi));
	if (command == MIDI_NOTE_ON) {
		gpio_set(GPIO_OUT, false);
		gpio_blink(GPIO_OUT);
	}
}

