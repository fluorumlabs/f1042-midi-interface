//
// Created by Artem Godin on 30/06/16.
//

#include <app_task.h>
#include "application.h"

bool usb_connected = false;
bool bus_connected = false;

enum operation_mode_e {
    OPERATION_MIDI_THRU = 0,
    OPERATION_SYNTH = 1,
    OPERATION_TRIGGER = 2
} operation_mode = OPERATION_MIDI_THRU;

// MIDI Filter
uint16_t channelMask = 0xffff;
bool range = false;
uint8_t noteMin = 0;
uint8_t noteMax = 255;

#define DEBOUNCE_DELAY_TICKS 100

#include <app_task_meta.h> //=== APP META BEGIN ===============================

#define EVENT_TRACK_CONNECTION_CHANGE                                          \
        EVENT(APP_EVENT_CONNECTION_STATUS_CHANGE):                             \
                if (usb_connected ^ bus_connected) {                           \
                        SWITCHTO(NORMAL);                                      \
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
				led_blink(LED_RED, false, false);
				break;

			EVENT_TRACK_CONNECTION_CHANGE
	STATE_END

	// Error: Attempt to connect two interfaces was made
	ERROR:
	STATE_BEGIN
			ENTER:
				led_blink(LED_RED, false, true);
				break;

			EVENT_TRACK_CONNECTION_CHANGE
	STATE_END

	// Normal mode: Receive and process MIDI
	NORMAL:
	STATE_BEGIN
			ENTER:
				led_blink(LED_BLUE, true, true);
				break;

			EVENT_TRACK_CONNECTION_CHANGE

			EVENT(APP_EVENT_MIDI):
				midi = EVENT_VALUE;
				if (MIDI_COMMAND(midi) >= MIDI_COMMAND_FIRST &&
				    MIDI_COMMAND(midi) <= MIDI_COMMAND_LAST) {
					// Normal command
					int channel = 1 << MIDI_CHANNEL(midi);
					if (channel & channelMask == channel) {
						if (operation_mode == OPERATION_MIDI_THRU) {
							//led_blink(range ? LED_PURPLE : LED_BLUE, true, true);
							process_midi_thru(midi);
						} else if (operation_mode == OPERATION_SYNTH) {
							if ((MIDI_COMMAND(midi) == MIDI_NOTE_ON ||
							     MIDI_COMMAND(midi) == MIDI_NOTE_OFF) &&
							    (MIDI_DATA0(midi) >= noteMin &&
							     MIDI_DATA1(midi) <= noteMax)) {
								//led_blink(range ? LED_PURPLE : LED_BLUE, true,
								//true);
								process_synth(midi);
							}
						} else if (operation_mode == OPERATION_TRIGGER) {
							if ((MIDI_COMMAND(midi) == MIDI_NOTE_ON ||
							     MIDI_COMMAND(midi) == MIDI_NOTE_OFF) &&
							    (MIDI_DATA0(midi) >= noteMin &&
							     MIDI_DATA1(midi) <= noteMax)) {
								//led_blink(range ? LED_PURPLE : LED_BLUE, true,
								//true);
								process_trigger(midi);
							}
						}
					}
				} else if (operation_mode == OPERATION_MIDI_THRU) {
					// RT command
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
				noteMin = 0;
				noteMax = 255;
				channelMask = 0xff;
				range = false;

				led_set(LED_RED, true);
				break;

			EVENT_TRACK_CONNECTION_CHANGE

			EVENT(APP_EVENT_MIDI):
				midi = EVENT_VALUE;
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
				//led_set(LED_BLUE, true);
				break;

			EVENT_TRACK_CONNECTION_CHANGE

			EVENT(APP_EVENT_MIDI):
				midi = EVENT_VALUE;
				if (operation_mode == OPERATION_MIDI_THRU &&
				    MIDI_COMMAND(midi) >= MIDI_COMMAND_FIRST &&
				    MIDI_COMMAND(midi) <= MIDI_COMMAND_LAST) {
					uint16_t channel = (uint16_t) (1 << MIDI_CHANNEL(midi));
					if (channel & channelMask == 0) {
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
					if ((channel & channelMask == channel) &&
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
#include <stm32f0xx_hal_conf.h>
#include <stm32f0xx_hal.h>
#include <ringbuffer.h>

uint32_t *flash_configuration = ((uint32_t *) (FLASH_BANK1_END + 1 - FLASH_PAGE_SIZE));
uint32_t local_configuration[2];
#define CONFIGURATION_SANITY_MARKER 0xF00FC7C8U

extern TIM_HandleTypeDef htim1;

// Internal
long _buttonTimestamp = 0;
bool _buttonState = false;

int _blinkCount = 0;
int _blinkDuration = 0;
int _blinkDurationCount = 0;

uint16_t _led;

void led_blink(enum led_e led, bool once, bool fast) {
	_led = (uint16_t) (((led & LED_RED) ? RED_Pin : 0) | ((led & LED_BLUE) ? BLUE_Pin : 0));

	_blinkCount = once ? 1 : 100;
	_blinkDuration = fast ? 1 : 5;
	_blinkDurationCount = 0;

	HAL_TIM_Base_Stop_IT(&htim1);
	HAL_GPIO_WritePin(GPIOA, RED_Pin | BLUE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, _led, GPIO_PIN_SET);
	HAL_TIM_Base_Start_IT(&htim1);

}

void led_set(enum led_e led, bool on) {
	HAL_TIM_Base_Stop_IT(&htim1);
	uint16_t l = (uint16_t) (((led & LED_RED) ? RED_Pin : 0) | ((led & LED_BLUE) ? BLUE_Pin : 0));
	HAL_GPIO_WritePin(GPIOA, RED_Pin | BLUE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, l, GPIO_PIN_SET);
}


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

void process_midi_thru(uint32_t midi) {
	// TODO Implementation
}

void process_synth(uint32_t midi) {
	// TODO Implementation
}

void process_trigger(uint32_t midi) {
	// TODO Implementation
}

/*
 * Interrupt handlers
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == BOOT_Pin) {
		bool newState = (HAL_GPIO_ReadPin(BOOT_GPIO_Port, BOOT_Pin) == GPIO_PIN_SET);
		long now = HAL_GetTick();
		if (newState && now - _buttonTimestamp > DEBOUNCE_DELAY_TICKS) {
			app_pushevent(APP_BUTTON_BOOT_DOWN);
			_buttonTimestamp = now;
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	unused htim;

	_blinkDurationCount++;
	if (_blinkDurationCount >= _blinkDuration) {
		_blinkDurationCount = 0;
		_blinkCount = (_blinkCount >= 100) ? 100 : _blinkCount - 1;
		if (_blinkCount == 0) {
			HAL_GPIO_WritePin(GPIOA, _led, GPIO_PIN_RESET);
			HAL_TIM_Base_Stop_IT(&htim1);
		} else {
			HAL_GPIO_TogglePin(GPIOA, _led);
		}
	}
}

void USBD_MIDI_ReceivedCallback(struct ringbuffer_s *midi_in_buffer) {
	uint16_t chunk_size;
	uint16_t chunk_read = 0;
	uint8_t *chunk;
	uint32_t midi;
	uint8_t midi_offset;

	ringbuffer_getreadbuffer(midi_in_buffer, &chunk, &chunk_size);
	// Echo back to host
	//USBD_MIDI_Transmit(chunk, chunk_size);

	while (chunk_read < chunk_size) {
		// Skip shit :)
		while (chunk_read < chunk_size && (chunk[chunk_read] & 0x80) != 0x80) chunk_read++;
		midi_offset = 0;
		midi = 0;
		while (chunk_read < chunk_size && (chunk[chunk_read] & 0x80) != 0x80 && midi_offset < 24) {
			midi |= chunk[chunk_read++] << midi_offset;
			midi_offset += 8;
		}
		// Now we should have our midi command
		app_pushevent(APP_EVENT_MIDI | midi);
	}
	ringbuffer_read(midi_in_buffer, 0, chunk_size);
}
