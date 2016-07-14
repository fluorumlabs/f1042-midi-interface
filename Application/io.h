//
// Created by Artem Godin on 14/07/16.
//

#ifndef F1042_MIDI_INTERFACE_IO_H
#define F1042_MIDI_INTERFACE_IO_H

#define DEBOUNCE_DELAY_TICKS 100

enum led_e {
    LED_RED = 0x01,
    LED_BLUE = 0x02,
    LED_PURPLE = 0x03
};

enum gpio_e {
    GPIO_OUT = 0x01,
};

void led_blink(enum led_e led, bool once, bool fast);

void led_set(enum led_e led, bool on);

void gpio_blink(enum gpio_e led);

void gpio_set(enum gpio_e led, bool on);


#endif //F1042_MIDI_INTERFACE_IO_H
