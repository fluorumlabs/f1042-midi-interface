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

#include "class_audio.h"
#include "common.h"
#include <util.h>

void usb_audio_initcallback() {
}


void usb_audio_deinitcallback() {
}

uint16_t usb_audio_cmdcallback(uint8_t bmRequestType, uint8_t bRequest,
			       uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t *data) {
	return USB_REQ_OK;
}
