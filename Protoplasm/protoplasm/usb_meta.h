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

///////////////////////////////////////////////////////////////////////////////
/// @file				ui_task_meta.h
///
/// @addtogroup hilevel			High-Level API
/// @{
/// @addtogroup hilevel_usb		USB Support
/// @{
/// @defgroup hilevel_usb_meta		META Programming
/// @{

#ifndef F1042_MIDI_INTERFACE_USB_META_H
#define F1042_MIDI_INTERFACE_USB_META_H

/**
	Version helper macro for descriptors.

	@param major Major digit of version.
	@param minor Minor digit of version.
	@param subversion Subversion.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	VERSION( 1,0,0 ) // This is equals 0x100
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define VERSION(major, minor, subversion) ((major<<8)|(minor<<4)|subversion)

/**
	Define device descriptor.

	@param idVendor Vendor Id (VID).
	@param idProduct Product Id (PID).
	@param bcdDevice Device version in BCD format.
	@param ... USB device class/subclass/protocol.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		DESCRIPTOR(
			STMICROELECTRONICS, STM32F4xx, VERSION( 1,0,0 ),
			USB_DEV_INTERFACE_DEFINED
			);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define DESCRIPTOR(idVendor, idProduct, bcdDevice, ...) \
        usb_descriptor_t _usb_descriptor = { USB_DEVICE( idVendor, idProduct, bcdDevice, __VA_ARGS__ ) }

/**
	Define strings.

	@param ... Comma separated string definitions in form of
	`[string index] = STRING( "string" )

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		STRINGS(
			[USB_IDX_MANUFACTURER]      = STRING( "FluorumLabs" ),
			[USB_IDX_PRODUCT]           = STRING( "ProtoPlasm Template" ),
			[USB_IDX_SERIAL]            = STRING( "12345" ),
			[USB_IDX_CONFIGURATION]     = STRING( "Default Configuration" ),
			[USB_IDX_INTERFACE]         = STRING( "Default Interface" )
			);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
//FIXME Hardcoded array size
#define STRINGS(...) struct usb_string_descriptor_s _usb_strings[6] = { __VA_ARGS__ }

/**
	String definition helper.

	@param ... String.

	@note To be used only within #STRINGS block.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		STRINGS(
			[USB_IDX_MANUFACTURER]      = STRING( "FluorumLabs" ),
			[USB_IDX_PRODUCT]           = STRING( "ProtoPlasm Template" ),
			[USB_IDX_SERIAL]            = STRING( "12345" ),
			[USB_IDX_CONFIGURATION]     = STRING( "Default Configuration" ),
			[USB_IDX_INTERFACE]         = STRING( "Default Interface" )
			);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define STRING(...) { USB_STRING( __VA_ARGS__ ) }

/**
	Define configuration descriptor.

	@param ... Comma separated descriptors.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
					bNumEndpoints(1),
					iInterface(0),
					USB_DEV_CDC(
						bInterfaceSubClass(CDC_IF_ACM),
						bInterfaceProtocol(CDC_IF_V250) )
					 ),
				CDC_HEADER(),
				CDC_CALLMANAGEMENT(
					bmCapabilities(CDC_CALLMANAGEMENT_ND0|CDC_CALLMANAGEMENT_ND1),
					bDataInterface(1) ),
				CDC_ACM(
					bmCapabilities(CDC_ACM_D1) ),
				CDC_UNION(
					bControlInterface(0),
					bSlaveInterfaces( 1 ) ),
				USB_ENDPOINT(
					bDevice2HostEP(2),
					bmAttributes(USB_ENDPOINT_INTERRUPT),
					wMaxPacketSize(CDC_COMMAND_PACKET),
					bInterval(CDC_COMMAND_INTERVAL) ),

				USB_INTERFACE(
					bInterfaceNumber(1),
					bAlternateSetting(0),
					bNumEndpoints(2),
					iInterface(0),
					USB_IF_CDC_DATA( CDC_IF_NOTSPECIFIED ) ),
				USB_ENDPOINT(
					bHost2DeviceEP(1),
					bmAttributes(USB_ENDPOINT_BULK),
					wMaxPacketSize(MAX_PACKET_SIZE),
					bInterval(0) ),
				USB_ENDPOINT(
					bDevice2HostEP(1),
					bmAttributes(USB_ENDPOINT_BULK),
					wMaxPacketSize(MAX_PACKET_SIZE),
					bInterval(0) )
			) );
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define CONFIGURATION(...) usb_descriptor_t _usb_configuration_descriptor =  { __VA_ARGS__ }; uint16_t _usb_configuration_size = sizeof( _usb_configuration_descriptor )

/**
	Declare USB receiver/transmitter buffer

	@param name Buffer identifier.
	@param size Buffer size.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		USB_BUFFER( EP1_RX, MAX_PACKET_SIZE );
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define USB_BUFFER(name, size) uint8_t _usb_buffer_##name[size]

/**
	Declare application receiver/transmitter buffer.

	@param name Buffer identifier.
	@param size Buffer size.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		APP_BUFFER( BUFFER_CDC_RX, 1024 );
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define APP_BUFFER(name, size) struct ringbuffer_s name = RINGBUFFER( size )

/**
	Declare endpoints.

	@param ... Comma separated list of endpoints in the following form:
	`[`endpoint index`][TX` | `RX] = ENDPOINT( ... )`

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		ENDPOINTS(
			[1][TX] = ENDPOINT( EP1_TX, BUFFER_CDC_TX, null ),
			[1][RX] = ENDPOINT( EP1_RX, BUFFER_CDC_RX, cdc_callback )
			);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define ENDPOINTS(...) struct usb_endpoint_s _usb_endpoints[TOTAL_ENDPOINTS][2] = { __VA_ARGS__ }

/**
	Endpoint is a receive endpoint (host-to-device, EP OUT).
*/
#define RX USB_ENDPOINT_HOST2DEVICE

/**
	Endpoint is a transmit endpoint (device-to-host, EP IN).
*/
#define TX USB_ENDPOINT_DEVICE2HOST

/**
	Endpoint definition.

	@param usb_buffer USB buffer identifier, created with #USB_BUFFER.
	@param app_buffer Application buffer identifier, created with #APP_BUFFER.
	@param rxcallback Data processing callback identifier, created with #CALLBACK_DECLARE.

	@note To be used only within #ENDPOINTS block.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		ENDPOINTS(
			[1][TX] = ENDPOINT( EP1_TX, BUFFER_CDC_TX, null ),
			[1][RX] = ENDPOINT( EP1_RX, BUFFER_CDC_RX, cdc_callback )
			);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define ENDPOINT(usb_buffer, app_buffer, callback) { 0,0,&app_buffer, _usb_buffer_##usb_buffer, callback }

/**
	Declare interfaces.

	@param ... Comma separated interface definitions in form `[interface index] = Interface definition`.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		INTERFACES(
			[0] = INTERFACE_CDC
			);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define INTERFACES(...) struct usb_interface_s _usb_interfaces[] = { __VA_ARGS__ }; unsigned int _usb_interfaces_num = sizeof( _usb_interfaces )/sizeof( struct usb_interface_s )

/**
	Define custom interface.

	@param init_callback Callback for initialization of associated systems.
	@param deinit_callback Callback for deinitialization of associated systems.
	@param request_callback Callback for preocessing of USB requests.

	@note To be used only within #INTERFACES block.
	@note Standard classes usually provide predefined macroses for interfaces,
	like #INTERFACE_CDC for CDC Class.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		INTERFACES(
			[0] = INTERFACE( usb_cdc_initcallback, usb_cdc_deinitcallback, usb_cdc_cmdcallback )
			);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define INTERFACE(init_callback, deinit_callback, request_callback) { 0, init_callback, deinit_callback, request_callback }

/**
	Data processing callbacks registration.

	@param ... Comma separated registration entries.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		CALLBACKS(
			REGISTER( cdc_callback, tskIDLE_PRIORITY )
			);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define CALLBACKS(...) struct usb_callback_task_s _usb_callbacks[] = { __VA_ARGS__, { 0, 0, null } }

/**
	Callback registration definition.

	@param name Callback identifier, mentioned previously with #CALLBACK_DECLARE.
	@param priority FreeRTOS task priority for this callback.

	@note To be used only within #CALLBACKS block.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		CALLBACKS(
			REGISTER( cdc_callback, tskIDLE_PRIORITY )
			);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define REGISTER(name, priority) { #name, priority, task_##name }

#else  // F1042_MIDI_INTERFACE_USB_META_H_H

#undef VERSION
#undef DESCRIPTOR
#undef STRINGS
#undef STRING

#undef CONFIGURATION

#undef USB_BUFFER
#undef APP_BUFFER

#undef ENDPOINTS
#undef ENDPOINT
#undef RX
#undef TX

#undef INTERFACES

#undef INTERFACE

#undef REGISTER
#undef CALLBACKS

#undef F1042_MIDI_INTERFACE_USB_META_H

#endif // F1042_MIDI_INTERFACE_USB_META_H

/// @}
/// @}
/// @}
///////////////////////////////////////////////////////////////////////////////
