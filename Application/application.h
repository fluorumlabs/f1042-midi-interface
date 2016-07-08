//
// Created by Artem Godin on 30/06/16.
//

#ifndef F1042_MIDI_INTERFACE_APPLICATION_H
#define F1042_MIDI_INTERFACE_APPLICATION_H

#include <stdbool.h>
#include <stdint.h>

enum led_e {
    LED_RED = 0x01,
    LED_BLUE = 0x02,
    LED_PURPLE = 0x03
};

enum app_custom_event_e {
    APP_BUTTON_BOOT_UP = 0x01000000,
    APP_BUTTON_BOOT_DOWN = 0x02000000,
    APP_EVENT_CONNECTION_STATUS_CHANGE = 0x03000000,          // connected/disconnected
    APP_EVENT_MIDI = 0x04000000                // MIDI command received
};

/*
 * MIDI event bytes:
 * 	EVENT DATA1 DATA0 STATUS
 */
#define MIDI_STATUS(value) (value & 0xff)
#define MIDI_DATA0(value) ((value & 0xff00) >> 8)
#define MIDI_DATA1(value) ((value & 0xff0000) >> 16)

#define MIDI_CHANNEL(value) (value & 0x0f)
#define MIDI_COMMAND(value) (value & 0xf0)

enum midi_command_e {
    MIDI_NOTE_OFF = 0x80,
    MIDI_NOTE_ON = 0x90,
    MIDI_KEY_AFTERTOUCH = 0xA0,
    MIDI_CC = 0xB0,
    MIDI_PROGRAM_CHANGE = 0xC0,
    MIDI_CHANNEL_AFTERTOUCH = 0xD0,
    MIDI_PITCH_BEND = 0xE0,

    MIDI_COMMAND_FIRST = 0x80,
    MIDI_COMMAND_LAST = 0xE0
};

enum midi_rt_command_e {
    MIDI_MTC_QF = 0xF8,
    MIDI_START = 0xFA,
    MIDI_CONTINUE = 0xFB,
    MIDI_STOP = 0xFC
};


void application_init();

void application_connection_usb(bool connected);

void application_connection_bus(bool connected);

void led_blink(enum led_e led, bool once, bool fast);

void led_set(enum led_e led, bool on);

void config_load();

void config_store();

void process_midi_thru(uint32_t midi);

void process_synth(uint32_t midi);

void process_trigger(uint32_t midi);


#endif //F1042_MIDI_INTERFACE_APPLICATION_H
