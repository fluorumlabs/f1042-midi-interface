//
// Created by Artem Godin on 14/07/16.
//

#include <stm32f0xx_hal.h>
#include <stdbool.h>
#include <app_task.h>
#include <stm32f0xx_hal_tim.h>
#include "io.h"
#include "application.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

// Internal
bool _buttonState = false;
bool _debounceButtonState = false;

int _blinkCount = 0;
int _blinkDuration = 0;
int _blinkDurationCount = 0;

uint16_t _led = 0;
uint16_t _gpio = 0;

void led_blink(enum led_e led, bool once, bool fast) {
	_led = (uint16_t) (((led & LED_RED) ? RED_Pin : 0) | ((led & LED_BLUE) ? BLUE_Pin : 0));

	_blinkCount = once ? 1 : 100;
	_blinkDuration = fast ? 1 : 5;
	_blinkDurationCount = 0;

	HAL_TIM_Base_Stop_IT(&htim1);
	HAL_GPIO_WritePin(GPIOA, _led, GPIO_PIN_SET);
	HAL_TIM_Base_Start_IT(&htim1);

}

void led_stopblink() {
	HAL_TIM_Base_Stop_IT(&htim1);
}


void led_set(enum led_e led, bool on) {
	uint16_t l = (uint16_t) (((led & LED_RED) ? RED_Pin : 0) | ((led & LED_BLUE) ? BLUE_Pin : 0));
	HAL_GPIO_WritePin(GPIOA, l, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void gpio_set(enum gpio_e led, bool on) {
	uint16_t l = (uint16_t) (((led & GPIO_OUT) ? OUT_Pin : 0));
	HAL_GPIO_WritePin(GPIOA, l, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void gpio_blink(enum gpio_e led) {
	_gpio = (uint16_t) (((led & GPIO_OUT) ? OUT_Pin : 0));

	HAL_TIM_Base_Stop_IT(&htim3);
	HAL_GPIO_WritePin(GPIOA, _gpio, GPIO_PIN_SET);
	HAL_TIM_Base_Start_IT(&htim3);

}

/*
 * Interrupt handlers
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == BOOT_Pin) {
		_debounceButtonState = (HAL_GPIO_ReadPin(BOOT_GPIO_Port, BOOT_Pin) == GPIO_PIN_SET);
		HAL_TIM_Base_Stop_IT(&htim2);
		HAL_TIM_Base_Start_IT(&htim2);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim1) {
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
	if (htim == &htim2) {
		if (_debounceButtonState != _buttonState) {
			_buttonState = _debounceButtonState;
			app_pushevent(_buttonState ? APP_BUTTON_BOOT_DOWN : APP_BUTTON_BOOT_UP);
			HAL_TIM_Base_Stop_IT(&htim2);
		}
	}
	if (htim == &htim3) {
		HAL_GPIO_WritePin(GPIOA, _gpio, GPIO_PIN_RESET);
		HAL_TIM_Base_Stop_IT(&htim3);
	}
}
