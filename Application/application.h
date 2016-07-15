//
// Created by Artem Godin on 30/06/16.
//

#ifndef F1042_MIDI_INTERFACE_APPLICATION_H
#define F1042_MIDI_INTERFACE_APPLICATION_H

#include <stdbool.h>
#include <stdint.h>

enum operation_mode_e {
    OPERATION_MIDI_THRU = 0,
    OPERATION_SYNTH = 1,
    OPERATION_TRIGGER = 2
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
#define MIDI_COMMAND_FULL(value) (value & 0xff)

void application_init();

void application_connection_usb(bool connected);

void application_connection_bus(bool connected);

void config_load();

void config_store();

#endif //F1042_MIDI_INTERFACE_APPLICATION_H
