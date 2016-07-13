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
/// @file				descriptor.h
///
/// @addtogroup usb			USB API
/// @{
/// @defgroup usb_descriptor		Descriptor Functions
/// @{

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <stdint.h>
#include <defines.h>

/**
	USB vendor Id (VID).
*/
enum vid_e {
    STMICROELECTRONICS = 0x0483        ///< ST Microelectronics
};

/**
	USB product Id (PID).
*/
enum pid_e {
    STM32F4xx = 0x5740                ///< STM32F4xx family
};

/**
	LangId for string descriptors.
*/
enum langid_e {
    US_ENGLISH = 0x0409                ///< US english.
};

/**
	Standard USB version.
*/
enum usb_versions_e {
    USB_VERSION_20 = 0x0200        ///< USB 2.0
};

/**
	String descriptor indexes.
*/
enum usb_string_e {
    USB_IDX_NONE = 0,        ///< No corresponding string
    USB_IDX_LANGID = 0,        ///< LangId
    USB_IDX_MANUFACTURER = 1,        ///< Manufacturer
    USB_IDX_PRODUCT = 2,        ///< Product
    USB_IDX_SERIAL = 3,        ///< Serial Number
    USB_IDX_CONFIGURATION = 4,        ///< Configuration
    USB_IDX_INTERFACE = 5,                ///< Interface
};

/**
	Standard USB descriptor types.
*/
enum usb_descriptor_e {
    USB_DESC_DEVICE = 0x01, ///< Device descriptor
    USB_DESC_CONFIGURATION = 0x02, ///< Configuration descriptor
    USB_DESC_STRING = 0x03, ///< String descriptor
    USB_DESC_INTERFACE = 0x04, ///< Interface descriptor
    USB_DESC_ENDPOINT = 0x05, ///< Endpoint descriptor
    USB_DESC_DEVICEQUALIFIER = 0x06, ///< Device qualifier descriptor
    USB_DESC_OTHER = 0x07, ///< Other type
    USB_DESC_INTERFACEPOWER = 0x08, ///< Interface power descriptor
    USB_DESC_INTERFACEASSOCIATION = 0x0B, ///< Interface association descriptor
    USB_DESC_CSUNDEFINED = 0x20, ///< Class specific undefined descriptor
    USB_DESC_CSDEVICE = 0x21, ///< Class specific device descriptor
    USB_DESC_CSCONFIGURATION = 0x22, ///< Class specific configuration descriptor
    USB_DESC_CSSTRING = 0x23, ///< Class specific string descriptor
    USB_DESC_CSINTERFACE = 0x24, ///< Class specific interface descriptor
    USB_DESC_CSENDPOINT = 0x25  ///< Class specific endpoint descriptor
};

/**
	USB descriptor type.
*/
typedef uint8_t usb_descriptor_t[];

/**
	wTotalLength helper macro

	@param this Size of descriptor with wTotalLength field in it.
	@param ... Following descriptors
*/
#define wTotalLength(this, ...)                                        \
        field16( this + sizeof( (uint8_t[]){ __VA_ARGS__ } ) )

/**
	bLength helper macro

	@param ... Data
*/
#define bLength(...) sizeof( (uint8_t[]){ __VA_ARGS__ } )

/**
	Array helper macro

	@param ... values
*/
#define bArray(...) __VA_ARGS__

///////////////////////////////////////////////////////////////////////////////
/// @name Descriptor Fields
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/// Descriptor type
///
#define bDescriptorType(x) x

/// Descriptor subtype
///
#define bDescriptorSubType(x) x

/// Vendor ID
///
#define idVendor(x) x

/// Product ID
///
#define idProduct(x) x

/// Device version
///
#define bcdDevice(x) x

/// Device class
///
#define bDeviceClass(x) x

/// Device subclass
///
#define bDeviceSubClass(x) x

/// Device protocol
///
#define bDeviceProtocol(x) x

/// Number of interfaces
///
#define bNumInterfaces(x) x

/// Configuration index
///
#define bConfigurationValue(x) x

/// Index of configuration string descriptor
///
#define iConfiguration(x) x

/// Attributes
///
#define bmAttributes(x) x

/// Max Power in mW
///
#define bMaxPower(x) x

/// Interface index
///
#define bInterfaceNumber(x) x

/// Alternate setting
///
#define bAlternateSetting(x) x

/// Number of endpoints
///
#define bNumEndpoints(x) x

/// Interface class
///
#define bInterfaceClass(x) x

/// Interface subclass
///
#define bInterfaceSubClass(x) x

/// Interface protocol
///
#define bInterfaceProtocol(x) x

/// Index of interface string descriptor
///
#define iInterface(x) x

/// Index of function string descriptor
///
#define iFunction(x) x

/// Endpoint address
///
#define bEndpointAddress(x) x

/// Endpoint address (device2host)
///
#define bDevice2HostEP(x) x | USB_DEVICE2HOST

/// Endpoint address (host2device)
///
#define bHost2DeviceEP(x) x | USB_HOST2DEVICE

/// Maximum packed size
///
#define wMaxPacketSize(x) x

/// Interval
///
#define bInterval(x) x

/// First interface in IAD.
///
#define bFirstInterface(x) x

/// Total number of interfaces in IAD.
///
#define bInterfaceCount(x) x

/// @}
///////////////////////////////////////////////////////////////////////////////


/**
	Define descriptor header.

	@param bLength Length of descriptor including header.
	@param bDescriptorType Descriptor type, one of #usb_descriptor_e values.
*/
#define USB_DESC_HEADER(bLength, bDescriptorType) \
        bLength, bDescriptorType

/**
	Define subdescriptor header.

	@param bLength Length of descriptor including header.
	@param bDescriptorType Descriptor type, one of #usb_descriptor_e values.
	@param bDescriptorSubType Descriptor subtype.
*/
#define USB_SUBDESC_HEADER(bLength, bDescriptorType, bDescriptorSubType) \
        bLength, bDescriptorType, bDescriptorSubType

/**
	Define device descriptor.

	@param idVendor Vendor Id.
	@param idProduct Product Id.
	@param bcdDevice Product version in BCD format.
	@param bDeviceClass Device class.
	@param bDeviceSubClass Device subclass.
	@param bDeviceProtocol Device protocol.
*/
#define USB_DEVICE(idVendor, idProduct, bcdDevice, bDeviceClass, bDeviceSubClass, bDeviceProtocol) \
        USB_DESC_HEADER( 18, USB_DESC_DEVICE ),                                        \
        field16( USB_VERSION_20 ),                                                \
        bDeviceClass, bDeviceSubClass, bDeviceProtocol,                                \
        USB_MAX_EP0_SIZE,                                                        \
        field16( idVendor ),                                                        \
        field16( idProduct ),                                                        \
        field16( bcdDevice ),                                                        \
        USB_IDX_MANUFACTURER,                                                        \
        USB_IDX_PRODUCT,                                                        \
        USB_IDX_SERIAL,                                                                \
        1 /* Currently we support only one configuration */

/**
	Define device qualifier (for High-Speed only).

	@param bDeviceClass Device class.
	@param bDeviceSubClass Device subclass.
	@param bDeviceProtocol Device protocol.

	@note ProtoPlasm is not supporting USB HS Mode.
*/
#define USB_DEVICE_QUALIFIER(bDeviceClass, bDeviceSubClass, bDeviceProtocol)        \
        USB_DESC_HEADER( 10, USB_DESC_DEVICEQUALIFIER ),                        \
        field16( USB_VERSION_20 ),                                                \
        bDeviceClass, bDeviceSubClass, bDeviceProtocol,                                \
        USB_MAX_EP0_SIZE,                                                        \
        1, /* Currently we support only one configuration */                        \
        0  /* RESERVED */

/**
	Configuration attribute values.
*/
enum usb_conf_attribute_e {
    USB_CONF_SELFPOWERED = 0b11000000,        ///< Device is self-powered.
    USB_CONF_REMOTEWKUP = 0b10100000        ///< Device supports remote wake-up.
};

/**
	Define configuration descriptor.

	@param bNumInterfaces Total number of interfaces in this configuration.
	@param bConfigurationValue Configuration index.
	@param iConfiguration String descriptor index for this configuration.
	@param bmAttributes Attributes, combination of #usb_conf_attribute_e values.
	@param bMaxPower Maximum power the device can consume (in mW, 0 to 500).
	@param ... Descriptors, defining this configuration
*/
#define USB_CONFIGURATION(bNumInterfaces, bConfigurationValue, iConfiguration, bmAttributes, bMaxPower /* mW */, ...) \
        USB_DESC_HEADER( 9, USB_DESC_CONFIGURATION ),                        \
        wTotalLength( 9, __VA_ARGS__ ),                                        \
        bNumInterfaces,                                                        \
        bConfigurationValue,                                                \
        iConfiguration,                                                        \
        bmAttributes,                                                        \
        (bMaxPower)>>1, /* Step 2 mW */                                        \
        __VA_ARGS__

/**
	Define 'other speed' configuration descriptor  (for High-Speed only)

	@see USB_CONFIGURATION

	@note ProtoPlasm is not supporting USB HS Mode.
*/
#define USB_OTHER_SPEED_CONFIGURATION(...)            \
        USB_CONFIGURATION( __VA_ARGS__ )

/**
	Interface association descriptor

	@param bFirstInterface Interface number of the first interface.
	@param bInterfaceCount Number of contiguous interfaces.
	@param iFunction Index of string descriptor for this function.
	@param ... Interface class/subclass/protocol.
*/
#define USB_IAD(bFirstInterface, bInterfaceCount, iFunction, ...)                        \
        USB_DESC_HEADER( 8, USB_DESC_INTERFACEASSOCIATION ),                        \
        bFirstInterface,                                                        \
        bInterfaceCount,                                                        \
        __VA_ARGS__,                                                                \
        iFunction

/**
	Define interface descriptor.

	@param bInterfaceNumber Interface index.
	@param bAlternateSetting `0` for normal interface, `1` for alternate.
	@param bNumEndpoints Total endpoints number for this interface.
	@param iInterface Index of string descriptor for this interface.
	@param ... Interface class/subclass/protocol.
*/
#define USB_INTERFACE(bInterfaceNumber, bAlternateSetting, bNumEndpoints, iInterface, ...) \
        USB_DESC_HEADER( 9, USB_DESC_INTERFACE ),           \
        bInterfaceNumber, bAlternateSetting,                \
        bNumEndpoints,                                      \
        __VA_ARGS__, \
        iInterface

/**
	Endpoint index values.
*/
enum usb_endpoint_address_direction_e {
    USB_DEVICE2HOST = 0b10000000,                ///< Device-to-host endpoint (EP IN)
    USB_HOST2DEVICE = 0b00000000,                ///< Host-to-device endpoint (EP OUT)
    USB_DIRECTION_MASK = USB_DEVICE2HOST,        ///< 'AND' mask for selecting direction.
    USB_ENDPOINT_MASK = 0b00001111                ///< 'AND' mask for selecting endpoint index.
};

/**
	Endpoint attribute transfer field.
*/
enum usb_endpoint_attributes_transfer_e {
    USB_ENDPOINT_CONTROL = 0b00,        ///< Control endpoint.
    USB_ENDPOINT_ISOCHRONOUS = 0b01, ///< Iso-chronous endpoint.
    USB_ENDPOINT_BULK = 0b10,        ///< Bulk endpoint.
    USB_ENDPOINT_INTERRUPT = 0b11,        ///< Interrupt endpoint.
    USB_ENDPOINT_TRANSFER = 0b11        ///< 'AND' mask for transfer field.
};

/**
	Iso-chronous endpoint attribute synchronisation field.
*/
enum usb_endpoint_attributes_synchronisation_e {
    USB_ENDPOINT_NOSYNC = 0b0000,        ///< No synchronisation.
    USB_ENDPOINT_ASYNC = 0b0100,        ///< Asynchronous mode.
    USB_ENDPOINT_ADAPTIVE = 0b1000,        ///< Adaptive mode.
    USB_ENDPOINT_SYNC = 0b1100,        ///< Synchronous mode.
    USB_ENDPOINT_SYNCHRONIZATION = 0b1100        ///< 'AND' mask for synchronisation field.
};

/**
	Iso-chronous endpoint attribute usage field.
*/
enum usb_endpoint_attributes_usage_e {
    USB_ENDPOINT_DATA = 0b000000,        ///< Data endpoint.
    USB_ENDPOINT_FEEDBACK = 0b010000,        ///< Feedback endpoint.
    USB_ENDPOINT_IMPLICIT = 0b100000,        ///< Implicit data feedback endpoint.
    USB_ENDPOINT_RESERVED = 0b110000,        ///< Reserved.
    USB_ENDPOINT_USAGE = 0b110000        ///< 'AND' mask for usage field.
};

/**
	Additional transaction opportunities per microframe for high-speed
	iso-chronous and interrupt endpoints.
*/
enum usb_endpoint_maxpacket_additional_e {
    USB_ENDPOINT_ADD0 = 0b0000000000000000,        ///< None.
    USB_ENDPOINT_ADD1 = 0b0000100000000000,        ///< +1 additional transaction.
    USB_ENDPOINT_ADD2 = 0b0001000000000000,        ///< +2 additional transactions.
    USB_ENDPOINT_ADDRESERVED = 0b0001100000000000        ///< Reserved.
};

/**
	Define endpoint descriptor.

	@param bEndpointAddress Endpoint address, combination of endpoint index and
	#usb_endpoint_address_direction_e value.
	@param bmAttributes Endpoint attributes, combination of
	#usb_endpoint_attributes_transfer_e, #usb_endpoint_attributes_synchronisation_e
	and #usb_endpoint_attributes_usage_e.
	@param wMaxPacketSize Maximum packet size (+ additional transaction
	opportunities if #usb_endpoint_maxpacket_additional_e specified.
	@param bInterval Interval for polling endpoint for data transfers.
*/
#define USB_ENDPOINT(bEndpointAddress, bmAttributes, wMaxPacketSize, bInterval) \
        USB_DESC_HEADER( 7, USB_DESC_ENDPOINT ),                                \
        bEndpointAddress,                                                        \
        bmAttributes,                                                                \
        field16( wMaxPacketSize ),                                                \
        bInterval

/**
	Define standard LangId string descriptor.

	@see US_ENGLISH
*/
#define USB_LANGID()                                                                \
        USB_DESC_HEADER( 4, USB_DESC_STRING ),                                        \
        field16( US_ENGLISH )

/**
	Define string descriptor.

	@param bString String.
*/
#define USB_STRING(bString)                           \
        USB_DESC_HEADER( 2+sizeof( L""bString ), USB_DESC_STRING ), \
        L""bString

/**
	Maximum string descriptor length
*/
#define USB_STRING_DESCRIPTOR_LEN 32

/**
	String descriptor structure.
*/
#ifdef __DOXYGEN__
struct usb_string_descriptor_s  {
	uint8_t bLength;		///< Descriptor length.
	uint8_t bDescriptorType;	///< Descriptor type (#USB_DESC_STRING).
	wchar_t bString[USB_STRING_DESCRIPTOR_LEN]; ///< UTF-16 String.
};
#else
struct usb_string_descriptor_s {
    uint8_t bLength;                ///< Descriptor length.
    uint8_t bDescriptorType;        ///< Descriptor type (#USB_DESC_STRING).
    wchar_t bString[USB_STRING_DESCRIPTOR_LEN]; ///< UTF-16 String.
} packed;
#endif

/**
	Initialize descriptors.
*/
void desc_init();

/**
	Get descriptor of specified type from specified configuration.

	@param [in] configuration Pointer to configuration descriptor.
	@param [in] type Descriptor type.
	@param [out] data Pointer to variable that will receive pointer to descriptor.
	@param [out] size Size of descriptor.
*/
void desc_getdescriptor(uint8_t *configuration, uint8_t type, uint8_t **data,
			uint16_t *size);

// Internal stuff

#endif // DESCRIPTOR_H

/// @}
/// @}
///////////////////////////////////////////////////////////////////////////////
