//
// Created by Artem Godin on 09/07/16.
//

#include <descriptor.h>
#include <common.h>
#include <class.h>
#include <class_audio.h>
#include <class_cdc.h>
#include <usbd_def.h>

void midi_callback(struct ringbuffer_s *appbuffer);

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

#define APP_BUFFER_SIZE 256

USB_BUFFER(EP1_RX, MAX_PACKET_SIZE);
USB_BUFFER(EP1_TX, MAX_PACKET_SIZE);

APP_BUFFER(BUFFER_MIDI_RX, APP_BUFFER_SIZE);
APP_BUFFER(BUFFER_MIDI_TX, APP_BUFFER_SIZE);

ENDPOINTS([1][TX] = ENDPOINT( EP1_TX, BUFFER_MIDI_TX, null),
	  [1][RX] = ENDPOINT( EP1_RX, BUFFER_MIDI_RX, midi_callback),
);

INTERFACES([0] =        INTERFACE_AUDIO);

#include <usb_meta.h> //==================================================
#include <app_task.h>
#include "application.h"

