/**
  ******************************************************************************
  * @file           : usbd_conf.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the board support package for the USB device library
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include <application.h>
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_audio_midi.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
void HAL_PCD_MspInit(PCD_HandleTypeDef *pcdHandle) {
	if (pcdHandle->Instance == USB) {
		/* USER CODE BEGIN USB_MspInit 0 */

		/* USER CODE END USB_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USB_CLK_ENABLE();

		/* Peripheral interrupt init */
		HAL_NVIC_SetPriority(USB_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USB_IRQn);
		/* USER CODE BEGIN USB_MspInit 1 */

		/* USER CODE END USB_MspInit 1 */
	}
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef *pcdHandle) {
	if (pcdHandle->Instance == USB) {
		/* USER CODE BEGIN USB_MspDeInit 0 */

		/* USER CODE END USB_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USB_CLK_DISABLE();

		/* Peripheral interrupt Deinit*/
		HAL_NVIC_DisableIRQ(USB_IRQn);

		/* USER CODE BEGIN USB_MspDeInit 1 */

		/* USER CODE END USB_MspDeInit 1 */
	}
}

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* USER CODE BEGIN 1 */
static void SystemClockConfig_Resume(void);

/* USER CODE END 1 */
void HAL_PCDEx_SetConnectionState(PCD_HandleTypeDef *hpcd, uint8_t state);

extern void SystemClock_Config(void);

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/
/* MSP Init */

/**
  * @brief  Setup Stage callback
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) {
	led_set(LED_RED, true);
	USBD_LL_SetupStage((USBD_HandleTypeDef *) hpcd->pData, (uint8_t *) hpcd->Setup);
	led_set(LED_RED, false);
}

/**
  * @brief  Data Out Stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	led_set(LED_RED, true);
	USBD_LL_DataOutStage((USBD_HandleTypeDef *) hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
	led_set(LED_RED, false);
}

/**
  * @brief  Data In Stage callback..
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	led_set(LED_RED, true);
	USBD_LL_DataInStage((USBD_HandleTypeDef *) hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
	led_set(LED_RED, false);
}

/**
  * @brief  SOF callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) {
	led_set(LED_RED, true);
	USBD_LL_SOF((USBD_HandleTypeDef *) hpcd->pData);
	led_set(LED_RED, false);
}

/**
  * @brief  Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) {
	led_set(LED_RED, true);
	USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

	/*Set USB Current Speed*/
	switch (hpcd->Init.speed) {
	case PCD_SPEED_FULL:
		speed = USBD_SPEED_FULL;
		break;

	default:
		speed = USBD_SPEED_FULL;
		break;
	}
	USBD_LL_SetSpeed((USBD_HandleTypeDef *) hpcd->pData, speed);

	/*Reset Device*/
	USBD_LL_Reset((USBD_HandleTypeDef *) hpcd->pData);

	application_connection_usb(true);
	led_set(LED_RED, false);
}

/**
  * @brief  Suspend callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd) {
	led_set(LED_RED, true);
	application_connection_usb(false);

	/* Inform USB library that core enters in suspend Mode */
	USBD_LL_Suspend((USBD_HandleTypeDef *) hpcd->pData);
	/*Enter in STOP mode */
	/* USER CODE BEGIN 2 */
	if (hpcd->Init.low_power_enable) {
		/* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register */
		SCB->SCR |= (uint32_t) ((uint32_t) (SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
	}
	/* USER CODE END 2 */
	led_set(LED_RED, false);
}

/**
  * @brief  Resume callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd) {
	led_set(LED_RED, true);
	/* USER CODE BEGIN 3 */
	if (hpcd->Init.low_power_enable) {
		/* Reset SLEEPDEEP bit of Cortex System Control Register */
		SCB->SCR &= (uint32_t) ~((uint32_t) (SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
		SystemClockConfig_Resume();
	}
	/* USER CODE END 3 */
	USBD_LL_Resume((USBD_HandleTypeDef *) hpcd->pData);
	led_set(LED_RED, false);

}

/**
  * @brief  ISOOUTIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	led_set(LED_RED, true);
	USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef *) hpcd->pData, epnum);
	led_set(LED_RED, false);
}

/**
  * @brief  ISOINIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	led_set(LED_RED, true);
	USBD_LL_IsoINIncomplete((USBD_HandleTypeDef *) hpcd->pData, epnum);
	led_set(LED_RED, false);
}

/**
  * @brief  ConnectCallback callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd) {
	USBD_LL_DevConnected((USBD_HandleTypeDef *) hpcd->pData);
}

/**
  * @brief  Disconnect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd) {
	USBD_LL_DevDisconnected((USBD_HandleTypeDef *) hpcd->pData);
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/
/**
  * @brief  Initializes the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev) {
	/* Init USB_IP */
	/* Link The driver to the stack */
	hpcd_USB_FS.pData = pdev;
	pdev->pData = &hpcd_USB_FS;

	hpcd_USB_FS.Instance = USB;
	hpcd_USB_FS.Init.dev_endpoints = 5;
	hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
	hpcd_USB_FS.Init.ep0_mps = DEP0CTL_MPS_8;
	hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
	hpcd_USB_FS.Init.low_power_enable = DISABLE;
	hpcd_USB_FS.Init.lpm_enable = DISABLE;
	HAL_PCD_Init(&hpcd_USB_FS);

	HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *) pdev->pData, 0x00, PCD_SNG_BUF, 0x18);
	HAL_PCDEx_PMAConfig((PCD_HandleTypeDef *) pdev->pData, 0x80, PCD_SNG_BUF, 0x58);
	return USBD_OK;
}

/**
  * @brief  De-Initializes the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev) {
	HAL_PCD_DeInit((PCD_HandleTypeDef *) pdev->pData);
	return USBD_OK;
}

/**
  * @brief  Starts the Low Level portion of the Device driver. 
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev) {
	HAL_PCD_Start((PCD_HandleTypeDef *) pdev->pData);
	return USBD_OK;
}

/**
  * @brief  Stops the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev) {
	HAL_PCD_Stop((PCD_HandleTypeDef *) pdev->pData);
	return USBD_OK;
}

/**
  * @brief  Opens an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  ep_type: Endpoint Type
  * @param  ep_mps: Endpoint Max Packet Size
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev,
				  uint8_t ep_addr,
				  uint8_t ep_type,
				  uint16_t ep_mps) {
	HAL_PCD_EP_Open((PCD_HandleTypeDef *) pdev->pData,
			ep_addr,
			ep_mps,
			ep_type);

	return USBD_OK;
}

/**
  * @brief  Closes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_Close((PCD_HandleTypeDef *) pdev->pData, ep_addr);
	return USBD_OK;
}

/**
  * @brief  Flushes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_Flush((PCD_HandleTypeDef *) pdev->pData, ep_addr);
	return USBD_OK;
}

/**
  * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_SetStall((PCD_HandleTypeDef *) pdev->pData, ep_addr);
	return USBD_OK;
}

/**
  * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_ClrStall((PCD_HandleTypeDef *) pdev->pData, ep_addr);
	return USBD_OK;
}

/**
  * @brief  Returns Stall condition.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval Stall (1: Yes, 0: No)
  */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *) pdev->pData;

	if ((ep_addr & 0x80) == 0x80) {
		return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
	}
	else {
		return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
	}
}

/**
  * @brief  Assigns a USB address to the device.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr) {
	HAL_PCD_SetAddress((PCD_HandleTypeDef *) pdev->pData, dev_addr);
	return USBD_OK;
}

/**
  * @brief  Transmits data over an endpoint.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  pbuf: Pointer to data to be sent
  * @param  size: Data size    
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev,
				    uint8_t ep_addr,
				    uint8_t *pbuf,
				    uint16_t size) {
	HAL_PCD_EP_Transmit((PCD_HandleTypeDef *) pdev->pData, ep_addr, pbuf, size);
	return USBD_OK;
}

/**
  * @brief  Prepares an endpoint for reception.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  pbuf: Pointer to data to be received
  * @param  size: Data size
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev,
					  uint8_t ep_addr,
					  uint8_t *pbuf,
					  uint16_t size) {
	HAL_PCD_EP_Receive((PCD_HandleTypeDef *) pdev->pData, ep_addr, pbuf, size);
	return USBD_OK;
}

/**
  * @brief  Returns the last transfered packet size.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval Recived Data Size
  */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef *) pdev->pData, ep_addr);
}

/**
  * @brief  Delays routine for the USB Device Library.
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBD_LL_Delay(uint32_t Delay) {
	HAL_Delay(Delay);
}

/**
  * @brief  static single allocation.
  * @param  size: size of allocated memory
  * @retval None
  */
void *USBD_static_malloc(uint32_t size) {
	//static uint8_t mem[sizeof(USBD_AUDIO_HandleTypeDef)];
	/* USER CODE BEGIN 4 */
	/**
	* To compute the request size you must use the formula:
	  AUDIO_OUT_PACKET = (USBD_AUDIO_FREQ * 2 * 2) /1000)
	  AUDIO_TOTAL_BUF_SIZE = AUDIO_OUT_PACKET * AUDIO_OUT_PACKET_NUM with
	      Number of sub-packets in the audio transfer buffer. You can modify this value but always make sure
	  that it is an even number and higher than 3
	      AUDIO_OUT_PACKET_NUM = 80
	*/
	static uint8_t mem[512];
	/* USER CODE END 4 */
	return mem;
}

/**
  * @brief  Dummy memory free
  * @param  *p pointer to allocated  memory address
  * @retval None
  */
void USBD_static_free(void *p) {

}

/* USER CODE BEGIN 5 */
/**
  * @brief  Configures system clock after wake-up from USB Resume CallBack: 
  *         enable HSI, PLL and select PLL as system clock source.
  * @param  None
  * @retval None
  */
static void SystemClockConfig_Resume(void) {
	SystemClock_Config();
}
/* USER CODE END 5 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
