/*
    ProtoPlasm: DIY digital pro-audio platform
    Copyright (C) 2013  Artem Godin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see [http://www.gnu.org/licenses/].
*/

// We don't want to change standard library, aren't we?
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"

#include <usbd_ioreq.h>

#pragma GCC diagnostic pop

#include <defines.h>
#include <ringbuffer.h>
#include <util.h>
#include <application.h>

#include "common.h"

extern struct usb_endpoint_s _usb_endpoints[TOTAL_ENDPOINTS][2];
extern struct usb_interface_s _usb_interfaces[];
extern unsigned int _usb_interfaces_num;

extern struct usb_string_descriptor_s _usb_strings[];
extern usb_descriptor_t _usb_descriptor;

extern usb_descriptor_t _usb_configuration_descriptor;
extern uint16_t _usb_configuration_size;

#define _USB_SEND_ON_FRAME_NUM 5

enum _usb_endpoint_inprocess_enum {
    _USB_ENDPOINT_SENDING = 0b10000000,
    _USB_ENDPOINT_IDLE = 0b01111111
};

uint8_t _usb_class_init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
	unused cfgidx;

	//FIXME Skip EP0
	for (unsigned int epnum = 0; epnum < TOTAL_ENDPOINTS; epnum++) {
		if (_usb_endpoints[epnum][USB_ENDPOINT_DEVICE2HOST].packetsize != 0)
			USBD_LL_OpenEP(pdev, tobyte(epnum | USB_DEVICE2HOST),
				       _usb_endpoints[epnum][USB_ENDPOINT_DEVICE2HOST].attributes,
				       _usb_endpoints[epnum][USB_ENDPOINT_DEVICE2HOST].packetsize);

		if (_usb_endpoints[epnum][USB_ENDPOINT_HOST2DEVICE].packetsize != 0)
			USBD_LL_OpenEP(pdev, tobyte(epnum | USB_HOST2DEVICE),
				       _usb_endpoints[epnum][USB_ENDPOINT_HOST2DEVICE].attributes,
				       _usb_endpoints[epnum][USB_ENDPOINT_HOST2DEVICE].packetsize);
	}

	for (unsigned int ifnum = 0; ifnum < _usb_interfaces_num; ifnum++) {
		invoke(_usb_interfaces[ifnum].init)();
	}

	//FIXME Skip EP0
	for (unsigned int epnum = 0; epnum < TOTAL_ENDPOINTS; epnum++) {
		if (_usb_endpoints[epnum][USB_ENDPOINT_HOST2DEVICE].packetsize != 0) {
			USBD_LL_PrepareReceive(pdev, tobyte(epnum | USB_HOST2DEVICE),
					       _usb_endpoints[epnum][USB_ENDPOINT_HOST2DEVICE].usbbuffer,
					       _usb_endpoints[epnum][USB_ENDPOINT_HOST2DEVICE].packetsize);
		}
	}

	return USBD_OK;
}

uint8_t _usb_class_deinit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
	unused cfgidx;

	for (unsigned int epnum = 0; epnum < TOTAL_ENDPOINTS; epnum++) {
		if (_usb_endpoints[epnum][USB_ENDPOINT_DEVICE2HOST].packetsize != 0)
			USBD_LL_CloseEP(pdev, tobyte(epnum | USB_DEVICE2HOST));

		if (_usb_endpoints[epnum][USB_ENDPOINT_HOST2DEVICE].packetsize != 0)
			USBD_LL_CloseEP(pdev, tobyte(epnum | USB_HOST2DEVICE));
	}

	for (unsigned int ifnum = 0; ifnum < _usb_interfaces_num; ifnum++) {
		invoke(_usb_interfaces[ifnum].deinit)();
	}

	return USBD_OK;
}

uint8_t _usb_cmd_buffer[COMMAND_BUFFERSIZE];

uint8_t _usb_cmd_bRequest = USB_NO_COMMAND;
uint8_t _usb_cmd_bmRequest = 0;
uint16_t _usb_cmd_wIndex = 0;
uint16_t _usb_cmd_wValue = 0;
uint16_t _usb_cmd_wLength = 0;

uint8_t _usb_class_setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	uint16_t len;
	uint8_t *pbuf;

	switch (req->bmRequest & USB_REQ_TYPE) {
		// Class Requests
	case USB_REQ_CLASS :
		if ((req->bmRequest & USB_REQ_RECIPIENT) != USB_REQ_INTERFACE) {
			USBD_CtlError(pdev, req);
			return USBD_FAIL;
		}

		// Check if the request is a data setup packet
		if (req->wLength) {
			// Check if the request is Device-to-Host
			if (req->bmRequest & USB_DEVICE2HOST) {
				// Get the data to be sent to Host from interface layer
				unsigned int iface = req->wIndex & USB_REQ_INTERFACEMASK;

				if (_usb_interfaces[iface].request != null) {
					uint16_t size = _usb_interfaces[iface].request(req->bmRequest, req->bRequest,
										       req->wValue, req->wIndex,
										       req->wLength, _usb_cmd_buffer);

					if (size == USB_REQ_FAIL) {
						USBD_CtlError(pdev, req);
						return USBD_FAIL;
					}

					// Send the data to the host
					if (size > 0)
						USBD_CtlSendData(pdev, _usb_cmd_buffer, size);
				} else {
					USBD_CtlError(pdev, req);
					return USBD_FAIL;
				}
			} else { // Host-to-Device requeset
				// Set the value of the current command to be processed
				_usb_cmd_bmRequest = req->bmRequest;
				_usb_cmd_bRequest = req->bRequest;
				_usb_cmd_wLength = req->wLength;
				_usb_cmd_wIndex = req->wIndex;
				_usb_cmd_wValue = req->wValue;

				// Prepare the reception of the buffer over EP0
				// Next step: the received data will be managed in usbd_cdc_EP0_TxSent()
				// function.
				USBD_CtlPrepareRx(pdev, _usb_cmd_buffer, req->wLength);
			}
		} else { // No Data request
			// Transfer the command to the interface layer
			unsigned int iface = req->wIndex & USB_REQ_INTERFACEMASK;

			if (_usb_interfaces[iface].request != null) {
				uint16_t ret = _usb_interfaces[iface].request(req->bmRequest, req->bRequest,
									      req->wValue, req->wIndex, 0, null);

				if (ret == USB_REQ_FAIL) {
					USBD_CtlError(pdev, req);
					return USBD_FAIL;
				}
			}
		}

		return USBD_OK;

	default:
		USBD_CtlError(pdev, req);
		return USBD_FAIL;



		// Standard Requests
	case USB_REQ_STANDARD:
		switch (req->bRequest) {
		case USB_GET_DESCRIPTOR:
			desc_getdescriptor(_usb_configuration_descriptor, tobyte(req->wValue >> 8),
					   &pbuf, &len);

			if (len > 0) {
				len = min(len, req->wLength);
				USBD_CtlSendData(pdev, pbuf, len);
			} else {
				USBD_CtlError(pdev, req);
			}

			break;

		case USB_GET_INTERFACE :

			USBD_CtlSendData(pdev, (uint8_t *) &
				(_usb_interfaces[req->wIndex & USB_REQ_INTERFACEMASK].altsetting), 1);

			break;

		case USB_SET_INTERFACE :
			_usb_interfaces[req->wIndex & USB_REQ_INTERFACEMASK].altsetting = tobyte(
				req->wValue);
			break;

		default:
			break;
		}

		break;
	}

	return USBD_OK;

}


uint8_t _usb_class_EP0_txsent(USBD_HandleTypeDef *pdev) {
	// Not implemented yet
	return USBD_OK;
}

uint8_t _usb_class_EP0_rxready(USBD_HandleTypeDef *pdev) {
	if (_usb_cmd_bRequest != USB_NO_COMMAND) {
		invoke(_usb_interfaces[_usb_cmd_wIndex & USB_REQ_INTERFACEMASK].request)(
				_usb_cmd_bmRequest, _usb_cmd_bRequest, _usb_cmd_wValue, _usb_cmd_wIndex,
				_usb_cmd_wLength, _usb_cmd_buffer);
		_usb_cmd_bRequest = USB_NO_COMMAND;
	}

	return USBD_OK;
}

void _usb_start_transfer(USBD_HandleTypeDef *pdev, unsigned int epnum, bool start) {
	struct usb_endpoint_s *ep = &_usb_endpoints[epnum & USB_ENDPOINT_MASK][USB_ENDPOINT_DEVICE2HOST];

	if ((ep->packetsize > 0)
	    && (((ep->attributes & _USB_ENDPOINT_SENDING) == 0) == start)) {
		unsigned int len;

		len = ringbuffer_read(ep->appbuffer, ep->usbbuffer,
				      ep->packetsize);

		if (len > 0) {
			ep->attributes |= _USB_ENDPOINT_SENDING;
			USBD_LL_Transmit(pdev, tobyte(epnum | USB_DEVICE2HOST), ep->usbbuffer, len);
		} else {
			ep->attributes &= _USB_ENDPOINT_IDLE;
		}
	}
}

uint8_t _usb_class_datain(USBD_HandleTypeDef *pdev, uint8_t epnum) {
	_usb_start_transfer(pdev, epnum, false);

	return USBD_OK;
}

uint8_t _usb_class_dataout(USBD_HandleTypeDef *pdev, uint8_t epnum) {

	struct usb_endpoint_s *ep =
		&_usb_endpoints[epnum & USB_ENDPOINT_MASK][USB_ENDPOINT_HOST2DEVICE];
	unsigned int len = USBD_LL_GetRxDataSize(pdev, epnum & USB_ENDPOINT_MASK);

	ringbuffer_write(ep->appbuffer, ep->usbbuffer, len);

	USBD_LL_PrepareReceive(pdev, epnum | USB_HOST2DEVICE, ep->usbbuffer,
			       ep->packetsize);

	invoke(ep->callback)(ep->appbuffer);

	return USBD_OK;
}

uint8_t _usb_class_sof(USBD_HandleTypeDef *pdev) {
	// Check the data to be sent through IN pipe
	for (unsigned int epnum = 0; epnum < TOTAL_ENDPOINTS; epnum++) {
		_usb_start_transfer(pdev, epnum, true);
	}

	return USBD_OK;

}

uint8_t _usb_class_isoin_incomplete(USBD_HandleTypeDef *pdev, uint8_t epnum) {
	// Not implemented yet
	return USBD_OK;
}

uint8_t _usb_class_isoout_incomplete(USBD_HandleTypeDef *pdev, uint8_t epnum) {
	// Not implemented yet
	return USBD_OK;
}

uint8_t *_usb_class_getconfiguration(uint16_t *length) {
	*length = _usb_configuration_size;
	return _usb_configuration_descriptor;
}

uint8_t *_usb_class_getdevicequalifierdescriptor(uint16_t *length) {
	// Not implemented yet
	*length = 0;
	return (uint8_t *) null;
}

__weak void usb_device_init_callback(void) { }

__weak void usb_device_reset_callback(void) { }

__weak void usb_device_configured_callback(void) { }

__weak void usb_device_suspended_callback(void) { }

__weak void usb_device_resumed_callback(void) { }

__weak void usb_device_connected_callback(void) { }

__weak void usb_device_disconnected_callback(void) { }

USBD_ClassTypeDef _usb_class_callbacks = {
	_usb_class_init,
	_usb_class_deinit,
	_usb_class_setup,
	_usb_class_EP0_txsent,
	_usb_class_EP0_rxready,
	_usb_class_datain,
	_usb_class_dataout,
	_usb_class_sof,
	_usb_class_isoin_incomplete,
	_usb_class_isoout_incomplete,
	_usb_class_getconfiguration,
	_usb_class_getconfiguration,
	_usb_class_getconfiguration,
	_usb_class_getdevicequalifierdescriptor
};


USBD_HandleTypeDef _usb_devicehandle;
extern USBD_DescriptorsTypeDef _usb_descriptors_callbacks;

#define DEVICE_FS                0

void usb_init() {
	desc_init();
	USBD_Init(&_usb_devicehandle, &_usb_descriptors_callbacks, DEVICE_FS);
	USBD_RegisterClass(&_usb_devicehandle, &_usb_class_callbacks);
	USBD_Start(&_usb_devicehandle);
}

void usb_flush(struct ringbuffer_s *buffer) {
	assert(buffer != null);

	for (unsigned int ep = 0; ep < TOTAL_ENDPOINTS; ep++) {
		if (_usb_endpoints[ep][USB_ENDPOINT_DEVICE2HOST].appbuffer == buffer) {
			_usb_start_transfer(&_usb_devicehandle, ep, true);
			return;
		}
	}
}
