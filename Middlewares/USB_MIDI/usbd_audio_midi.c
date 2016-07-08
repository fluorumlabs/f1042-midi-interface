/**
  ******************************************************************************
  * @file    usbd_audio.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   This file provides the Audio core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                AUDIO Class  Description
  *          ===================================================================
 *           This driver manages the Audio Class 1.0 following the "USB Device Class Definition for
  *           Audio Devices V1.0 Mar 18, 98".
  *           This driver implements the following aspects of the specification:
  *             - Device descriptor management
  *             - Configuration descriptor management
  *             - Standard AC Interface Descriptor management
  *             - 1 Audio Streaming Interface (with single channel, PCM, Stereo mode)
  *             - 1 Audio Streaming Endpoint
  *             - 1 Audio Terminal Input (1 channel)
  *             - Audio Class-Specific AC Interfaces
  *             - Audio Class-Specific AS Interfaces
  *             - AudioControl Requests: only SET_CUR and GET_CUR requests are supported (for Mute)
  *             - Audio Feature Unit (limited to Mute control)
  *             - Audio Synchronization type: Asynchronous
  *             - Single fixed audio sampling rate (configurable in usbd_conf.h file)
  *          The current audio class version supports the following audio features:
  *             - Pulse Coded Modulation (PCM) format
  *             - sampling rate: 48KHz. 
  *             - Bit resolution: 16
  *             - Number of channels: 2
  *             - No volume control
  *             - Mute/Unmute capability
  *             - Asynchronous Endpoints 
  *          
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_audio_midi.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"
#include <stdbool.h>
#include <application.h>


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_AUDIO 
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_AUDIO_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_AUDIO_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_AUDIO_Private_FunctionPrototypes
  * @{
  */


static uint8_t USBD_AUDIO_Init(USBD_HandleTypeDef *pdev,
			       uint8_t cfgidx);

static uint8_t USBD_AUDIO_DeInit(USBD_HandleTypeDef *pdev,
				 uint8_t cfgidx);

static uint8_t USBD_AUDIO_Setup(USBD_HandleTypeDef *pdev,
				USBD_SetupReqTypedef *req);

static uint8_t *USBD_AUDIO_GetCfgDesc(uint16_t *length);

static uint8_t *USBD_AUDIO_GetDeviceQualifierDesc(uint16_t *length);

static uint8_t USBD_AUDIO_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_AUDIO_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_AUDIO_EP0_RxReady(USBD_HandleTypeDef *pdev);

static uint8_t USBD_AUDIO_EP0_TxReady(USBD_HandleTypeDef *pdev);

static uint8_t USBD_AUDIO_SOF(USBD_HandleTypeDef *pdev);

static uint8_t USBD_AUDIO_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_AUDIO_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);

static void AUDIO_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static void AUDIO_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

/**
  * @}
  */

/** @defgroup USBD_AUDIO_Private_Variables
  * @{
  */

USBD_ClassTypeDef USBD_AUDIO =
	{
		USBD_AUDIO_Init,
		USBD_AUDIO_DeInit,
		USBD_AUDIO_Setup,
		USBD_AUDIO_EP0_TxReady,
		USBD_AUDIO_EP0_RxReady,
		USBD_AUDIO_DataIn,
		USBD_AUDIO_DataOut,
		USBD_AUDIO_SOF,
		USBD_AUDIO_IsoINIncomplete,
		USBD_AUDIO_IsoOutIncomplete,
		USBD_AUDIO_GetCfgDesc,
		USBD_AUDIO_GetCfgDesc,
		USBD_AUDIO_GetCfgDesc,
		USBD_AUDIO_GetDeviceQualifierDesc,
	};

/* USB AUDIO device Configuration Descriptor */
#define MY_USB_AUDIO_CONFIG_DESC_SIZ                     101
#define MIDI_OUT_EP                                      0x01
#define MIDI_IN_EP                                      0x82

__ALIGN_BEGIN static uint8_t USBD_AUDIO_CfgDesc[MY_USB_AUDIO_CONFIG_DESC_SIZ] __ALIGN_END =
	{
		/* Configuration 1 */
		0x09,                                 /* bLength */
		USB_DESC_TYPE_CONFIGURATION,          /* bDescriptorType */
		LOBYTE(MY_USB_AUDIO_CONFIG_DESC_SIZ),    /* wTotalLength  101 bytes*/
		HIBYTE(MY_USB_AUDIO_CONFIG_DESC_SIZ),
		0x02,                                 /* bNumInterfaces */
		0x01,                                 /* bConfigurationValue */
		0x00,                                 /* iConfiguration */
		0xC0,                                 /* bmAttributes  BUS Powred*/
		0x32,                                 /* bMaxPower = 100 mA*/
		/* 09 byte*/

		/* USB Audio Control interface descriptor */
		AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
		USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
		0x00,                                 /* bInterfaceNumber */
		0x00,                                 /* bAlternateSetting */
		0x00,                                 /* bNumEndpoints */
		USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
		AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
		AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
		0x00,                                 /* iInterface */
		/* 09 byte*/

		/*	 MIDI Adapter Class-specific AC Interface Descriptor */
		AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,                        /* CS_INTERFACE.	*/
		AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
		0x00,          /* 1.00 */             /* bcdADC */
		0x01,
		0x09,                                 /* wTotalLength = 9*/
		0x00,
		0x01,                                 /* bInCollection */
		0x01,                                 /* baInterfaceNr */
		/* 09 byte*/

		AUDIO_OUTPUT_TERMINAL_DESC_SIZE,      /* bLength */
		USB_DESC_TYPE_INTERFACE,                          /* bDescriptorType */
		0x01,                                 /* Interface Number */
		0x00,                                 /* bAlternateSetting */
		0x02,                                 /* bNumEndpoints */
		USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
		0x03,                                                                                                                                        /* bSubclass - MIDI Streaming */
		0x00,                                                                                              /* bInterfaceProtocol */
		0x00,                                 /* iInterface */
		/* 9 byte*/

		/* MIDI Adapter Class-specific MS Interface Descriptor   */
		AUDIO_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,                        /* CS_INTERFACE.	*/
		AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
		0x00,          /* 1.00 */             /* bcdADC */
		0x01,
		0x41,                                 /* wTotalLength = 65*/
		0x00,
		/* 7 byte*/

		/* MIDI Adapter MIDI IN Jack Descriptor (Embedded) */
		0x06,                                                                                                                                  /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,                        /* CS_INTERFACE.	*/
		AUDIO_CONTROL_INPUT_TERMINAL,                                        /* bDescriptorSubtype:  MIDI_IN_JACK subtype. */
		0x01,                                                                                                                                        /* bJackType: EMBEDDED. */
		0x01,                                                                                                                                        /* bJackID */
		0x00,                                                                                                                                        /* iJack: Unused. */
		/* 6 byte*/

		/* MIDI Adapter MIDI IN Jack Descriptor (External) */
		0x06,                                                                                                                                  /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,                        /* CS_INTERFACE.	*/
		AUDIO_CONTROL_INPUT_TERMINAL,                                        /* bDescriptorSubtype:  MIDI_IN_JACK subtype. */
		0x02,                                                                                                                                        /* bJackType: EXTERNAL. */
		0x02,                                                                                                                                        /* bJackID */
		0x00,                                                                                                                                        /* iJack: Unused. */
		/* 6 byte*/

		/* MIDI Adapter MIDI OUT Jack Descriptor (Embedded) */
		0x09,                                                                                                                                  /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,                        /* CS_INTERFACE.	*/
		AUDIO_CONTROL_OUTPUT_TERMINAL,                                /* bDescriptorSubtype:  MIDI_OUT_JACK subtype. */
		0x01,                                                                                                                                        /* bJackType: EMBEDDED. */
		0x03,                                                                                                                                        /* bJackID */
		0x01,                                                                                                                                        /* bNrInputPins: Number of Input Pins of this Jack. */
		0x02,                                                                                                                                        /* BaSourceID 1 0x02 ID of the Entity to which this Pin is connected. */
		0x01,                                                                                                                                        /* BaSourcePin 1 Output Pin number of the Entity to which this Input Pin is connected */
		0x00,                                                                                                                                        /* iJack: Unused. */
		/* 9 byte*/

/* MIDI Adapter MIDI OUT Jack Descriptor (External) */
		0x09,                                                                                                                                  /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,                        /* CS_INTERFACE.	*/
		AUDIO_CONTROL_OUTPUT_TERMINAL,                                /* bDescriptorSubtype:  MIDI_OUT_JACK subtype. */
		0x02,                                                                                                                                        /* bJackType: EXTERNAL. */
		0x04,                                                                                                                                        /* bJackID */
		0x01,                                                                                                                                        /* bNrInputPins: Number of Input Pins of this Jack. */
		0x01,                                                                                                                                        /* BaSourceID 0x01 ID of the Entity to which this Pin is connected. */
		0x01,                                                                                                                                        /* BaSourcePin Output Pin number of the Entity to which this Input Pin is connected */
		0x00,                                                                                                                                        /* iJack: Unused. */
		/* 9 byte*/


/* Endpoint 0x01 OUT - Standard Descriptor */
		AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
		USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
		MIDI_OUT_EP,                         /* bEndpointAddress 1 out endpoint*/
		USBD_EP_TYPE_BULK,                    /* bmAttributes */
		0x40,                                                                                                                        /* wMaxPacketSize */
		0x00,
		0x00,                                 /* bInterval */
		0x00,                                 /* bRefresh */
		0x00,                                 /* bSynchAddress */
		/* 09 byte*/

		/* Endpoint - OUT MIDI Streaming Descriptor*/

		5,   /* bLength */
		AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
		AUDIO_ENDPOINT_GENERAL,               /* bDescriptorSubType */
		0x01,                                 /* bNumEmbMIDIJack: Number of embedded MIDI IN Jacks. */
		0x01,                                 /* BaAssocJackID: ID of the Embedded MIDI IN Jack	*/
		/* 05 byte*/

		/* Endpoint 0x82 IN - Standard Descriptor */
		AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
		USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
		MIDI_IN_EP,                         /* bEndpointAddress 1 out endpoint*/
		USBD_EP_TYPE_BULK,                    /* bmAttributes */
		64,                                                                                                                        /* wMaxPacketSize */
		0x00,
		0x00,                                 /* bInterval */
		0x00,                                 /* bRefresh */
		0x00,                                 /* bSynchAddress */
		/* 09 byte*/

		/* Endpoint - IN MIDI Streaming Descriptor*/

		5,   /* bLength */
		AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
		AUDIO_ENDPOINT_GENERAL,               /* bDescriptorSubType */
		0x01,                                 /* bNumEmbMIDIJack: Number of embedded MIDI IN Jacks. */
		0x03,                                 /* BaAssocJackID: ID of the Embedded MIDI IN Jack	*/
		/* 05 byte*/
	};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_AUDIO_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
	{
		USB_LEN_DEV_QUALIFIER_DESC,
		USB_DESC_TYPE_DEVICE_QUALIFIER,
		0x00,
		0x02,
		0x00,
		0x00,
		0x00,
		0x40,
		0x01,
		0x00,
	};

volatile bool midiBufferBusy = false;

void sendMidi(USBD_HandleTypeDef *pdev, uint8_t *buf, size_t len) {
	if (pdev->dev_state != USBD_STATE_CONFIGURED) {
		midiBufferBusy = false;
	} else if (!midiBufferBusy) {
		midiBufferBusy = true;
		USBD_LL_Transmit(pdev, MIDI_IN_EP, buf, len);
	}
}


/**
  * @}
  */

static struct ringbuffer_s _midi_in_buffer = RINGBUFFER(MIDI_BUFFER_SIZE);
static struct ringbuffer_s _midi_out_buffer = RINGBUFFER(MIDI_BUFFER_SIZE);
static uint16_t _midi_out_tx_size = 0;

// Static buffer to receive MIDI messages
static USBD_AUDIO_HandleTypeDef _usbd_audio_handleTypeDef = {
	alt_setting: 0,
	control: {0, {[USB_MAX_EP0_SIZE - 1]=0}, 0, 0}
};

/** @defgroup USBD_AUDIO_Private_Functions
  * @{
  */

USBD_HandleTypeDef *_usbd_pdev = 0;

/**
  * @brief  USBD_AUDIO_Init
  *         Initialize the AUDIO interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_AUDIO_Init(USBD_HandleTypeDef *pdev,
			       uint8_t cfgidx) {
	_usbd_pdev = pdev;

	/* Open EP OUT */
	USBD_LL_OpenEP(pdev,
		       MIDI_OUT_EP,
		       USBD_EP_TYPE_BULK,
		       64);

	/* Open EP IN */
	USBD_LL_OpenEP(pdev,
		       MIDI_IN_EP,
		       USBD_EP_TYPE_BULK,
		       64);

	/* Allocate Audio structure */
	pdev->pClassData = &_usbd_audio_handleTypeDef;

	uint16_t len = MIDI_PACKET_SIZE;
	uint8_t *buffer;

	ringbuffer_getwritebuffer(&_midi_in_buffer, &buffer, &len);

	/* Prepare Out endpoint to receive 1st packet */
	USBD_LL_PrepareReceive(pdev,
			       MIDI_OUT_EP,
			       buffer,
			       len);

	return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_Init
  *         DeInitialize the AUDIO layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_AUDIO_DeInit(USBD_HandleTypeDef *pdev,
				 uint8_t cfgidx) {

	/* Open EP OUT */
	USBD_LL_CloseEP(pdev,
			MIDI_OUT_EP);

	/* Open EP IN */
	USBD_LL_CloseEP(pdev,
			MIDI_IN_EP);

	/* DeInit  physical Interface components */
	if (pdev->pClassData != NULL) {
		pdev->pClassData = NULL;
	}

	return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_Setup
  *         Handle the AUDIO specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_AUDIO_Setup(USBD_HandleTypeDef *pdev,
				USBD_SetupReqTypedef *req) {
	uint16_t len;
	uint8_t *pbuf;
	uint8_t ret = USBD_OK;

	switch (req->bmRequest & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_CLASS :
		switch (req->bRequest) {
		case AUDIO_REQ_GET_CUR:
			AUDIO_REQ_GetCurrent(pdev, req);
			break;

		case AUDIO_REQ_SET_CUR:
			AUDIO_REQ_SetCurrent(pdev, req);
			break;

		default:
			USBD_CtlError(pdev, req);
			ret = USBD_FAIL;
		}
		break;

	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_GET_DESCRIPTOR:
			if ((req->wValue >> 8) == AUDIO_DESCRIPTOR_TYPE) {
				pbuf = USBD_AUDIO_CfgDesc + 18;
				len = (uint16_t) MIN(USB_AUDIO_DESC_SIZ, req->wLength);


				USBD_CtlSendData(pdev,
						 pbuf,
						 len);
			}
			break;

		case USB_REQ_GET_INTERFACE :
			USBD_CtlSendData(pdev,
					 (uint8_t *) &(_usbd_audio_handleTypeDef.alt_setting),
					 1);
			break;

		case USB_REQ_SET_INTERFACE :
			if ((uint8_t) (req->wValue) <= USBD_MAX_NUM_INTERFACES) {
				_usbd_audio_handleTypeDef.alt_setting = (uint8_t) (req->wValue);
			}
			else {
				/* Call the error management function (command will be nacked */
				USBD_CtlError(pdev, req);
			}
			break;

		default:
			USBD_CtlError(pdev, req);
			ret = USBD_FAIL;
		}
	}
	return ret;
}


/**
  * @brief  USBD_AUDIO_GetCfgDesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_AUDIO_GetCfgDesc(uint16_t *length) {
	*length = sizeof(USBD_AUDIO_CfgDesc);
	return USBD_AUDIO_CfgDesc;
}

/**
  * @brief  USBD_AUDIO_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIO_DataIn(USBD_HandleTypeDef *pdev,
				 uint8_t epnum) {
	if (_midi_out_tx_size > 0) {
		ringbuffer_read(&_midi_out_buffer, 0, _midi_out_tx_size);
		_midi_out_tx_size = 0;
	}

	// This is called after data is sent
	uint16_t len = MIDI_PACKET_SIZE;
	uint8_t *buffer;

	ringbuffer_getreadbuffer(&_midi_out_buffer, &buffer, &len);
	if (len > 0) {
		// Wait, there is more!
		_midi_out_tx_size = len;
		USBD_LL_Transmit(pdev, MIDI_IN_EP, buffer, len);
	}

	/* Only OUT data are processed */
	return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_AUDIO_EP0_RxReady(USBD_HandleTypeDef *pdev) {
	return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_AUDIO_EP0_TxReady(USBD_HandleTypeDef *pdev) {
	/* Only OUT control data are processed */
	return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_AUDIO_SOF(USBD_HandleTypeDef *pdev) {
	return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIO_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum) {

	return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIO_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum) {

	return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIO_DataOut(USBD_HandleTypeDef *pdev,
				  uint8_t epnum) {
	uint16_t len = (uint16_t) USBD_LL_GetRxDataSize(pdev, epnum);
	if (len > 0) {
		// What? How could it be 0?
		ringbuffer_write(&_midi_in_buffer, 0, len);
		USBD_MIDI_ReceivedCallback(&_midi_in_buffer);
	}

	len = MIDI_PACKET_SIZE;
	uint8_t *buffer;

	ringbuffer_getwritebuffer(&_midi_in_buffer, &buffer, &len);
	USBD_LL_PrepareReceive(pdev,
			       MIDI_OUT_EP,
			       buffer,
			       len);
	return USBD_OK;
}

/**
  * @brief  AUDIO_Req_GetCurrent
  *         Handles the GET_CUR Audio control request.
  * @param  pdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	memset(_usbd_audio_handleTypeDef.control.data, 0, USB_MAX_EP0_SIZE);
	/* Send the current mute state */
	USBD_CtlSendData(pdev,
			 _usbd_audio_handleTypeDef.control.data,
			 req->wLength);
}

/**
  * @brief  AUDIO_Req_SetCurrent
  *         Handles the SET_CUR Audio control request.
  * @param  pdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	if (req->wLength) {
		/* Prepare the reception of the buffer over EP0 */
		USBD_CtlPrepareRx(pdev,
				  _usbd_audio_handleTypeDef.control.data,
				  req->wLength);

		_usbd_audio_handleTypeDef.control.cmd = AUDIO_REQ_SET_CUR;     /* Set the request value */
		_usbd_audio_handleTypeDef.control.len = (uint8_t) req->wLength;          /* Set the request data length */
		_usbd_audio_handleTypeDef.control.unit = HIBYTE(req->wIndex);  /* Set the request target unit */
	}
}


/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t *USBD_AUDIO_GetDeviceQualifierDesc(uint16_t *length) {
	*length = sizeof(USBD_AUDIO_DeviceQualifierDesc);
	return USBD_AUDIO_DeviceQualifierDesc;
}

void USBD_MIDI_Transmit(uint8_t *data, uint16_t len) {
	bool unfinishedXfer = (ringbuffer_toread(&_midi_out_buffer) > 0);
	ringbuffer_write(&_midi_out_buffer, data, len);
	if (!unfinishedXfer) {
		uint16_t len = MIDI_PACKET_SIZE;
		uint8_t *buffer;

		ringbuffer_getreadbuffer(&_midi_out_buffer, &buffer, &len);
		if (len > 0) {
			// Wait, there is more!
			_midi_out_tx_size = len;
			USBD_LL_Transmit(_usbd_pdev, MIDI_IN_EP, buffer, len);
		}

	}
}

/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
