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
/// @file				class.h
///
/// @addtogroup usb			USB API
/// @{
/// @defgroup usb_classes		Standard Classes
/// @{
///
/// USB defines class code information that is used to identify a device’s
/// functionality and to nominally load a device driver based on that
/// functionality. The information is contained in three bytes with the
/// names Base Class, SubClass, and Protocol. (Note that ‘Base Class’ is
/// used in this description to identify the first byte of the Class Code
/// triple. That terminology is not used in the USB Specification). There
/// are two places on a device where class code information can be
/// placed.One place is in the Device Descriptor, and the other is in
/// Interface Descriptors. Some defined class codes are allowed to be used
/// only in a Device Descriptor, others can be used in both Device and
/// Interface Descriptors, and some can only be used in Interface
/// Descriptors. The table below shows the currently defined set of Base
/// Class values, what the generic usage is, and where that Base Class can
/// be used (either Device or Interface Descriptors or both).
///
/// Source: http://www.usb.org/developers/defined_class

#ifndef CLASS_H
#define CLASS_H

/**
	Use class code from interface descriptor.

	This base class is defined to be used in Device Descriptors to indicate
	that class information should be determined from the Interface
	Descriptors in the device. There is one class code definition in this
	base class. All other values are reserved.

	This value is also used in Interface Descriptors to indicate a null
	class code triple.
*/
#define USB_DEV_INTERFACE_DEFINED                0x00, 0x00, 0x00

/**
	Audio.

	These class codes may only be used in Interface Descriptors.
*/
#define USB_IF_AUDIO(bSubClass, bProtocol)        0x01, bSubClass, bProtocol

/**
	Communications and CDC control.

	Some class code values (triples) to be used in Device Descriptors and
	some to be used in Interface Descriptors.
*/
#define USB_DEV_CDC(bSubClass, bProtocol)        0x02, bSubClass, bProtocol

/**
	Human interface device (HID).

	These class codes can only be used in Interface Descriptors.
*/
#define USB_IF_HID(bSubClass, bProtocol)        0x03, bSubClass, bProtocol

/**
	Physical.

	These class codes can only be used in Interface Descriptors.
*/
#define USB_ID_PHYSICAL(bSubClass, bProtocol)        0x05, bSubClass, bProtocol

/**
	Still imaging.
*/
#define USB_IF_STILL_IMAGING                        0x06, 0x01, 0x01

/**
	Printer.

	These class codes can only be used in Interface Descriptors.
*/
#define USB_IF_PRINTER(bSubClass, bProtocol)        0x07, bSubClass, bProtocol

/**
	Mass storage.

	These class codes can only be used in Interface Descriptors.
*/
#define USB_IF_MASS_STORAGE(bSubClass, bProtocol) 0x08, bSubClass, bProtocol

///////////////////////////////////////////////////////////////////////////////
/// @name Hub
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/**
	Full speed hub.

	These class codes can only be used in Device Descriptors.
*/
#define USB_DEV_HUB_FS                                0x09, 0x00, 0x00

/**
	Hi-speed hub with single TT.

	These class codes can only be used in Device Descriptors.
*/
#define USB_DEV_HUB_HS_STT                        0x09, 0x00, 0x01

/**
	Hi-speed hub with multiple TTs.

	These class codes can only be used in Device Descriptors.
*/
#define USB_DEV_HUB_HS_MTT                        0x09, 0x00, 0x02

/// @}
///////////////////////////////////////////////////////////////////////////////

/**
	CDC data.

	These class codes can only be used in Interface Descriptors.
*/
#define USB_IF_CDC_DATA(bProtocol)                        0x0a, 0x00, bProtocol

/**
	Smart card.

	These class codes can only be used in Interface Descriptors.
*/
#define USB_IF_SMART_CARD(bSubClass, bProtocol) 0x0b, bSubClass, bProtocol

/**
	Content security.

	These class codes can only be used in Interface Descriptors.
*/
#define USB_IF_CONTENT_SECURITY                        0x0d, 0x00, 0x00

/**
	Video.

	These class codes can only be used in Interface Descriptors.
*/
#define USB_IF_VIDEO(bSubClass, bProtocol)        0x0e, bSubClass, bProtocol

/**
	Personal healthcare.

	These class codes should only be used in Interface Descriptors.
*/
#define USB_IF_PERSONAL_HEALTHCARE(bSubClass, bProtocol) 0x0f, bSubClass, bProtocol

///////////////////////////////////////////////////////////////////////////////
/// @name USB Audio/Video
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/**
	Audio/Video device - AVControl interface.

	These class codes can only be used in Interface Descriptors.
*/
#define USB_IF_AV_CONTROL                        0x10, 0x01, 0x00

/**
	Audio/Video device - AVData video streaming interface.

	These class codes can only be used in Interface Descriptors.
*/
#define USB_IF_AV_DATA_VIDEO                        0x10, 0x02, 0x00

/**
	Audio/Video device - AVData audio streaming interface.

	These class codes can only be used in Interface Descriptors.
*/
#define USB_IF_AV_DATA_AUDIO                        0x10, 0x03, 0x00

/// @}
///////////////////////////////////////////////////////////////////////////////

/**
	USB2 diagnostic device.

	This base class is defined for devices that diagnostic devices. There
	is currently only one value defined. All other values are reserved.
	This class code can be used in Device or Interface Descriptors.

	Definition for this device can be found at
	http://www.intel.com/technology/usb/spec.htm
*/
#define USB_DEV_DIAGNOSTIC                        0xdc, 0x01, 0x01

///////////////////////////////////////////////////////////////////////////////
/// @name Wireless Controller
///////////////////////////////////////////////////////////////////////////////
/// @{
///


/**
	Bluetooth programming interface.

	These class codes are to be used in Interface Descriptors, with the
	exception of the Bluetooth class code which can also be used in a
	Device Descriptor.

	Get specific information from http://www.bluetooth.com.
*/
#define USB_IF_BLUETOOTH_PROGRAMMING                0xe0, 0x01, 0x01

/**
	UWB radio control interface.

	These class codes are to be used in Interface Descriptors, with the
	exception of the Bluetooth class code which can also be used in a
	Device Descriptor.

	Definition for this is found in the Wireless USB Specification in
	Chapter 8.
*/
#define USB_IF_UWB_RADIO_CONTROL                0xe0, 0x01, 0x02

/**
	Remote NDIS.

	These class codes are to be used in Interface Descriptors, with the
	exception of the Bluetooth class code which can also be used in a
	Device Descriptor.

	Information can be found at:
	http://www.microsoft.com/windowsmobile/mobileoperators/default.mspx
*/
#define USB_IF_REMOTE_NDIS                        0xe0, 0x01, 0x03

/**
	Bluetooth AMP controller.

	These class codes are to be used in Interface Descriptors, with the
	exception of the Bluetooth class code which can also be used in a
	Device Descriptor.

	Get specific information from http://www.bluetooth.com.
*/
#define USB_IF_BLUETOOTH_AMP_CONTROLLER                0xe0, 0x01, 0x04

/**
	Host wire adapter control/data interface.

	These class codes are to be used in Interface Descriptors, with the
	exception of the Bluetooth class code which can also be used in a
	Device Descriptor.

	Definition can be found in the Wireless USB Specification in Chapter 8.
*/
#define USB_IF_HOST_WIRE_CD                        0xe0, 0x02, 0x01

/**
	Device wire adapter control/data interface.

	These class codes are to be used in Interface Descriptors, with the
	exception of the Bluetooth class code which can also be used in a
	Device Descriptor.

	Definition can be found in the Wireless USB Specification in Chapter 8.
*/
#define USB_IF_DEVICE_WIRE_CD                        0xe0, 0x02, 0x02

/**
	Device wire adapter isochronous interface.

	These class codes are to be used in Interface Descriptors, with the
	exception of the Bluetooth class code which can also be used in a
	Device Descriptor.

	Definition can be found in the Wireless USB Specification in Chapter 8.
*/
#define USB_IF_DEVICE_WIRE_ISO                        0xe0, 0x02, 0x03

/// @}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @name Miscellaneous
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/**
	Active sync device.

	This class code can be used in either Device or Interface Descriptors.
	Contact Microsoft for more information on this class.
*/
#define USB_DEV_MISC_ACTIVESYNC                        0xef, 0x01, 0x01

/**
	Palm sync device.

	This class code can be used in either Device or Interface Descriptors.
*/
#define USB_DEV_MISC_PALMSYNC                        0xef, 0x01, 0x02

/**
	Interface association descriptor (IAD).

	The usage of this class code triple is defined in the Interface
	Association Descriptor ECN that is provided on http://www.usb.org .
	This class code may only be used in Device Descriptors.
*/
#define USB_DEV_MISC_IAD                        0xef, 0x02, 0x01

/**
	Wire adapter multifunction peripheral programming interface.

	Definition can be found in the Wireless USB Specification in Chapter 8.
	This class code may only be used in Device Descriptors.
*/
#define USB_DEV_MISC_WAMP                        0xef, 0x02, 0x02

/**
	Cable based association framework.

	This is defined in the Association Model addendum to the Wireless USB
	specification. This class code may only be used in Interface
	Descriptors.
*/
#define USB_IF_MISC_CBAF                        0xef, 0x03, 0x01

/**
	RNDIS over ethernet.

	Connecting a host to the Internet via Ethernet mobile device. The
	device appears to the host as an Ethernet gateway device.

	This class code may only be used in Interface Descriptors.
*/
#define USB_IF_MISC_RNDIS_OVER_ETHERNET                0xef, 0x04, 0x01

/**
	RNDIS over WiFi.

	Connecting a host to the Internet via WiFi enabled mobile device. The
	device represents itself to the host as an 802.11 compliant network
	device.

	This class code may only be used in Interface Descriptors.
*/
#define USB_IF_MISC_RNDIS_OVER_WIFI                0xef, 0x04, 0x02

/**
	RNDIS over WiMax.

	Connecting a host to the Internet via WiMAX enabled mobile device. The
	device is represented to the host as an 802.16 network device.

	This class code may only be used in Interface Descriptors.
*/
#define USB_IF_MISC_RNDIS_OVER_WIMAX                0xef, 0x04, 0x03

/**
	RNDIS over WWAN.

	Connecting a host to the Internet via a device using mobile broadband,
	i.e. WWAN (GSM/CDMA).

	This class code may only be used in Interface Descriptors.
*/
#define USB_IF_MISC_RNDIS_OVER_WWAN                0xef, 0x04, 0x04

/**
	RNDIS for raw IPv4.

	Connecting a host to the Internet using raw IPv4 via non-Ethernet
	mobile device. Devices that provide raw IPv4, not in an Ethernet
	packet, may use this form to in lieu of other stock types.

	This class code may only be used in Interface Descriptors.
*/
#define USB_IF_MISC_RNDIS_FOR_IPV4                0xef, 0x04, 0x05

/**
	RNDIS for raw IPv6.

	Connecting a host to the Internet using raw IPv6 via non-Ethernet
	mobile device. Devices that provide raw IPv6, not in an Ethernet
	packet, may use this form to in lieu of other stock types.

	This class code may only be used in Interface Descriptors.
*/
#define USB_IF_MISC_RNDIS_FOR_IPV6                0xef, 0x04, 0x06

/**
	RNDIS for GPRS.

	Connecting a host to the Internet over GPRS mobile device using the
	device’s cellular radio
*/
#define USB_IF_MISC_RNDIS_FOR_GPRS                0xef, 0x04, 0x07

/**
	USB3 vision control.

	Machine Vision Device conforming to the USB3 Vision specification. This
	standard covers cameras and other related devices that are typically
	used in machine vision, industrial, and embedded applications.

	Reference: http://visiononline.org/

	This class code may only be used in Interface Descriptors.
*/
#define USB_IF_USB3_VISION_CONTROL                0xef, 0x05, 0x00

/**
	USB3 vision event.

	Machine Vision Device conforming to the USB3 Vision specification. This
	standard covers cameras and other related devices that are typically
	used in machine vision, industrial, and embedded applications.

	Reference: http://visiononline.org/

	This class code may only be used in Interface Descriptors.
*/
#define USB_IF_USB3_VISION_EVENT                0xef, 0x05, 0x01

/**
	USB3 vision streaming.

	Machine Vision Device conforming to the USB3 Vision specification. This
	standard covers cameras and other related devices that are typically
	used in machine vision, industrial, and embedded applications.

	Reference: http://visiononline.org/

	This class code may only be used in Interface Descriptors.
*/
#define USB_IF_USB3_VISION_STREAMING                0xef, 0x05, 0x02

/// @}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @name Application Specific
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/**
	Device firmware upgrade.

	Device class definition provided on http://www.usb.org.
*/
#define USB_IF_DFU                                0xfe, 0x01, 0x01

/**
	IRDA bridge device.

	Device class definition provided on http://www.usb.org.
*/
#define USB_IF_IRDA_BRIDGE                        0xfe, 0x02, 0x00

/**
	USB test and measurement device.

	Definition provided in the USB Test and Measurement Class spec found on
	http://www.usb.org.
*/
#define USB_IF_USB_TM                                0xfe, 0x03, 0x00

/**
	USB Test and Measurement Device conforming to the USBTMC USB488
	Subclass Specification found on http://www.usb.org.
*/
#define USB_IF_USB_TMC                                0xfe, 0x03, 0x01

/// @}
///////////////////////////////////////////////////////////////////////////////

/**
	Vendor specific.

	This base class is defined for vendors to use as they please. These
	class codes can be used in both Device and Interface Descriptors.
*/
#define USB_DEV_VENDOR(bSubClass, bProtocol)        0xff, bSubClass, bProtocol

#endif // CLASS_H

/// @}
/// @}
///////////////////////////////////////////////////////////////////////////////
