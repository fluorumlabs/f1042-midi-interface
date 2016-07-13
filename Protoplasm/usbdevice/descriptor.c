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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"

#include <usbd_core.h>

#pragma GCC diagnostic pop

#include <defines.h>

#include "descriptor.h"
#include "common.h"

extern struct usb_endpoint_s _usb_endpoints[TOTAL_ENDPOINTS][2];

extern struct usb_string_descriptor_s _usb_strings[];
extern usb_descriptor_t _usb_descriptor;

extern usb_descriptor_t _usb_configuration_descriptor;
extern uint16_t _usb_configuration_size;

usb_descriptor_t _usb_langid = {USB_LANGID()};

void desc_init() {
	unsigned int csize = _usb_configuration_size;
	uint8_t *configuration = _usb_configuration_descriptor;

	unsigned int i = 0;

	while (i < csize) {
		if (configuration[i + 1] == USB_DESC_ENDPOINT) {
			int epnum = configuration[i + 2] & USB_ENDPOINT_MASK;

			if ((configuration[i + 2] & USB_DIRECTION_MASK) == USB_DEVICE2HOST) {
				if (_usb_endpoints[epnum][USB_ENDPOINT_DEVICE2HOST].usbbuffer != null) {
					_usb_endpoints[epnum][USB_ENDPOINT_DEVICE2HOST].packetsize = toword(
						configuration[i + 4] | (configuration[i + 5] << 8));
					_usb_endpoints[epnum][USB_ENDPOINT_DEVICE2HOST].attributes =
						configuration[i + 3];
				}
			} else {
				if (_usb_endpoints[epnum][USB_ENDPOINT_HOST2DEVICE].usbbuffer != null) {
					_usb_endpoints[epnum][USB_ENDPOINT_HOST2DEVICE].packetsize = toword(
						configuration[i + 4] | (configuration[i + 5] << 8));
					_usb_endpoints[epnum][USB_ENDPOINT_HOST2DEVICE].attributes =
						configuration[i + 3];
				}
			}
		}

		i += configuration[i];
	}
}

void desc_getdescriptor(uint8_t *configuration, uint8_t type, uint8_t **data,
			uint16_t *size) {
	unsigned int csize = _usb_configuration_size;

	unsigned int i = 0;

	while (i < csize) {
		if (configuration[i + 1] == type) {
			*data = &configuration[i];
			*size = configuration[i];
			return;
		}

		i += configuration[i];
	}

	*data = null;
	*size = 0;
}


uint8_t *_desc_device(uint8_t speed, uint16_t *length) {
	unused speed;
	*length = _usb_descriptor[0];
	return (uint8_t *) &_usb_descriptor;
}

uint8_t *_desc_langid(uint8_t speed, uint16_t *length) {
	unused speed;
	*length = _usb_langid[0];
	return (uint8_t *) &_usb_langid;
}

uint8_t *_desc_string(uint8_t idx, uint16_t *length) {
	*length = _usb_strings[idx].bLength;
	return (uint8_t *) &_usb_strings[idx];
}

uint8_t *_desc_manufacturer(uint8_t speed, uint16_t *length) {
	unused speed;
	return _desc_string(USB_IDX_MANUFACTURER, length);
}

uint8_t *_desc_product(uint8_t speed, uint16_t *length) {
	unused speed;
	return _desc_string(USB_IDX_PRODUCT, length);
}

uint8_t *_desc_serial(uint8_t speed, uint16_t *length) {
	unused speed;
	return _desc_string(USB_IDX_SERIAL, length);
}

uint8_t *_desc_config(uint8_t speed, uint16_t *length) {
	unused speed;
	return _desc_string(USB_IDX_CONFIGURATION, length);
}

uint8_t *_desc_interface(uint8_t speed, uint16_t *length) {
	unused speed;
	return _desc_string(USB_IDX_INTERFACE, length);
}

USBD_DescriptorsTypeDef _usb_descriptors_callbacks = {
	_desc_device,
	_desc_langid,
	_desc_manufacturer,
	_desc_product,
	_desc_serial,
	_desc_config,
	_desc_interface,
};
