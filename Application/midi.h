//
// Created by Artem Godin on 14/07/16.
//

#ifndef F1042_MIDI_INTERFACE_MIDI_H
#define F1042_MIDI_INTERFACE_MIDI_H


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

enum midi_common_command_e {
    MIDI_SYSEX_START = 0xF0,
    MIDI_SYSEX_END = 0xF7,
    MIDI_MTC_QF = 0xF1,
    MIDI_SONG_POSITION = 0xF2,
    MIDI_SONG_SELECT = 0xF3,
    MIDI_RESERVED_F4 = 0xF4,
    MIDI_RESERVED_F5 = 0xF5,
    MIDI_TUNE = 0xF6
};


enum midi_rt_command_e {
    MIDI_MTC = 0xF8,
    MIDI_RESERVED_F9 = 0xF9,
    MIDI_START = 0xFA,
    MIDI_CONTINUE = 0xFB,
    MIDI_STOP = 0xFC,
    MIDI_RESERVED_FD = 0xFD,
    MIDI_ACTIVE_SENSE = 0xFE,
    MIDI_RESET = 0xFF,
};

static const uint8_t midi_length_lut[0x100] = {
	[MIDI_NOTE_OFF...MIDI_NOTE_OFF | 0xF] = 2,
	[MIDI_NOTE_ON...MIDI_NOTE_ON | 0xF] = 2,
	[MIDI_KEY_AFTERTOUCH...MIDI_KEY_AFTERTOUCH | 0xF]=2,
	[MIDI_CC...MIDI_CC | 0xF]=2,
	[MIDI_PROGRAM_CHANGE...MIDI_PROGRAM_CHANGE | 0xF]=1,
	[MIDI_CHANNEL_AFTERTOUCH...MIDI_CHANNEL_AFTERTOUCH | 0xF]=1,
	[MIDI_PITCH_BEND...MIDI_PITCH_BEND | 0xF]=2,
	[MIDI_MTC_QF]=1,
	[MIDI_SONG_POSITION]=2,
	[MIDI_SONG_SELECT]=1,
};


void midi_callback(struct ringbuffer_s *appbuffer);

void midi_reset();

#endif //F1042_MIDI_INTERFACE_MIDI_H
