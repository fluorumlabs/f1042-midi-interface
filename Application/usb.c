//
// Created by Artem Godin on 09/07/16.
//

#include <descriptor.h>
#include <common.h>
#include <class.h>
#include <class_audio.h>
#include <class_cdc.h>
#include <usbd_def.h>

void midi_callback();

#include <usb_meta.h> //=== USB META BEGIN ===============================

DESCRIPTOR(STMICROELECTRONICS, STM32F4xx, VERSION(1, 0, 0),
	   USB_DEV_INTERFACE_DEFINED
);

STRINGS(
[USB_IDX_MANUFACTURER]      = STRING( "FluorumLabs" ),
[USB_IDX_PRODUCT]           = STRING( "ProtoPlasm Template" ),
[USB_IDX_SERIAL]            = STRING( "12345" ),
[USB_IDX_CONFIGURATION]     = STRING( "Default Configuration" ),
[USB_IDX_INTERFACE]         = STRING( "Default Interface" )
);

CONFIGURATION(
	USB_CONFIGURATION(
		bNumInterfaces(2),
		bConfigurationValue(1),
		iConfiguration(0),
		bmAttributes(USB_CONF_SELFPOWERED),
		bMaxPower(100),

		USB_INTERFACE(
			bInterfaceNumber(0),
			bAlternateSetting(0),
			bNumEndpoints(0),
			iInterface(0),
			USB_IF_AUDIO_CONTROL),
		AUDIO_AUDIOCONTROL_HEADER_NULL(
			baInterfaceNr(1)),
		USB_INTERFACE(
			bInterfaceNumber(1),
			bAlternateSetting(0),
			bNumEndpoints(2),
			iInterface(0),
			USB_IF_AUDIO_MIDI_STREAMING),
		AUDIO_MIDISTREAM_HEADER(
			AUDIO_MIDI_IN_JACK(
				bJackType(AUDIO_MIDI_EMBEDDED),
				bJackID(1),
				iJack(0)),
			AUDIO_MIDI_IN_JACK(
				bJackType(AUDIO_MIDI_EXTERNAL),
				bJackID(2),
				iJack(0)),
			AUDIO_MIDI_OUT_JACK(
				bJackType(AUDIO_MIDI_EMBEDDED),
				bJackID(3),
				baSourceIDPin(bSourceIDPin(2, 1)),
				iJack(0)),
			AUDIO_MIDI_OUT_JACK(
				bJackType(AUDIO_MIDI_EXTERNAL),
				bJackID(4),
				baSourceIDPin(bSourceIDPin(1, 1)),
				iJack(0)),
			AUDIO_MIDISTREAM_DATA_ENDPOINT(
				bHost2DeviceEP(1),
				64),
			AUDIO_MIDISTREAM_DATA_CSENDPOINT(
				baAssocJackID(1)),
			AUDIO_MIDISTREAM_DATA_ENDPOINT(
				bDevice2HostEP(1),
				64),
			AUDIO_MIDISTREAM_DATA_CSENDPOINT(
				baAssocJackID(3))
		)

	)
);

USB_BUFFER(EP1_RX, MAX_PACKET_SIZE);
USB_BUFFER(EP1_TX, MAX_PACKET_SIZE);

APP_BUFFER(BUFFER_MIDI_RX, MAX_PACKET_SIZE);
APP_BUFFER(BUFFER_MIDI_TX, MAX_PACKET_SIZE);

ENDPOINTS([1][TX] = ENDPOINT( EP1_TX, BUFFER_MIDI_TX, null),
	  [1][RX] = ENDPOINT( EP1_RX, BUFFER_MIDI_RX, midi_callback),
);

INTERFACES([0] =        INTERFACE_AUDIO);

#include <usb_meta.h> //==================================================
#include <app_task.h>
#include "application.h"

void midi_callback() {
	uint16_t chunk_size = ringbuffer_toread(&BUFFER_MIDI_RX);
	uint16_t chunk_read = 0;
	uint8_t *chunk;
	uint32_t midi;
	uint8_t midi_offset;

	ringbuffer_getreadbuffer(&BUFFER_MIDI_RX, &chunk, &chunk_size);
	// Echo back to host
	ringbuffer_write(&BUFFER_MIDI_TX, chunk, chunk_size);
	usb_flush(&BUFFER_MIDI_TX);

	if (chunk_size != 0) {
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
	}
	ringbuffer_read(&BUFFER_MIDI_RX, 0, chunk_size);

}

void usb_device_reset(uint8_t speed) {
	application_connection_usb(true);
}

void usb_device_suspended() {
	application_connection_usb(false);
}
