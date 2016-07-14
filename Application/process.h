//
// Created by Artem Godin on 14/07/16.
//

#ifndef F1042_MIDI_INTERFACE_PROCESS_H
#define F1042_MIDI_INTERFACE_PROCESS_H

void process_synth_noteoff();

void process_midi(uint32_t midi);

void process_midi_thru(uint32_t midi);

void process_synth(uint32_t midi);

void process_trigger(uint32_t midi);

#endif //F1042_MIDI_INTERFACE_PROCESS_H
