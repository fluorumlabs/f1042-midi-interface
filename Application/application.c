//
// Created by Artem Godin on 30/06/16.
//

#include <app_task.h>
#include <stm32f0xx_hal.h>
#include <ringbuffer.h>
#include <uart.h>
#include <i2c.h>
#include "application.h"
#include "io.h"
#include "midi.h"
#include "process.h"

extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;

bool usb_connected = false;
bool bus_connected = false;
bool was_usb_connected = false;

enum operation_mode_e operation_mode = OPERATION_MIDI_THRU;

// MIDI Filter
uint16_t channelMask = 0xffff;
bool range = false;
uint8_t noteMin = 0;
uint8_t noteMax = 255;

uint8_t keyDowns = 0;

bool normal = false;

void midi_callback(struct ringbuffer_s *appbuffer);

UART_BUFFER(UART_MIDI_TX, 64);
UART_DEFINE({ &huart2, &UART_MIDI_TX });

I2C_BUFFER(I2C_MIDI_TX, 64);
I2C_BUFFER(I2C_MIDI_RX, 64);
I2C_DEFINE({ &hi2c1, &I2C_MIDI_TX, &I2C_MIDI_RX, midi_callback });

#include <app_task_meta.h> //=== APP META BEGIN ===============================

#define EVENT_TRACK_CONNECTION_CHANGE                                          \
        EVENT(APP_EVENT_CONNECTION_STATUS_CHANGE):                             \
                if (usb_connected ^ bus_connected) {                           \
                        SWITCHTO(CONNECTED);                                   \
                }                                                              \
                if (!(usb_connected | bus_connected)) {                        \
                        SWITCHTO(IDLE);                                        \
                }                                                              \
                if (usb_connected & bus_connected) {                           \
                        SWITCHTO(ERROR);                                       \
                }                                                              \
                break;

APPLICATION {
	// Global variables
	uint32_t midi;
	uint32_t tick = 0;
	uint32_t newTick = 0;
	uint8_t cmdDetect = MIDI_ACTIVE_SENSE;
	uint8_t cmdAbort = MIDI_RESERVED_FD;

	// Initialization
	STARTUP:
	STATE_BEGIN
			ENTER:
				application_init();
				config_load();
				SWITCHTO(IDLE);
	STATE_END

	// Idle: no interfaces connected
	IDLE:
	STATE_BEGIN
			ENTER:
				process_synth_noteoff();
				normal = false;
				if (was_usb_connected) {
					led_set(LED_RED, true);
					ringbuffer_write(&I2C_MIDI_TX, &cmdAbort, 1);
					i2c_flush(&I2C_MIDI_TX);
					HAL_Delay(1000);
				}
				// Reset I2C and go slave
				i2c_abort(&hi2c1);
				i2c_start(&I2C_MIDI_RX);
				led_set(LED_BLUE, false);
				led_blink(LED_RED, false, false);
				break;

			EVENT_TRACK_CONNECTION_CHANGE
	STATE_END

	// Error: Attempt to connect two interfaces was made
	ERROR:
	STATE_BEGIN
			ENTER:
				// Reset I2C
				process_synth_noteoff();
				led_set(LED_BLUE, false);
				led_blink(LED_RED, false, true);
				break;

			EVENT_TRACK_CONNECTION_CHANGE
	STATE_END

	// Just connected
	CONNECTED:
	STATE_BEGIN
			ENTER:
				led_set(LED_RED, false);
				if (usb_connected && !bus_connected) {
					// Reset I2C and go master
					i2c_abort(&hi2c1);
					ringbuffer_write(&I2C_MIDI_TX, &cmdDetect, 1);
					i2c_flush(&I2C_MIDI_TX);
					was_usb_connected = true;
				}

				SWITCHTO(NORMAL);
	STATE_END

	// Normal mode: Receive and process MIDI
	NORMAL:
	STATE_BEGIN
			ENTER:
				normal = true;
				process_synth_noteoff();

				led_set(LED_RED, false);
				break;

			EVENT_TRACK_CONNECTION_CHANGE

			EVENT(APP_EVENT_MIDI):
				midi = EVENT_VALUE;
				if (MIDI_COMMAND(midi) >= MIDI_COMMAND_FIRST &&
				    MIDI_COMMAND(midi) <= MIDI_COMMAND_LAST) {
					// Normal command
					int channel = 1 << MIDI_CHANNEL(midi);
					if ((channel & channelMask) == channel) {
						if (operation_mode == OPERATION_MIDI_THRU) {
							process_midi_thru(midi);
						} else if (operation_mode == OPERATION_SYNTH) {
							if ((MIDI_COMMAND(midi) == MIDI_NOTE_ON ||
							     MIDI_COMMAND(midi) == MIDI_NOTE_OFF) &&
							    (MIDI_DATA0(midi) >= noteMin &&
							     MIDI_DATA1(midi) <= noteMax)) {
								process_synth(midi);
							}
						} else if (operation_mode == OPERATION_TRIGGER) {
							if ((MIDI_COMMAND(midi) == MIDI_NOTE_ON ||
							     MIDI_COMMAND(midi) == MIDI_NOTE_OFF) &&
							    (MIDI_DATA0(midi) >= noteMin &&
							     MIDI_DATA1(midi) <= noteMax)) {
								process_trigger(midi);
							}
						}
					}
				} else if (operation_mode == OPERATION_MIDI_THRU) {
					// Other command
					process_midi_thru(midi);
				}
				break;


			EVENT(APP_BUTTON_BOOT_DOWN):
				SWITCHTO(LEARN_CLEAR);
	STATE_END

	// Learn/Clear: reset filters and learn first channel/note
	LEARN_CLEAR:
	STATE_BEGIN
			ENTER:
				// If state entered second time in less than 1 second, clear configuration
				newTick = HAL_GetTick();
				if (tick > 0 && newTick - tick < 1000) {
					noteMin = 0;
					noteMax = 255;
					channelMask = 0xff;
					range = false;
					keyDowns = 0;
				}
				tick = newTick;
				normal = false;
				led_set(LED_RED, true);
				led_set(LED_BLUE, false);
				process_synth_noteoff();
				break;

			EVENT_TRACK_CONNECTION_CHANGE

			EVENT(APP_EVENT_MIDI):
				// Clear config on first event
				midi = EVENT_VALUE;
				noteMin = 0;
				noteMax = 255;
				channelMask = 0xff;
				range = false;
				process_synth_noteoff();

				if (operation_mode == OPERATION_MIDI_THRU &&
				    MIDI_COMMAND(midi) >= MIDI_COMMAND_FIRST &&
				    MIDI_COMMAND(midi) <= MIDI_COMMAND_LAST) {
					uint16_t channel = (uint16_t) (1 << MIDI_CHANNEL(midi));
					range = false;
					channelMask = channel;
					process_midi_thru(midi);
					SWITCHTO(LEARN);
				} else if (MIDI_COMMAND(midi) == MIDI_NOTE_ON ||
					   MIDI_COMMAND(midi) == MIDI_NOTE_OFF) {
					uint16_t channel = (uint16_t) (1 << MIDI_CHANNEL(midi));
					uint8_t note = (uint8_t) MIDI_DATA0(midi);
					range = false;
					noteMin = note;
					noteMax = note;
					channelMask = channel;
					if (operation_mode == OPERATION_SYNTH) {
						process_synth(midi);
					} else if (operation_mode == OPERATION_TRIGGER) {
						process_trigger(midi);
					}
					SWITCHTO(LEARN);
				}
				break;

			EVENT(APP_BUTTON_BOOT_DOWN):
				config_store();
				SWITCHTO(NORMAL);


	STATE_END

	// Learn: adjust filters
	LEARN:
	STATE_BEGIN
			ENTER:
				led_set(LED_BLUE, true);
				break;

			EVENT_TRACK_CONNECTION_CHANGE

			EVENT(APP_EVENT_MIDI):
				midi = EVENT_VALUE;
				if (operation_mode == OPERATION_MIDI_THRU &&
				    MIDI_COMMAND(midi) >= MIDI_COMMAND_FIRST &&
				    MIDI_COMMAND(midi) <= MIDI_COMMAND_LAST) {
					uint16_t channel = (uint16_t) (1 << MIDI_CHANNEL(midi));
					if ((channel & channelMask) == 0) {
						channelMask |=
							channel;
						range = true;
						led_set(LED_PURPLE,
							true);
					}
					process_midi_thru(midi);
				} else if (MIDI_COMMAND(midi) == MIDI_NOTE_ON ||
					   MIDI_COMMAND(midi) == MIDI_NOTE_OFF) {
					uint16_t channel = (uint16_t) (1 << MIDI_CHANNEL(midi));
					uint8_t note = (uint8_t) MIDI_DATA0(midi);
					if (((channel & channelMask) == channel) &&
					    (note < noteMin || note > noteMax)) {
						noteMin = min(noteMin, note);
						noteMax = max(noteMax, note);
						range = true;
						led_set(LED_PURPLE,
							true);
					}
					if (operation_mode == OPERATION_SYNTH) {
						process_synth(midi);
					} else if (operation_mode == OPERATION_TRIGGER) {
						process_trigger(midi);
					}
				}
				break;


			EVENT(APP_BUTTON_BOOT_DOWN):
				if (operation_mode == OPERATION_SYNTH && noteMin == noteMax) {
					// Expand note range for synth
					noteMin = 0;
					noteMax = 255;
				}

				config_store();
				SWITCHTO(NORMAL);


	STATE_END

}

#include <app_task_meta.h> //==================================================
#include <common.h>


void usb_device_reset_callback() {
	application_connection_usb(true);
	midi_reset();
}

void usb_device_suspended_callback() {
	application_connection_usb(false);
}


uint32_t *flash_configuration = ((uint32_t *) (FLASH_BANK1_END + 1 - FLASH_PAGE_SIZE));
uint32_t local_configuration[2];
#define CONFIGURATION_SANITY_MARKER 0xF00FC7C8U

/*
 * Configure operation mode
 */
void application_init() {
	bool readUp;
	bool readDown;

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = MODE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(MODE_GPIO_Port, &GPIO_InitStruct);
	readUp = HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin);

	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(MODE_GPIO_Port, &GPIO_InitStruct);
	readDown = HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin);

	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(MODE_GPIO_Port, &GPIO_InitStruct);

	if (readUp && !readDown) operation_mode = OPERATION_MIDI_THRU;     // Floating
	if (!readUp && !readDown) operation_mode = OPERATION_SYNTH;        // Tied to GND
	if (readUp && readDown) operation_mode = OPERATION_TRIGGER;        // Tied to VCC

	if (operation_mode != OPERATION_MIDI_THRU) {
		// Kill the uart if we are in the wrong mode :)
		HAL_UART_DeInit(&huart2);
	}
	if (operation_mode == OPERATION_TRIGGER || operation_mode == OPERATION_SYNTH) {
		// Init OUT_Pin for gate or trigger
		GPIO_InitStruct.Pin = OUT_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(OUT_GPIO_Port, &GPIO_InitStruct);
	}
}

void application_connection_usb(bool connected) {
	if (connected != usb_connected) {
		usb_connected = connected;
		app_pushevent(APP_EVENT_CONNECTION_STATUS_CHANGE);
	}
}

void application_connection_bus(bool connected) {
	if (connected != bus_connected) {
		bus_connected = connected;
		app_pushevent(APP_EVENT_CONNECTION_STATUS_CHANGE);
	}
}

void config_load() {
	// Check sanity
	if (flash_configuration[0] == CONFIGURATION_SANITY_MARKER) {
		local_configuration[0] = flash_configuration[0];
		local_configuration[1] = flash_configuration[1];

		channelMask = hiword(local_configuration[1]);
		noteMin = lobyte(local_configuration[1]);
		noteMax = hibyte(local_configuration[1]);
	}
}

void config_store() {
	// Check if configuration has changed
	if ((flash_configuration[0] != CONFIGURATION_SANITY_MARKER) ||
	    channelMask != hiword(flash_configuration[1]) ||
	    noteMin != lobyte(flash_configuration[1]) ||
	    noteMax != hibyte(flash_configuration[1])) {
		local_configuration[0] = CONFIGURATION_SANITY_MARKER;
		local_configuration[1] = (channelMask << 16) | (noteMax << 8) | (noteMin);
		HAL_FLASH_Unlock();

		static FLASH_EraseInitTypeDef eraseInitTypeDef;
		uint32_t pageError;

		eraseInitTypeDef.TypeErase = TYPEERASE_PAGES;
		eraseInitTypeDef.PageAddress = (uint32_t) flash_configuration;
		eraseInitTypeDef.NbPages = 1;

		if (HAL_FLASHEx_Erase(&eraseInitTypeDef, &pageError) == HAL_OK) {
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t) flash_configuration,
					  (uint32_t) local_configuration);
		}
		HAL_FLASH_Lock();
	}
}

