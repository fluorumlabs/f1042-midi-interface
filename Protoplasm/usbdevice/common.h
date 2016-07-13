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
/// @file				common.h
///
/// @addtogroup usb			USB API
/// @{
/// @defgroup usb_common		Common Functions
/// @{

#ifndef USBCOMMON_H
#define USBCOMMON_H

#include <defines.h>
#include <ringbuffer.h>

#include "descriptor.h"

///////////////////////////////////////////////////////////////////////////////
/// @name Constants
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Total available endpoints.

	STM32F042 supports 8 endpoints: EP0 for control and 7 general purpose EP.
*/
#define TOTAL_ENDPOINTS        8

/**
	Buffer size for commands.

	We use maximum defined size for EP0 endpoint, which is 64 bytes.
*/
#define COMMAND_BUFFERSIZE  USB_MAX_EP0_SIZE

/**
	Maximum packed size.

	USB FS defines maximum packet size of 64 bytes.
*/
#define MAX_PACKET_SIZE     USB_FS_MAX_PACKET_SIZE

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name Requests
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Return codes for interfacing with STM USB FS Core Library.
*/
enum usb_req_return_e {
    USB_REQ_FAIL = 0xffff,                ///< Request processing was unsuccessful.
    USB_REQ_OK = 0                ///< Request processing was successful.
};

/**
	bmRequestType type field.

	See section 9.3 of USB 2.0 specification.
*/
enum usb_req_type_e {
    USB_REQ_TYPE = 0b1100000,        ///< 'AND' mask for type.
    USB_REQ_STANDARD = 0b0000000,        ///< Standard type.
    USB_REQ_CLASS = 0b0100000,        ///< Class type.
    USB_REQ_VENDOR = 0b1000000,        ///< Vendor type.
    USB_REQ_RESERVED = 0b1100000        ///< Reserved.
};

/**
	bmRequestType recepient field.

	See section 9.3 of USB 2.0 specification.

	@note Codes 4 to 31 are reserved for future use.
*/
enum usb_req_recipient_e {
    USB_REQ_RECIPIENT = 0x1f,                ///< 'AND' mask for recepient.
    USB_REQ_DEVICE = 0,                        ///< Recepient is device.
    USB_REQ_INTERFACE = 1,                        ///< Recepient is interface.
    USB_REQ_ENDPOINT = 2,                        ///< Recepient is endpoint.
    USB_REQ_OTHER = 3                        ///< Other recepient.
};

/**
	USB requests.

	See section 9.3 of USB 2.0 specification.
*/
enum usb_req_code_e {
    /**
	    This request returns status for the specified recipient.
    */
	    USB_GET_STATUS = 0,

    /**
	    Clear or disable a specific feature.
    */
	    USB_CLEAR_FEATURE = 1,

    /**
	    This request is used to set or enable a specific feature.
    */
	    USB_SET_FEATURE = 3,

    /**
	    This request sets the device address for all future device
	    accesses.
    */
	    USB_SET_ADDRESS = 5,

    /**
	    This request returns the specified descriptor if the descriptor
	    exists.
    */
	    USB_GET_DESCRIPTOR = 6,

    /**
	    This request is optional and may be used to update existing
	    descriptors or new descriptors may be added.
    */
	    USB_SET_DESCRIPTOR = 7,

    /**
	    This request returns the current device configuration value.
    */
	    USB_GET_CONFIGURATION = 8,

    /**
	    This request sets the device configuration.
    */
	    USB_SET_CONFIGURATION = 9,

    /**
	    This request returns the selected alternate setting for the specified
	    interface.
    */
	    USB_GET_INTERFACE = 10,

    /**
	    This request allows the host to select an alternate setting for
	    the specified interface.
    */
	    USB_SET_INTERFACE = 11,

    /**
	    This request is used to set and then report an endpoint’s synchronization
	    frame.
    */
	    USB_SYNCH_FRAME = 12,

    /**
	    This is dummy request value used internally.
    */
	    USB_NO_COMMAND = 0xff
};

/**
	Feature selector for #USB_CLEAR_FEATURE and #USB_SET_FEATURE.

	Feature selectors are used when enabling or setting features, such as
	remote wakeup, specific to a device, interface, or endpoint.
*/
enum usb_req_feature_e {
    USB_DEVICE_REMOTE_WAKEUP = 1,        ///< Remote wake-up feature
    USB_ENDPOINT_HALT = 0,                ///< Halt feature
    USB_TEST_MODE = 2                ///< Test mode feature
};

/**
	Interface field.
*/
enum usb_req_index_e {
    USB_REQ_INTERFACEMASK = 0xf                ///< 'AND' mask for selecting interface number.
};

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name Endpoints
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Endpoint direction.
*/
enum usb_idx_endpoint_e {
    USB_ENDPOINT_HOST2DEVICE = 0,        ///< Host-to-device (EP OUT)
    USB_ENDPOINT_DEVICE2HOST = 1        ///< Device-to-host (EP IN)
};

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name Functions
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Initialize USB subsystem.
*/
void usb_init();

/**
	Flush sending queue.

	@param [in] buffer Pointer to ringbuffer, for which sending queue
		should be flushed.
*/
void usb_flush(struct ringbuffer_s *buffer);

/// @}
///////////////////////////////////////////////////////////////////////////////

#ifndef __DOXYGEN__
// Internal stuff /////////////////////////////////////////////////////////////

/*
	Internal structure representing interface.
*/
struct usb_interface_s {
    /*
	    Current alternative setting.
    */
    uint8_t altsetting;

    /*
	    Callback for initializing associated systems.
    */
    void ( *init )();

    /*
	    Callback for deinitializing associated systems.
    */
    void ( *deinit )();

    /*
	    Callback for processing of USB requests.
    */
    uint16_t ( *request )(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue,
			  uint16_t wIndex, uint16_t wLength, uint8_t *data);
};

/*
	Internal structure representing endpoint.
*/
struct usb_endpoint_s {
    /*
	    Endpoint packet size, as specified in descriptor.

	    @note Filled in run-time.
    */
    uint16_t packetsize;

    /*
	    Endpoint attributes, as specified in descriptor.

	    @note Filled in run-time.
    */
    uint8_t attributes;

    /*
	    Ring buffer for application access.
    */
    struct ringbuffer_s *appbuffer;

    /*
	    USB system buffer.
    */
    uint8_t *usbbuffer;

    void ( *callback )();
};


#endif


void usb_device_init(void);

void usb_device_reset(uint8_t speed);

void usb_device_configured(void);

void usb_device_suspended(void);

void usb_device_resumed(void);

void usb_device_connected(void);

void usb_device_disconnected(void);

#endif // USBCOMMON_H

/// @}
/// @}
///////////////////////////////////////////////////////////////////////////////
