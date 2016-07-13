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
/// @file				class_cdc.h
///
/// @addtogroup usb			USB API
/// @{
/// @defgroup usb_cdc			CDC Class
/// @{
///	Communication Device Class.
///
///	Example USB configuration:
///	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///		USB_CONFIGURATION(
///			bNumInterfaces(2),
///			bConfigurationValue(1),
///			iConfiguration(0),
///			bmAttributes(USB_CONF_SELFPOWERED),
///			bMaxPower(100),
///
///			USB_INTERFACE(
///				bInterfaceNumber(0),
///				bAlternateSetting(0),
///				bNumEndpoints(1),
///				iInterface(0),
///				USB_DEV_CDC(
///					bInterfaceSubClass(CDC_IF_ACM),
///					bInterfaceProtocol(CDC_IF_V250) )
///				 ),
///			CDC_HEADER(),
///			CDC_CALLMANAGEMENT(
///				bmCapabilities(CDC_CALLMANAGEMENT_ND0|CDC_CALLMANAGEMENT_ND1),
///				bDataInterface(1) ),
///			CDC_ACM(
///				bmCapabilities(CDC_ACM_D1) ),
///			CDC_UNION(
///				bControlInterface(0),
///				bSlaveInterfaces( 1 ) ),
///			USB_ENDPOINT(
///				bDevice2HostEP(2),
///				bmAttributes(USB_ENDPOINT_INTERRUPT),
///				wMaxPacketSize(CDC_COMMAND_PACKET),
///				bInterval(CDC_COMMAND_INTERVAL) ),
///
///			USB_INTERFACE(
///				bInterfaceNumber(1),
///				bAlternateSetting(0),
///				bNumEndpoints(2),
///				iInterface(0),
///				USB_IF_CDC_DATA( CDC_IF_NOTSPECIFIED ) ),
///			USB_ENDPOINT(
///				bHost2DeviceEP(1),
///				bmAttributes(USB_ENDPOINT_BULK),
///				wMaxPacketSize(MAX_PACKET_SIZE),
///				bInterval(0) ),
///			USB_ENDPOINT(
///				bDevice2HostEP(1),
///				bmAttributes(USB_ENDPOINT_BULK),
///				wMaxPacketSize(MAX_PACKET_SIZE),
///				bInterval(0) )
///		)
///	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///

#ifndef CLASS_CDC_H
#define CLASS_CDC_H

#include <defines.h>
#include <ringbuffer.h>

#include "descriptor.h"
#include "class.h"

/**
	USB CDC Version
*/
enum cdc_versions_e {
	CDC_VERSION = 0x0120,			///< CDC 1.20.
	CDC_WHCM_VERSION = 0x0110		///< WHCM 1.10.
};

/**
	CDC device class/subclass/protocol.
*/
#define DEVICE_CDC USB_DEV_CDC( 0x00, 0x00 )


/**
	CDC interface subclasses.
*/
enum cdc_sub_e {
	CDC_IF_DIRECTLINE = 0x01,		///< Direct Line Control Model.
	CDC_IF_ACM = 0x02,			///< Abstract Control Model.
	CDC_IF_TELEPHONE = 0x03,		///< Telephone Control Model.
	CDC_IF_MULTICHANNEL = 0x04,		///< Multi-Channel Control Model.
	CDC_IF_CAPI = 0x05,			///< CAPI Control Model.
	CDC_IF_ETHERNET_NETWORKING = 0x06,	///< Ethernet Networking Control Model.
	CDC_IF_ATM_NETWORKING = 0x07,		///< ATM Networking Control Model.
	CDC_IF_WIRELESS_HANDSET = 0x08,		///< Wireless Handset Control Model.
	CDC_IF_DEVICE_MANAGEMENT = 0x09,	///< Device Management.
	CDC_IF_MOBILE_DIRECT_LINE = 0x0a,	///< Mobile Direct Line Model.
	CDC_IF_OBEX = 0x0b,			///< OBEX.
	CDC_IF_ETHERNET_EMULATION = 0x0c,	///< Ethernet Emulation Model.
	CDC_IF_NCM = 0x0d			///< Network Control Model.
};

/**
	CDC interface protocols.
*/
enum cdc_proto_e {
	CDC_IF_NOTSPECIFIED = 0x00,		///< Not specified.
	CDC_IF_V250 = 0x01,			///< AT Commands: V.250 etc.
	CDC_IF_PCCA101 = 0x02,			///< AT Commands defined by PCCA-101.
	CDC_IF_PCCA101_O = 0x03,		///< AT Commands defined by PCCA-101 & Annex O.
	CDC_IF_GSM7_07 = 0x04,			///< AT Commands defined by GSM 07.07.
	CDC_IF_3GPP27_7 = 0x05,			///< AT Commands defined by 3GPP 27.007.
	CDC_IF_C_S0017_0 = 0x06,		///< AT Commands defined by TIA for CDMA.
	CDC_IF_USB_EEM = 0x07,			///< Ethernet Emulation Model.
	CDC_IF_EXTERNAL = 0xfe,			///< External.
	CDC_IF_VENDORSPECIFIC = 0xff		///< Vendor Specific.
};

/**
	CDC data interface protocol.
*/
enum cdc_data_proto_e {
	CDC_IF_USBNCM10 = 0x01,		///< *Network Transfer Block.
	CDC_IF_I430 = 0x30,		///< Physical interface protocol for ISDN BRI.
	CDC_IF_ISO3309_1993 = 0x31,	///< HDLC.
	CDC_IF_TRANSPARENT = 0x32,		///< Transparent.
	CDC_IF_Q921M = 0x50,		///< Management protocol for Q.921 data link protocol.
	CDC_IF_Q921 = 0x51,		///< Data link protocol for Q.931.
	CDC_IF_Q921TM = 0x52,		///< TEI-multiplexor for Q.921 data link protocol.
	CDC_IF_V42BIS = 0x90,		///< Data compression procedures.
	CDC_IF_Q931 = 0x91,		///< Euro-ISDN protocol control.
	CDC_IF_V120 = 0x92,		///< V.24 rate adaptation to ISDN.
	CDC_IF_CAPI20 = 0x93,		///< CAPI Commands.

	/**
		Host based driver.

		@note This protocol code should only be used in messages
		between host and device to identify the host driver portion of
		a protocol stack.
	*/
	CDC_IF_HOSTBASED = 0xfd
};

/**
	CDC descriptor subtypes.
*/
enum cdc_subtype_e {
	/**
		Header Functional Descriptor, which marks the beginning of the
		concatenated set of functional descriptors for the interface.
	*/
	CDC_SUBTYPE_HEADER = 0x00,
	CDC_SUBTYPE_CALL_MANAGEMENT = 0x01,	///< Call Management Functional Descriptor.
	CDC_SUBTYPE_ACM = 0x02,			///< Abstract Control Management Functional Descriptor.
	CDC_SUBTYPE_DIRECT_LINE_MANAGEMENT = 0x03, ///< Direct Line Management Functional Descriptor.
	CDC_SUBTYPE_TELEPHONE_RINGER = 0x04,	///< Telephone Ringer Functional Descriptor.
	CDC_SUBTYPE_TELEPHONE_CALL = 0x05,	///< Telephone Call and Line State Reporting Capabilities Functional Descriptor.
	CDC_SUBTYPE_UNION = 0x06,		///< Union Functional Descriptor
	CDC_SUBTYPE_COUNTRY_SELECTION = 0x07,	///< Country Selection Functional Descriptor
	CDC_SUBTYPE_TELEPHONE_MODES = 0x08,	///< Telephone Operational Modes Functional Descriptor
	CDC_SUBTYPE_USB_TERMINAL = 0x09,	///< USB Terminal Functional Descriptor
	CDC_SUBTYPE_NETWORK_CHANNEL_TERMINAL = 0x0a, ///< Network Channel Terminal Descriptor
	CDC_SUBTYPE_PROTOCOL_UNIT = 0x0b,	///< Protocol Unit Functional Descriptor
	CDC_SUBTYPE_EXTENSION_UNIT = 0x0c,	///< Extension Unit Functional Descriptor
	CDC_SUBTYPE_MULTICHANNEL_MANAGEMENT = 0x0d, ///< Multi-Channel Management Functional Descriptor
	CDC_SUBTYPE_CAPI_CONTROL_MANAGEMENT = 0x0e, ///< CAPI Control Management Functional Descriptor
	CDC_SUBTYPE_ETHERNET_NETWORKING = 0x0f,	///< Ethernet Networking Functional Descriptor
	CDC_SUBTYPE_ATM_NETWORKING = 0x10,	///< Ethernet Networking Functional Descriptor
	CDC_SUBTYPE_WIRELESS_HANDSET_CONTROL_MODEL = 0x11, ///< Wireless Handset Control Model Functional Descriptor
	CDC_SUBTYPE_MDLM = 0x12,		///< Mobile Direct Line Model Functional Descriptor
	CDC_SUBTYPE_MDLM_DETAIL = 0x13,		///< MDLM Detail Functional Descriptor
	CDC_SUBTYPE_DEVICE_MANAGEMENT = 0x14,	///< Device Management Model Functional Descriptor
	CDC_SUBTYPE_OBEX = 0x15,		///< OBEX Functional Descriptor
	CDC_SUBTYPE_COMMAND_SET = 0x16,		///< Command Set Functional Descriptor
	CDC_SUBTYPE_COMMAND_SET_DETAIL = 0x17,	///< Command Set Detail Functional Descriptor
	CDC_SUBTYPE_TELEPHONE_CONTROL_MODEL = 0x18, ///< Telephone Control Model Functional Descriptor
	CDC_SUBTYPE_OBEX_SERVICE_IDENTIFIER = 0x19, ///< OBEX Service Identifier Functional Descriptor
	CDC_SUBTYPE_NCM = 0x1a			///< *NCM Functional Descriptor
};

///////////////////////////////////////////////////////////////////////////////
/// @name Descriptor Fields
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/// Index of control interface
///
#define bControlInterface( x ) x

/// Slave interfaces
///
#define bSlaveInterfaces( ... ) __VA_ARGS__

/// Capabilities
///
#define bmCapabilities( x ) x

/// Index of data interface
///
#define bDataInterface( x ) x

/// @}
///////////////////////////////////////////////////////////////////////////////


/**
	Define CDC header descriptor.
*/
#define CDC_HEADER()                                    \
	USB_SUBDESC_HEADER( 5, USB_DESC_CSINTERFACE, CDC_SUBTYPE_HEADER ), \
	lobyte( CDC_VERSION ), hibyte( CDC_VERSION )

/**
	Define CDC union descriptor.

	@param bControlInterface Index of master interface.
	@param bSlaveInterfaces Indices of subordinates.
*/
#define CDC_UNION( bControlInterface, bSlaveInterfaces ) \
	USB_SUBDESC_HEADER( 4+bLength(bSlaveInterfaces), USB_DESC_CSINTERFACE, CDC_SUBTYPE_UNION ), \
	bControlInterface,                                  \
	bSlaveInterfaces

/**
	Define CDC WHCM descriptor.
*/
#define CDC_WHCM()								\
	USB_SUBDESC_HEADER( 5, USB_DESC_CSINTERFACE, CDC_SUBTYPE_WIRELESS_HANDSET_CONTROL_MODEL ), \
	field16( CDC_WHCM_VERSION )

/**
	CDC ACM capabilities.
*/
enum cdc_acm_capabilities_e {
	/**
		Function supports management elements GetCommFeature,
		SetCommFeature and ClearCommFeature
	*/
	CDC_ACM_D0 = 0b00000001,

	/**
		Function supports the management elements GetLineCoding,
		SetControlLineState, GetLineCoding. Function will generate the
		notification SerialState.
	*/
	CDC_ACM_D1 = 0b00000010,

	/**
		Function supports the management element SendBreak
	*/
	CDC_ACM_D2 = 0b00000100,

	/**
		Function generates the notification NetworkConnect ION
	*/
	CDC_ACM_D3 = 0b00001000
};

/**
	Define CDC ACM descriptor.

	@param bmCapabilities Capabilities, combination of
	#cdc_acm_capabilities_e values.
*/
#define CDC_ACM( bmCapabilities )                       \
	USB_SUBDESC_HEADER( 4, USB_DESC_CSINTERFACE, CDC_SUBTYPE_ACM ), \
	bmCapabilities

/**
	CDC ACM call management capabilities.
*/
enum cdc_acm_callmanagement_capabilities_e {
	/**
		Function does not perform call management
	*/
	CDC_CALLMANAGEMENT_ND0 = 0b00000000,

	/**
		Function does perform call management
	*/
	CDC_CALLMANAGEMENT_D0  = 0b00000001,

	/**
		Function sends/receives call management information only over
		this Communications Class interface
	*/
	CDC_CALLMANAGEMENT_ND1 = 0b00000000,

	/**
		Function can send/receive call management information over the
		Data Class interface.
	*/
	CDC_CALLMANAGEMENT_D1  = 0b00000010
};

/**
	Define CDC call management descriptor.

	@param bmCapabilities Call management capabilities, combination of
	#cdc_acm_callmanagement_capabilities_e values.
	@param bDataInterface Index of data interface.
*/
#define CDC_CALLMANAGEMENT( bmCapabilities, bDataInterface ) \
	USB_SUBDESC_HEADER( 5, USB_DESC_CSINTERFACE, CDC_SUBTYPE_CALL_MANAGEMENT ), \
	bmCapabilities,                                     \
	bDataInterface

/**
	CDC command packet size
*/
#define CDC_COMMAND_PACKET 8

/**
	CDC command polling interval.
*/
#define CDC_COMMAND_INTERVAL 0xff

/**
	Standard CDC callbacks.
*/
#define INTERFACE_CDC INTERFACE( usb_cdc_initcallback, usb_cdc_deinitcallback, usb_cdc_cmdcallback )

/**
	Callback for initializing CDC related systems.
*/
void usb_cdc_initcallback();

/**
	Callback for deinitializing CDC related systems.
*/
void usb_cdc_deinitcallback();

/**
	Callback for processing of USB requests.

	@param [in] bmRequestType This field specifies characteristics of the
	specific request.
	@param [in] bRequest This field specifies the particular request.
	@param [in] wValue The contents of this field vary according to the
	request.
	@param [in] wIndex The contents of this field vary according to the
	request.
	@param [in] wLength This field specifies the length of the data
	transferred during the second phase of the control transfer.
	@param [in] data Pointer to the data, transferred during the second phase of
	the control transfer.
	@return Success flag, #USB_REQ_OK or #USB_REQ_FAIL
*/
uint16_t usb_cdc_cmdcallback( uint8_t bmRequestType, uint8_t bRequest,
			      uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t* data );

#endif // CLASS_CDC_H

/// @}
/// @}
///////////////////////////////////////////////////////////////////////////////
