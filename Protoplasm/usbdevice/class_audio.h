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
/// @file				class_audio.h
///
/// @addtogroup usb			USB API
/// @{
/// @defgroup usb_audio			Audio Class
/// @{
///	Audio Class.
///

#ifndef CLASS_AUDIO_H
#define CLASS_AUDIO_H

#include "descriptor.h"
#include "common.h"
#include "class.h"

/**
	USB Audio Version
*/
enum audio_versions_e {
    AUDIO_VERSION = 0x0100                        ///< USB Audio 1.0.
};

/**
	Example MIDI Descriptor:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Device Descriptor
	DESCRIPTOR( STMICROELECTRONICS, STM32F4xx, VERSION( 1, 0, 0 ),
		    USB_DEV_INTERFACE_DEFINED );

	// Configuration Descriptor
	CONFIGURATION(
		USB_CONFIGURATION(
			bNumInterfaces( 2 ),
			bConfigurationValue( 1 ),
			iConfiguration( 0 ),
			bmAttributes( USB_CONF_SELFPOWERED ),
			bMaxPower( 100 ),

			USB_INTERFACE(
				bInterfaceNumber( 0 ),
				bAlternateSetting( 0 ),
				bNumEndpoints( 0 ),
				iInterface( 0 ),
				USB_IF_AUDIO_CONTROL
				),
			AUDIO_AUDIOCONTROL_HEADER(
				baInterfaceNr( 1 )
				),
			USB_INTERFACE(
				bInterfaceNumber( 1 ),
				bAlternateSetting( 0 ),
				bNumEndpoints( 2 ),
				iInterface( 0 ),
				USB_IF_AUDIO_MIDI_STREAMING
				),
			AUDIO_MIDISTREAM_HEADER(
				AUDIO_MIDI_IN_JACK(
					bJackType( AUDIO_MIDI_EMBEDDED ),
					bJackID( 1 ),
					iJack( 0 )
					),
				AUDIO_MIDI_IN_JACK(
					bJackType( AUDIO_MIDI_EXTERNAL ),
					bJackID( 2 ),
					iJack( 0 )
					),
				AUDIO_MIDI_OUT_JACK(
					bJackType( AUDIO_MIDI_EMBEDDED ),
					bJackID( 3 ),
					baSourceIDPin( bSourceIDPin(2,1) ),
					iJack( 0 )
					),
				AUDIO_MIDI_OUT_JACK(
					bJackType( AUDIO_MIDI_EXTERNAL ),
					bJackID( 4 ),
					baSourceIDPin( bSourceIDPin(1,1) ),
					iJack( 0 )
					),
				AUDIO_MIDISTREAM_DATA_ENDPOINT(
					bEndpointAddress( 1 ),
					wMaxPacketSize( 64 ),
					),
				AUDIO_MIDISTREAM_DATA_CSENDPOINT(
					baAssocJackID( 1 )
					),
				AUDIO_MIDISTREAM_DATA_ENDPOINT(
					bEndpointAddress( 0x81 ),
					wMaxPacketSize( 64 ),
					),
				AUDIO_MIDISTREAM_DATA_CSENDPOINT(
					baAssocJackID( 3 ) )
				)
			);
		);
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

///////////////////////////////////////////////////////////////////////////////
/// @name Interface Subclasses
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/**
	Generic audio interface.
*/
#define USB_IF_AUDIO_GENERIC                USB_IF_AUDIO( 0x00, 0x00 )

/**
	Audio control interface.
*/
#define USB_IF_AUDIO_CONTROL                USB_IF_AUDIO( 0x01, 0x00 )

/**
	Audio streaming interface.
*/
#define USB_IF_AUDIO_STREAMING                USB_IF_AUDIO( 0x02, 0x00 )

/**
	MIDI streaming interface.
*/
#define USB_IF_AUDIO_MIDI_STREAMING        USB_IF_AUDIO( 0x03, 0x00 )

/// @}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @name Status Interrupt Endpoint
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/**
	bStatusType.originator values.
*/
enum audio_bStatusTupe_originator_e {
    AUDIO_ORIGINATOR_AUDIOCONTROL_IF = 0,        ///< AudioControl interface.
    AUDIO_ORIGINATOR_AUDIOSTREAMING_IF = 1, ///< AudioStreaming interface.
    AUDIO_ORIGINATOR_AUDIOSTREAMING_EP = 2        ///< AudioStreaming endpoint.
};

/**
	bStatusType values.
*/
enum audio_status_type_e {
    AUDIO_STATUS_AUDIOCONTROL_IF = 0,        ///< AudioControl interface.
    AUDIO_STATUS_AUDIOSTREAMING_IF = 1,        ///< AudioStreaming interface.
    AUDIO_STATUS_AUDIOSTREAMING_EP = 2,        ///< AudioStreaming endpoint.

    AUDIO_STATUS_CONTENTS_CHANGED = 0x40,        ///< Memory contents changed.
    AUDIO_STATUS_INTERRUPT_PENDING = 0x80,        ///< Interrupt pending.

    AUDIO_STATUS_ORIGINATOR_MASK = 0x0f        ///< Originator mask.
};

/**
	Status Word Format.
*/
struct audio_status_s {
    /**
	    Status type, combination of #audio_status_type_e values.
    */
    uint8_t bStatusType;

    /**
	    ID of the Terminal, Unit, interface, or endpoint that reports
	    the interrupt.
    */
    uint8_t bOriginator;
} packed;

/// @}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @name Audio Channel Cluster Format
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/**
	Channels.
*/
enum audio_channel_e {
    AUDIO_CHN_FRONT_LEFT = 0x0001,        ///< Left Front
    AUDIO_CHN_FRONT_RIGHT = 0x0002,        ///< Right Front
    AUDIO_CHN_FRONT_CENTER = 0x0004,        ///< Center Front
    AUDIO_CHN_LFE = 0x0008,        ///< LFE
    AUDIO_CHN_SURROUND_LEFT = 0x0010,        ///< Left Surround
    AUDIO_CHN_SURROUND_RIGHT = 0x0020,        ///< Right Surround
    AUDIO_CHN_FRONT_CENTER_LEFT = 0x0040,        ///< Left of Center
    AUDIO_CHN_FRONT_CENTER_RIGHT = 0x0080,        ///< Right of Center
    AUDIO_CHN_SURROUND = 0x0100,        ///< Surround
    AUDIO_CHN_SIDE_LEFT = 0x0200,        ///< Side Left
    AUDIO_CHN_SIDE_RIGHT = 0x0400,        ///< Side Right
    AUDIO_CHN_TOP = 0x0800        ///< Top
};

/**
	Channel cluster.
*/
struct audio_channel_cluster_s {
    /**
	    A number that specifies how many logical audio channels are
	    present in the cluster.
    */
    uint8_t bNrChannels;

    /**
	    A bit field that indicates which spatial locations are present
	    in the cluster. Combination of #audio_channel_e values.
    */
    uint16_t wChannelConfig;

    /**
	    Index to a string descriptor that describes the spatial
	    location of the first non-predefined logical channel in the
	    cluster. The spatial locations of all remaining logical
	    channels @b must be described by string descriptors with
	    indices that immediately follow the index of the descriptor of
	    the first non-predefined channel. Therefore, iChannelNames
	    inherently describes an array of string descriptor indices,
	    ranging from iChannelNames to (iChannelNames + (bNrChannels-
	    [Number_Of_Bits_Set]) - 1)
    */
    uint8_t iChannelNames;
};

/// @}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @name Descriptor Fields
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/// AudioStreaming or MIDIStreaming interface indices
///
#define baInterfaceNr(...) __VA_ARGS__

/// Terminal ID
///
#define bTerminalId(x) x

/// Terminal Type
///
#define wTerminalType(x) x

/// Associated terminal
///
#define bAssocTerminal(x) x

/// Number of channels
///
#define bNrChannels(x) x

/// Channel configuration
///
#define wChannelConfig(x) x

/// Index of string descriptor for the first channel
///
#define iChannelNames(x) x

/// Index of string descriptor for terminal
///
#define iTerminal(x) x

/// Source ID
///
#define bSourceID(x) x

/// Unit ID
///
#define bUnitID(x) x

/// Source IDs
///
#define baSourceID(...) __VA_ARGS__

/// Source IDs and Pins
///
#define baSourceIDPin(...) __VA_ARGS__

/// Available controls
///
#define bmControls(...) __VA_ARGS__

/// Index of string descriptor for mixer
///
#define iMixer(x) x

/// Index of a string descriptor, describing this Feature Unit.
///
#define iFeature(x) x

/// Available controls array
///
#define bmaControls(...) __VA_ARGS__

/// Size in bytes of controls bitfield
///
#define bControlSize(x) x

/// Index of a string descriptor, describing this Processing Unit.
///
#define iProcessing(x) x

/// Processing unit type
///
#define wProcessType(x) x

/// Modes, supported by Processing Unit
///
#define waModes(...) __VA_ARGS__

/// Extension Unit code
///
#define wExtensionCode(x) x

/// Index of a string descriptor, describing this Extension Unit.
///
#define iExtension(x) x

/// The Terminal ID of the Terminal to which endpoint is connected.
///
#define bTerminalLink(x) x

/// Delay, introduced by data path. Expressed in number of frames.
///
#define bDelay(x) x

/// Audio Data Format
///
#define wFormatTag(x) x

/// The address of the endpoint used to communicate synchronization information
/// if required by this endpoint. Reset to zero if no synchronization pipe is used.
///
#define bSynchAddress(x) x

/// Indicates the units used for the wLockDelay field.
///
#define bLockDelayUnits(x) x

/// Indicates the time it takes this endpoint to reliably lock its internal
/// clock recovery circuitry.
///
#define wLockDelay(x) x

/// This field indicates the rate at which an isochronous synchronization pipe
/// provides new synchronization feedback data. This rate must be a power of 2,
/// therefore only the power is reported back and the range of this field is from 1
/// (2 ms) to 9 (512 ms).
///
#define bRefresh(x) x

/// Constant identifying the Format Type the AudioStreaming interface is using.
///
#define bFormatType(x) x

/// The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.
///
#define bSubframeSize(x) x

/// The number of effectively used bits from the available bits in an audio
/// subframe.
///
#define bBitResolution(x) x

/// Indicates how the sampling frequency can be programmed. 0 - continuous,
/// 1..255 - discrete
///
#define bSamFreqType(x) x

/// Indicates the maximum number of bits per second this interface can handle.
/// Expressed in kbits/s
///
#define wMaxBitRate(x) x

/// Indicates the number of PCM audio samples contained in one encoded audio
/// frame.
///
#define wSamplesPerFrame(x) x

/// Type of MIDI jacl
///
#define bJackType(x) x

/// Constant uniquely identifying the MIDI Jack within the USB-MIDI
/// function.
///
#define bJackID(x) x

/// Index of a string descriptor, describing the MIDI Jack.
///
#define iJack(x) x

/// IDs of the Entity to which the Input Pins of this MIDI OUT Jack is
/// connected interleaved with Output Pin numbers of the Entity to which that
/// Input Pin of this MIDI OUT Jack is connected.
///
#define bSourceIDPin(x, y) x, y

/// Constant uniquely identifying the MIDI element.
///
#define bElementID(x) x

/// Number of Output Pins of this Element.
///
#define bNrOutPins(x) x

/// The Terminal ID of the Input Terminal to which this Element is connected.
///
#define bInTerminalLink(x) x

/// The Terminal ID of the Output Terminal to which this Element is connected.
///
#define bOutTerminalLink(x) x

/// Size, in bytes of the bmElementCaps field.
///
#define bElCapsSize(x) x

/// Element capabilities.
///
#define bmElementCaps(...) __VA_ARGS__

/// Index of a string descriptor, describing the Element.
///
#define iElement(x) x

/// Embedded Jacks that is associated with endpoint.
///
#define baAssocJackID(...) __VA_ARGS__

/// @}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @name Descriptors
///////////////////////////////////////////////////////////////////////////////
/// @{
///

/**
	Class-Specific AC Interface Header Descriptor

	@param baInterfaceNr AudioStreaming or MIDIStreaming interface indices in
		the collection.
	@param ... Descriptors.
*/
#define AUDIO_AUDIOCONTROL_HEADER(baInterfaceNr, ...)                        \
        USB_SUBDESC_HEADER( 8+bLength(baInterfaceNr), USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_HEADER ),        \
        field16( AUDIO_VERSION ),                                                \
        wTotalLength( 8+bLength(baInterfaceNr), __VA_ARGS__ ),                \
        bLength( baInterfaceNr ),                                                \
        baInterfaceNr,                                                                \
        __VA_ARGS__

/**
	Class-Specific AC Interface Header Descriptor without children.

	@param baInterfaceNr AudioStreaming or MIDIStreaming interface indices in
		the collection.
*/
#define AUDIO_AUDIOCONTROL_HEADER_NULL(baInterfaceNr)                        \
        USB_SUBDESC_HEADER( 8+bLength(baInterfaceNr), USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_HEADER ),        \
        field16( AUDIO_VERSION ),                                                \
        wTotalLength( 8, baInterfaceNr ),                                        \
        bLength( baInterfaceNr ),                                                \
        baInterfaceNr

/**
	Input Terminal Descriptor.

	@param bTerminalId Constant uniquely identifying the Terminal within
		the audio function. This value is used in all requests to
		address this terminal.
	@param wTerminalType Constant characterizing the type of Terminal.
	@param bAssocTerminal ID of the Output Terminal to which this
		Input Terminal is associated.
	@param bNrChannels Number of logical output channels in the
		Terminal’s output audio channel cluster.
	@param wChannelConfig Describes the spatial location of the
		logical channels.
	@param iChannelNames Index of a string descriptor, describing
		the name of the first logical channel.
	@param iTerminal Index of a string descriptor, describing
		the Input Terminal.
*/
#define AUDIO_INPUT_TERMINAL(bTerminalId, wTerminalType, bAssocTerminal, bNrChannels, wChannelConfig, iChannelNames, iTerminal) \
        USB_SUBDESC_HEADER( 12, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_INPUT_TERMINAL ), \
        bTerminalId,                                                                \
        field16( wTerminalType ),                                                \
        bAssocTerminal,                                                                \
        bNrChannels,                                                                \
        field16( wChannelConfig ),                                                \
        iChannelNames,                                                                \
        iTerminal

/**
	Output Terminal Descriptor.

	@param bTerminalID Constant uniquely identifying the Terminal within the
		audio function.
	@param wTerminalType Constant characterizing the type of Terminal.
	@param bAssocTerminal Constant, identifying the Input Terminal
		to which this Output Terminal is associated.
	@param bSourceID ID of the Unit or Terminal to which this
		Terminal is connected.
	@param iTerminal Index of a string descriptor, describing
		the Output Terminal.
*/
#define AUDIO_OUTPUT_TERMINAL(bTerminalID, wTerminalType, bAssocTerminal, bSourceID, iTerminal) \
        USB_SUBDESC_HEADER( 9, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_OUTPUT_TERMINAL ), \
        bTerminalID,                                                                \
        field16( wTerminalType ),                                                \
        bAssocTerminal,                                                                \
        bSourceID,                                                                \
        iTerminal

/**
	Mixer Unit Descriptor.

	@param bUnitID Constant uniquely identifying the Unit within the audio
		function.
	@param baSourceID IDs of the Unit or Terminal to which the Input Pins of
		this Mixer Unit is connected.
	@param bNrChannels Number of logical output channels in the
		Mixer’s output audio channel cluster.
	@param wChannelConfig Describes the spatial location of the
		logical channels.
	@param iChannelNames Index of a string descriptor, describing
		the name of the first logical channel.
	@param bmControls Bit map indicating which mixing Controls
		are programmable.
	@param iMixer Index of a string descriptor, describing
		the Mixer Unit.
*/
#define AUDIO_MIXER_UNIT(bUnitID, baSourceID, bNrChannels, wChannelConfig, iChannelNames, bmControls, iMixer) \
        USB_SUBDESC_HEADER( 10+bLength(baSourceID)+bLength(bmControls), USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_MIXER_UNIT ), \
        bUnitID,                                                                \
        bLength( baSourceID ),                                                        \
        baSourceID,                                                                \
        bNrChannels,                                                                \
        field16( wChannelConfig ),                                                \
        iChannelNames,                                                                \
        bmControls,                                                                \
        iMixer

/**
	Selector Unit Descriptor

	@param bUnitID Constant uniquely identifying the Unit
		within the audio function. This value is
		used in all requests to address this Unit.
	@param baSourceID IDs of the Unit or Terminal to which the Input Pins
		of this Selector Unit is connected.
	@param iSelector Index of a string descriptor, describing
		the Selector Unit.
*/
#define AUDIO_SELECTOR_UNIT(bUnitID, baSourceID, iSelector)        \
        USB_SUBDESC_HEADER( 6+bLength( baSourceID ), USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_SELECTOR_UNIT ), \
        bUnitID,                                                                \
        bLength( baSourceID ),                                                        \
        baSourceID,                                                                \
        iSelector

/**
	Feature Unit Descriptor

	@param bUnitID Constant uniquely identifying the Unit
		within the audio function. This value is
		used in all requests to address this Unit.
	@param bSourceID ID of the Unit or Terminal to which this
		Feature Unit is connected.
	@param bControlSize Size in bytes of the bmControls field
	@param bmaControls A bit set to 1 indicates that the mentioned
		Control is supported for logical channel.
	@param iFeature Index of a string descriptor, describing
		this Feature Unit.
*/
#define AUDIO_FEATURE_UNIT(bUnitID, bSourceID, bControlSize, bmaControls, iFeature) \
        USB_SUBDESC_HEADER( 7+bLength(bmaControls), USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_FEATURE_UNIT ), \
        bUnitID,                                                                \
        bSourceID,                                                                \
        bControlSize,                                                                \
        bmaControls,                                                                \
        iFeature

/**
	Common Part of the Processing Unit Descriptor

	@param bUnitID Constant uniquely identifying the Unit
		within the audio function. This value is
		used in all requests to address this Unit.
	@param wProcessType Constant identifying the type of
		processing this Unit is performing.
	@param baSourceID IDs of the Unit or Terminal to which the Input Pins
		of this Processing Unit is connected.
	@param bNrChannels Number of logical output channels in the
		audio channel cluster of the Processing Unit.
	@param wChannelConfig Describes the spatial location of the
		logical channels in the audio channel cluster of the
		Processing Unit.
	@param iChannelNames Index of a string descriptor that describes
		the name of the first logical channel in the
		audio channel cluster of the Processing Unit.
	@param bControlSize Size in bytes of the bmControls field
	@param bmControls A bit set to 1 indicates that the mentioned
		Control is supported for logical channel.
	@param iProcessing Index of a string descriptor, describing
		this Processing Unit.
	@param ... A process-specific descriptor is appended
		to the common descriptor.
*/
#define AUDIO_PROCESSING_UNIT(bUnitID, wProcessType, baSourceID, bNrChannels, wChannelConfig, iChannelNames, bControlSize, bmControls, iProcessing, ...) \
        USB_SUBDESC_HEADER( 13+bLength(baSourceID)+bControlSize+bLength(__VA_ARGS__), USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_PROCESSING_UNIT ), \
        bUnitID,                                                                \
        field16( wProcessType ),                                                \
        bLength( baSourceID ),                                                        \
        baSourceID,                                                                \
        bNrChannels,                                                                \
        field16( wChannelConfig ),                                                \
        iChannelNames,                                                                \
        bControlSize,                                                                \
        bmControls,                                                                \
        iProcessing,                                                                \
        __VA_ARGS__

/**
	Up/Down-mix Processing Unit Descriptor

	@param bUnitID Constant uniquely identifying the Unit
		within the audio function. This value is
		used in all requests to address this Unit.
	@param bSourceID ID of the Unit or Terminal to which the Input Pin
		of this Processing Unit is connected
	@param bNrChannels Number of logical output channels in the
		audio channel cluster of the Processing Unit.
	@param wChannelConfig Describes the spatial location of the
		logical channels in the audio channel cluster of the
		Processing Unit.
	@param iChannelNames Index of a string descriptor that describes
		the name of the first logical channel in the
		audio channel cluster of the Processing Unit.
	@param bControlSize Size in bytes of the bmControls field
	@param bmControls A bit set to 1 indicates that the mentioned
		Control is supported for logical channel.
	@param iProcessing Index of a string descriptor, describing
		this Processing Unit.
	@param waModes Modes, supported by this Processing Unit.
*/
#define AUDIO_UPDOWN_MIX_UNIT(bUnitID, bSourceID, bNrChannels, wChannelConfig, iChannelNames, bControlSize, bmControls, iProcessing, waModes) \
        USB_SUBDESC_HEADER( 13+bLength(bSourceID)+bControlSize+bLength(waModes)+1, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_PROCESSING_UNIT ), \
        bUnitID,                                                                \
        field16( AUDIO_UPDOWN_PROCESS ),                                                \
        bLength( bSourceID ),                                                        \
        bSourceID,                                                                \
        bNrChannels,                                                                \
        field16( wChannelConfig ),                                                \
        iChannelNames,                                                                \
        bControlSize,                                                                \
        bmControls,                                                                \
        iProcessing,                                                                \
        bLength( waModes ),                                                        \
        waModes

/**
	Dolby Prologic Processing Unit Descriptor

	@param bUnitID Constant uniquely identifying the Unit
		within the audio function. This value is
		used in all requests to address this Unit.
	@param bSourceID ID of the Unit or Terminal to which the Input Pin
		of this Processing Unit is connected
	@param bNrChannels Number of logical output channels in the
		audio channel cluster of the Processing Unit.
	@param wChannelConfig Describes the spatial location of the
		logical channels in the audio channel cluster of the
		Processing Unit.
	@param iChannelNames Index of a string descriptor that describes
		the name of the first logical channel in the
		audio channel cluster of the Processing Unit.
	@param bControlSize Size in bytes of the bmControls field
	@param bmControls A bit set to 1 indicates that the mentioned
		Control is supported for logical channel.
	@param iProcessing Index of a string descriptor, describing
		this Processing Unit.
	@param waModes Modes, supported by this Processing Unit.
*/
#define AUDIO_DOLBY_PROLOGIC_UNIT(bUnitID, bSourceID, bNrChannels, wChannelConfig, iChannelNames, bControlSize, bmControls, iProcessing, waModes) \
        USB_SUBDESC_HEADER( 13+bLength(bSourceID)+bControlSize+bLength(waModes)+1, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_PROCESSING_UNIT ), \
        bUnitID,                                                                \
        field16( AUDIO_DOLBY_PROLOGIC_PROCESS ),                                \
        bLength( bSourceID ),                                                        \
        bSourceID,                                                                \
        bNrChannels,                                                                \
        field16( wChannelConfig ),                                                \
        iChannelNames,                                                                \
        bControlSize,                                                                \
        bmControls,                                                                \
        iProcessing,                                                                \
        bLength( waModes ),                                                        \
        waModes

/**
	3D-Stereo Extender Processing Unit Descriptor

	@param bUnitID Constant uniquely identifying the Unit
		within the audio function. This value is
		used in all requests to address this Unit.
	@param bSourceID ID of the Unit or Terminal to which the Input Pin
		of this Processing Unit is connected
	@param bNrChannels Number of logical output channels in the
		audio channel cluster of the Processing Unit.
	@param wChannelConfig Describes the spatial location of the
		logical channels in the audio channel cluster of the
		Processing Unit.
	@param iChannelNames Index of a string descriptor that describes
		the name of the first logical channel in the
		audio channel cluster of the Processing Unit.
	@param bControlSize Size in bytes of the bmControls field
	@param bmControls A bit set to 1 indicates that the mentioned
		Control is supported for logical channel.
	@param iProcessing Index of a string descriptor, describing
		this Processing Unit.
*/
#define AUDIO_DOLBY_3D_STEREO_UNIT(bUnitID, bSourceID, bNrChannels, wChannelConfig, iChannelNames, bControlSize, bmControls, iProcessing) \
        USB_SUBDESC_HEADER( 13+bLength(bSourceID)+bControlSize, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_PROCESSING_UNIT ), \
        bUnitID,                                                                \
        field16( AUDIO_3D_STEREO_EXTENDER_PROCESS ),                                \
        bLength( bSourceID ),                                                        \
        bSourceID,                                                                \
        bNrChannels,                                                                \
        field16( wChannelConfig ),                                                \
        iChannelNames,                                                                \
        bControlSize,                                                                \
        bmControls,                                                                \
        iProcessing

/**
	Reverberation Processing Unit Descriptor

	@param bUnitID Constant uniquely identifying the Unit
		within the audio function. This value is
		used in all requests to address this Unit.
	@param bSourceID ID of the Unit or Terminal to which the Input Pin
		of this Processing Unit is connected
	@param bNrChannels Number of logical output channels in the
		audio channel cluster of the Processing Unit.
	@param wChannelConfig Describes the spatial location of the
		logical channels in the audio channel cluster of the
		Processing Unit.
	@param iChannelNames Index of a string descriptor that describes
		the name of the first logical channel in the
		audio channel cluster of the Processing Unit.
	@param bControlSize Size in bytes of the bmControls field
	@param bmControls A bit set to 1 indicates that the mentioned
		Control is supported for logical channel.
	@param iProcessing Index of a string descriptor, describing
		this Processing Unit.
*/
#define AUDIO_REVERBERATION_UNIT(bUnitID, bSourceID, bNrChannels, wChannelConfig, iChannelNames, bControlSize, bmControls, iProcessing) \
        USB_SUBDESC_HEADER( 13+bLength(bSourceID)+bControlSize, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_PROCESSING_UNIT ), \
        bUnitID,                                                                \
        field16( AUDIO_REVERBERATION_UNIT ),                                \
        bLength( bSourceID ),                                                        \
        bSourceID,                                                                \
        bNrChannels,                                                                \
        field16( wChannelConfig ),                                                \
        iChannelNames,                                                                \
        bControlSize,                                                                \
        bmControls,                                                                \
        iProcessing

/**
	Chorus Processing Unit Descriptor

	@param bUnitID Constant uniquely identifying the Unit
		within the audio function. This value is
		used in all requests to address this Unit.
	@param bSourceID ID of the Unit or Terminal to which the Input Pin
		of this Processing Unit is connected
	@param bNrChannels Number of logical output channels in the
		audio channel cluster of the Processing Unit.
	@param wChannelConfig Describes the spatial location of the
		logical channels in the audio channel cluster of the
		Processing Unit.
	@param iChannelNames Index of a string descriptor that describes
		the name of the first logical channel in the
		audio channel cluster of the Processing Unit.
	@param bControlSize Size in bytes of the bmControls field
	@param bmControls A bit set to 1 indicates that the mentioned
		Control is supported for logical channel.
	@param iProcessing Index of a string descriptor, describing
		this Processing Unit.
*/
#define AUDIO_CHORUS_UNIT(bUnitID, bSourceID, bNrChannels, wChannelConfig, iChannelNames, bControlSize, bmControls, iProcessing) \
        USB_SUBDESC_HEADER( 13+bLength(bSourceID)+bControlSize, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_PROCESSING_UNIT ), \
        bUnitID,                                                                \
        field16( AUDIO_CHORUS_UNIT ),                                \
        bLength( bSourceID ),                                                        \
        bSourceID,                                                                \
        bNrChannels,                                                                \
        field16( wChannelConfig ),                                                \
        iChannelNames,                                                                \
        bControlSize,                                                                \
        bmControls,                                                                \
        iProcessing

/**
	Compressor Processing Unit Descriptor

	@param bUnitID Constant uniquely identifying the Unit
		within the audio function. This value is
		used in all requests to address this Unit.
	@param bSourceID ID of the Unit or Terminal to which the Input Pin
		of this Processing Unit is connected
	@param bNrChannels Number of logical output channels in the
		audio channel cluster of the Processing Unit.
	@param wChannelConfig Describes the spatial location of the
		logical channels in the audio channel cluster of the
		Processing Unit.
	@param iChannelNames Index of a string descriptor that describes
		the name of the first logical channel in the
		audio channel cluster of the Processing Unit.
	@param bControlSize Size in bytes of the bmControls field
	@param bmControls A bit set to 1 indicates that the mentioned
		Control is supported for logical channel.
	@param iProcessing Index of a string descriptor, describing
		this Processing Unit.
*/
#define AUDIO_COMPRESSOR_UNIT(bUnitID, bSourceID, bNrChannels, wChannelConfig, iChannelNames, bControlSize, bmControls, iProcessing) \
        USB_SUBDESC_HEADER( 13+bLength(bSourceID)+bControlSize, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_PROCESSING_UNIT ), \
        bUnitID,                                                                \
        field16( AUDIO_COMPRESSOR_UNIT ),                                        \
        bLength( bSourceID ),                                                        \
        bSourceID,                                                                \
        bNrChannels,                                                                \
        field16( wChannelConfig ),                                                \
        iChannelNames,                                                                \
        bControlSize,                                                                \
        bmControls,                                                                \
        iProcessing

/**
	Extension Unit Descriptor

	@param bUnitID Constant uniquely identifying the Unit
		within the audio function. This value is
		used in all requests to address this Unit.
	@param wExtensionCode Vendor-specific code identifying the
		Extension Unit.
	@param baSourceID IDs of the Unit or Terminal to which the Input Pins
		of this Processing Unit is connected.
	@param bNrChannels Number of logical output channels in the
		audio channel cluster of the Processing Unit.
	@param wChannelConfig Describes the spatial location of the
		logical channels in the audio channel cluster of the
		Processing Unit.
	@param iChannelNames Index of a string descriptor that describes
		the name of the first logical channel in the
		audio channel cluster of the Processing Unit.
	@param bControlSize Size in bytes of the bmControls field
	@param bmControls A bit set to 1 indicates that the mentioned
		Control is supported for logical channel.
	@param iExtension Index of a string descriptor, describing
		this Processing Unit.
*/
#define AUDIO_EXTENSION_UNIT(bUnitID, wExtensionCode, baSourceID, bNrChannels, wChannelConfig, iChannelNames, bControlSize, bmControls, iExtension) \
        USB_SUBDESC_HEADER( 13+bLength(baSourceID)+bControlSize, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_EXTENSION_UNIT ), \
        bUnitID,                                                                \
        field16( wExtensionCode ),                                                \
        bLength( baSourceUD ),                                                        \
        bNrChannels,                                                                \
        field16( wChannelConfig ),                                                \
        iChannelNames,                                                                \
        bControlSize,                                                                \
        bmControls,                                                                \
        iExtension

/**
	Audio Stream Interface Descriptor

	@param bTerminalLink The Terminal ID of the Terminal to which
		the endpoint of this interface is connected.
	@param bDelay Delay (d) introduced by the data path. Expressed in number
		of frames.
	@param wFormatTag The Audio Data Format that has to be
		used to communicate with this interface.
*/
#define AUDIO_AUDIOSTREAM_INTERFACE(bTerminalLink, bDelay, wFormatTag)        \
        USB_SUBDESC_HEADER( 7, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_AUDIOSTREAM_GENERAL ), \
        bTerminalLink,                                                                \
        bDelay,                                                                        \
        field16( wFormatTag )

/**
	Standard AudioStream Isochronous Audio Data Endpoint Descriptor.

	@param bEndpointAddress Endpoint address, combination of endpoint index and
		#usb_endpoint_address_direction_e value.
	@param bmAttributes Endpoint attributes, combination of
		#usb_endpoint_attributes_transfer_e, #usb_endpoint_attributes_synchronisation_e
		and #usb_endpoint_attributes_usage_e.
	@param wMaxPacketSize Maximum packet size (+ additional transaction
		opportunities if #usb_endpoint_maxpacket_additional_e specified.
	@param bInterval Interval for polling endpoint for data transfers.
	@param bSynchAddress The address of the endpoint used to
		communicate synchronization information
		if required by this endpoint. Reset to zero
		if no synchronization pipe is used.
*/
#define AUDIO_AUDIOSTREAM_DATA_ENDPOINT(bEndpointAddress, bmAttributes, wMaxPacketSize, bInterval, bSynchAddress) \
        USB_DESC_HEADER( 9, USB_DESC_ENDPOINT ),                                \
        bEndpointAddress,                                                        \
        bmAttributes,                                                                \
        field16( wMaxPacketSize ),                                                \
        bInterval,                                                                \
        bRefresh(0),                                                                        \
        bSynchAddress

/**
	Class-Specific AS Isochronous Audio Data Endpoint Descriptor

	@param bmAttributes Endpoint attributes, combination of
		#audio_iso_data_attribute_e values.
	@param bLockDelayUnits Indicates the units used for the
		wLockDelay field, one of #audio_lock_delay_unit_e values.
	@param wLockDelay Indicates the time it takes this endpoint
		to reliably lock its internal clock recovery
		circuitry. Units used depend on the value
		of the bLockDelayUnits field.
*/
#define AUDIO_AUDIOSTREAM_DATA_CSENDPOINT(bmAttributes, bLockDelayUnits, wLockDelay) \
        USB_SUBDESC_HEADER( 7, USB_DESC_CSENDPOINT, AUDIO_SUBTYPE_EP_GENERAL ),        \
        bmAttributes,                                                                \
        bLockDelayUnits,                                                        \
        field16( wLockDelay )

/**
	Standard AudioStream Isochronous Synch Endpoint Descriptor.

	@param bEndpointAddress Endpoint address, combination of endpoint index and
		#usb_endpoint_address_direction_e value.
	@param bmAttributes Endpoint attributes, combination of
		#usb_endpoint_attributes_transfer_e, #usb_endpoint_attributes_synchronisation_e
		and #usb_endpoint_attributes_usage_e.
	@param wMaxPacketSize Maximum packet size (+ additional transaction
		opportunities if #usb_endpoint_maxpacket_additional_e specified.
	@param bRefresh This field indicates the rate at which an
		isochronous synchronization pipe
		provides new synchronization feedback
		data. This rate must be a power of 2,
		therefore only the power is reported back
		and the range of this field is from 1
		(2 ms) to 9 (512 ms).
*/
#define AUDIO_AUDIOSTREAM_SYNCH_ENDPOINT(bEndpointAddress, bmAttributes, wMaxPacketSize, bRefresh) \
        USB_DESC_HEADER( 9, USB_DESC_ENDPOINT ),                                \
        bEndpointAddress,                                                        \
        bmAttributes,                                                                \
        field16( wMaxPacketSize ),                                                \
        bInterval(1),                                                                        \
        bRefresh,                                                                \
        bSynchAddress(0)


/**
	Type I Format Type Descriptor

	@param bNrChannels Indicates the number of physical
		channels in the audio data stream.
	@param bSubFrameSize The number of bytes occupied by one
		audio subframe. Can be 1, 2, 3 or 4.
	@param bBitResolution The number of effectively used bits from
		the available bits in an audio subframe.
	@param bSamFreqType Indicates how the sampling frequency
		can be programmed. 0 - continuous, 1..255 - discrete
	@param ... Sampling frequency table, array of field24 values.
		For the continuous mode lowest and highest sampling
		frequency supported.
*/
#define AUDIO_FORMAT_TYPEI(bNrChannels, bSubFrameSize, bBitResolution, bSamFreqType, ...) \
        USB_SUBDESC_HEADER( 8+bLength(__VA_ARGS__), USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_FORMAT_TYPE ), \
        bFormatType( AUDIO_FORMAT_TYPE_I ),                                        \
        bNrChannels,                                                                \
        bSubframeSize,                                                                \
        bBitResolution,                                                                \
        bSamFreqType,                                                                \
        __VA_ARGS__

/**
	Type II Format Type Descriptor

	@param wMaxBitRate Indicates the maximum number of bits
		per second this interface can handle. Expressed in kbits/s
	@param wSamplesPerFrame Indicates the number of PCM audio
		samples contained in one encoded audio frame.
	@param bSamFreqType Indicates how the sampling frequency
		can be programmed. 0 - continuous, 1..255 - discrete
	@param ... Sampling frequency table, array of field24 values.
		For the continuous mode lowest and highest sampling
		frequency supported.
*/
#define AUDIO_FORMAT_TYPEII(wMaxBitRate, wSamplesPerFrame, bSamFreqType, ...) \
        USB_SUBDESC_HEADER( 9+bLength(__VA_ARGS__), USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_FORMAT_TYPE ), \
        bFormatType( AUDIO_FORMAT_TYPE_II ),                                        \
        field16( wMaxBitRate ),                                                        \
        field16( wSamplesPerFrame ),                                                \
        bSamFreqType,                                                                \
        __VA_ARGS__

/**
	Type III Format Type Descriptor

	@param bNrChannels Indicates the number of physical
		channels in the audio data stream.
	@param bSubFrameSize The number of bytes occupied by one
		audio subframe. Can be 1, 2, 3 or 4.
	@param bBitResolution The number of effectively used bits from
		the available bits in an audio subframe.
	@param bSamFreqType Indicates how the sampling frequency
		can be programmed. 0 - continuous, 1..255 - discrete
	@param ... Sampling frequency table, array of field24 values.
		For the continuous mode lowest and highest sampling
		frequency supported.
*/
#define AUDIO_FORMAT_TYPEIII(bNrChannels, bSubFrameSize, bBitResolution, bSamFreqType, ...) \
        USB_SUBDESC_HEADER( 8+bLength(__VA_ARGS__), USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_FORMAT_TYPE ), \
        bFormatType( AUDIO_FORMAT_TYPE_III ),                                        \
        bNrChannels,                                                                \
        bSubframeSize,                                                                \
        bBitResolution,                                                                \
        bSamFreqType,                                                                \
        __VA_ARGS__

/**
	Class Specific MIDIStream Interface Header

	@param ... Comma separated Jack and Element descriptors
*/
#define AUDIO_MIDISTREAM_HEADER(...)                                                \
        USB_SUBDESC_HEADER( 7, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_MIDI_HEADER ),        \
        field16( AUDIO_VERSION ),                                                \
        wTotalLength( 0, __VA_ARGS__ ),                                                \
        __VA_ARGS__

/**
	MIDI In Jack Descriptor

	@param bJackType EMBEDDED or EXTERNAL
	@param bJackID Constant uniquely identifying the MIDI IN
		Jack within the USB-MIDI function.
	@param iJack Index of a string descriptor, describing
		the MIDI IN Jack.
*/
#define AUDIO_MIDI_IN_JACK(bJackType, bJackID, iJack)                                \
        USB_SUBDESC_HEADER( 6, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_MIDI_IN_JACK ), \
        bJackType,                                                                \
        bJackID,                                                                \
        iJack

/**
	MIDI Out Jack Descriptor

	@param bJackType EMBEDDED or EXTERNAL
	@param bJackID Constant uniquely identifying the MIDI
		OUT Jack within the USB-MIDI function.
	@param baSourceIDPin IDs of the Entity to which the Input Pins
		of this MIDI OUT Jack is connected interleaved with
		Output Pin numbers of the Entity to which
		that Input Pin of this MIDI OUT Jack is
		connected.
	@param iJack Index of a string descriptor, describing
		the MIDI IN Jack.
*/
#define AUDIO_MIDI_OUT_JACK(bJackType, bJackID, baSourceIDPin, iJack)                \
        USB_SUBDESC_HEADER( 7+bLength( baSourceIDPin ), USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_MIDI_OUT_JACK ), \
        bJackType,                                                                \
        bJackID,                                                                \
        bLength( baSourceIDPin )/2,                                                \
        baSourceIDPin,                                                                \
        iJack

/**
	MIDI Element Descriptor

	@param bElementID Constant uniquely identifying the MIDI element.
	@param baSourceIDPin IDs of the Entity to which the Input Pins
		of this Element is connected interleaved with
		Output Pin numbers of the Entity to which
		that Input Pin of this Element is
		connected.
	@param bNrOutPins Number of Output Pins of this Element.
	@param bInTerminalLink The Terminal ID of the Input Terminal to
		which this Element is connected.
	@param bOutTerminalLink The Terminal ID of the Output Terminal to
		which this Element is connected.
	@param bElCapsSize Size, in bytes of the bmElementCaps field.
	@param bmElementCaps Element capabilities.
	@param iElement Index of a string descriptor, describing the Element.
*/
#define AUDIO_MIDI_ELEMENT(bElementID, baSourceIDPin, bNrOutPins, bInTerminalLink, bOutTerminalLink, bElCapsSize, bmElementCaps, iElement) \
        USB_SUBDESC_HEADER( 10 + bLength( baSourceIDPin )+bElCapsSize, USB_DESC_CSINTERFACE, AUDIO_SUBTYPE_MIDI_ELEMENT ), \
        bElementID,                                                                \
        bLength( baSourceIDPin )/2,                                                \
        bNrOutPins,                                                                \
        bInTerminalLink,                                                        \
        bOutTerminalLink,                                                        \
        bElCapsSize,                                                                \
        bmElementCaps,                                                                \
        iElement

/**
	Standard MIDIStream Bulk Data Endpoint Descriptor.

	@param bEndpointAddress Endpoint address, combination of endpoint index and
		#usb_endpoint_address_direction_e value.
	@param wMaxPacketSize Maximum packet size (+ additional transaction
		opportunities if #usb_endpoint_maxpacket_additional_e specified.
*/
#define AUDIO_MIDISTREAM_DATA_ENDPOINT(bEndpointAddress, wMaxPacketSize) \
        USB_DESC_HEADER( 9, USB_DESC_ENDPOINT ),                                \
        bEndpointAddress,                                                        \
        bmAttributes( USB_ENDPOINT_BULK ),                                        \
        field16( wMaxPacketSize ),                                                \
        bInterval(0),                                                                \
        bRefresh(0),                                                                \
        bSynchAddress(0)

/**
	Class-Specific MS Bulk Data Endpoint Descriptor

	@param baAssocJackID IDs of the Embedded Jacks that is associated with
		this endpoint.
*/
#define AUDIO_MIDISTREAM_DATA_CSENDPOINT(baAssocJackID) \
        USB_SUBDESC_HEADER( 4 + bLength( baAssocJackID ), USB_DESC_CSENDPOINT, AUDIO_SUBTYPE_EP_GENERAL ), \
        bLength( baAssocJackID ),                                                                \
        baAssocJackID


/// @}
///////////////////////////////////////////////////////////////////////////////

/**
	MIDI Element capabilities.
*/
enum audio_midi_element_capabilities_e {
    /**
	    The Element has unique, undefined features. A typical example
	    would be a unique synthesizer type or MIDI controlled audio
	    effects processor.
    */
	    AUDIO_MIDI_CAP_CUSTOM_UNDEFINED = 0x0001,

    /**
	    MIDI CLOCK messages are supported. Typical example Elements
	    include drum machines and MIDI CLOCK to FSK converters.
    */
	    AUDIO_MIDI_CAP_MIDI_CLOCK = 0x0002,

    /**
	    Synchronization features are supported. Typical example
	    Elements include MTC to SMPTE converters.
    */
	    AUDIO_MIDI_CAP_MTC = 0x0004,

    /**
	    MMC messages are supported.
    */
	    AUDIO_MIDI_CAP_MMC = 0x0008,

    /**
	    General MIDI System Level 1 compatibility as defined by the
	    MIDI Manufacturers Association.
    */
	    AUDIO_MIDI_CAP_GM1 = 0x0010,

    /**
	    General MIDI System Level 2 compatibility as defined by the
	    MIDI Manufacturers Association.
    */
	    AUDIO_MIDI_CAP_GM2 = 0x0020,

    /**
	    GS Format compatibility as defined by Roland Corporation.
    */
	    AUDIO_MIDI_CAP_GS = 0x0040,

    /**
	    XG compatibility as defined by Yamaha Corporation.
    */
	    AUDIO_MIDI_CAP_XG = 0x0080,

    /**
	    The Element provides an audio effects processor controlled by
	    USB.
    */
	    AUDIO_MIDI_CAP_EFX = 0x0100,

    /**
	    The Element provides an internal MIDI Patcher or Router.
    */
	    AUDIO_MIDI_CAP_MIDI_PATCH_BAY = 0x0200,

    /**
	    DownLoadable Sounds Standard Level 1 compatibility as defined
	    by the MIDI Manufacturers Association.
    */
	    AUDIO_MIDI_CAP_DLS1 = 0x0400,

    /**
	    DownLoadable Sounds Standard Level 2 compatibility as defined
	    by the MIDI Manufacturers Association.
    */
	    AUDIO_MIDI_CAP_DLS2 = 0x0800
};

/**
	Sampling frequency programming
*/
enum audio_sample_freq_programming_e {
    AUDIO_SAMPLE_FREQ_CONTINUOUS = 0        ///< Continuous sampling frequency
};

/**
	Isochronous data endpoint attributes
*/
enum audio_iso_data_attribute_e {
    AUDIO_ISO_SAMPLING_FREQUENCY = 0x01,        ///< Sampling frequency
    AUDIO_ISO_PITCH = 0x02,        ///< Pitch
    AUDIO_ISO_MAX_PACKETS_ONLY = 0x80        ///< Requires MaxPacketSize only
};

/**
	Lock delay units
*/
enum audio_lock_delay_unit_e {
    AUDIO_LOCK_UNDEFINED = 0,        ///< Undefined
    AUDIO_LOCK_MS = 1,        ///< Milliseconds
    AUDIO_LOCK_SAMPLES = 2        ///< Decoded PCM samples
};

/**
	Standard control codes
*/
enum audio_controls_e {
    AUDIO_ENABLE_PROCESSING = 0x00000000        ///< Enable processing
};

/**
	Interface descriptor subtypes.
*/
enum audio_ac_desc_subtype_e {
    AUDIO_SUBTYPE_HEADER = 0x01,                ///< Header
    AUDIO_SUBTYPE_INPUT_TERMINAL = 0x02,                ///< Input Terminal
    AUDIO_SUBTYPE_OUTPUT_TERMINAL = 0x03,                ///< Output Terminal
    AUDIO_SUBTYPE_MIXER_UNIT = 0x04,                ///< Mixer Unit
    AUDIO_SUBTYPE_SELECTOR_UNIT = 0x05,                ///< Selector Unit
    AUDIO_SUBTYPE_FEATURE_UNIT = 0x06,                ///< Feature Unit
    AUDIO_SUBTYPE_PROCESSING_UNIT = 0x07,                ///< Processing Unit
    AUDIO_SUBTYPE_EXTENSION_UNIT = 0x08,                ///< Extension Unit

    AUDIO_SUBTYPE_AUDIOSTREAM_GENERAL = 0x01,        ///< AudioStream General
    AUDIO_SUBTYPE_FORMAT_TYPE = 0x02,                ///< Format Type
    AUDIO_SUBTYPE_FORMAT_SPECIFIC = 0x03,                ///< Format Specific

    AUDIO_SUBTYPE_MIDI_HEADER = 0x01,                ///< MIDI Header
    AUDIO_SUBTYPE_MIDI_IN_JACK = 0x02,                ///< MIDI In Jack
    AUDIO_SUBTYPE_MIDI_OUT_JACK = 0x03,                ///< MIDI Out Jack
    AUDIO_SUBTYPE_MIDI_ELEMENT = 0x04                ///< MIDI Element
};

/**
	Processing unit process types.
*/
enum audio_process_e {
    AUDIO_UPDOWN_PROCESS = 0x01,                /// Up/Down-mix Process
    AUDIO_DOLBY_PROLOGIC_PROCESS = 0x02,                /// Dolby Prologic Process
    AUDIO_3D_STEREO_EXTENDER_PROCESS = 0x03,        /// 3D-Stereo Extender Process
    AUDIO_REVERBERATION_PROCESS = 0x04,                /// Reverb Process
    AUDIO_CHORUS_PROCESS = 0x05,                /// Chorus Process
    AUDIO_COMPRESSOR_PROCESS = 0x06                /// Compressor Process
};

/**
	Endpoint descriptor types.
*/
enum audio_endpoint_desc_e {
    AUDIO_SUBTYPE_EP_GENERAL = 0x01                ///< General Endpoint
};

/**
	MIDI Jack Types.
*/
enum audio_midi_jack_type_e {
    AUDIO_MIDI_EMBEDDED = 0x01,                ///< Embedded MIDI Jack
    AUDIO_MIDI_EXTERNAL = 0x02                ///< External MIDI Jack
};

/**
	Audio specific request codes.
*/
enum audio_req_e {
    AUDIO_REQ_SET_CUR = 0x01,
    AUDIO_REQ_GET_CUR = 0x81,
    AUDIO_REQ_SET_MIN = 0x02,
    AUDIO_REQ_GET_MIN = 0x82,
    AUDIO_REQ_SET_MAX = 0x03,
    AUDIO_REQ_GET_MAX = 0x83,
    AUDIO_REQ_SET_RES = 0x04,
    AUDIO_REQ_GET_RES = 0x84,
    AUDIO_REQ_SET_MEM = 0x05,
    AUDIO_REQ_GET_MEM = 0x85,
    AUDIO_REQ_GET_STAT = 0xff
};

/**
	Terminal control selectors.
*/
enum audio_terminal_e {
    AUDIO_TE_COPY_PROTECT_CONTROL = 0x01                ///< Copy Protect Control
};

/**
	Feature unit control selectors.
*/
enum audio_feature_e {
    AUDIO_FU_MUTE_CONTROL = 0x01,                ///< Mute Control
    AUDIO_FU_VOLUME_CONTROL = 0x02,                ///< Volume Control
    AUDIO_FU_BASS_CONTROL = 0x03,                ///< Bass Control
    AUDIO_FU_MID_CONTROL = 0x04,                ///< Mid Control
    AUDIO_FU_TREBLE_CONTROL = 0x05,                ///< Treble Control
    AUDIO_FU_GEQ_CONTROL = 0x06,                ///< GEQ Control
    AUDIO_FU_AGC_CONTROL = 0x07,                ///< AGC Control
    AUDIO_FU_DELAY_CONTROL = 0x08,                ///< Delay Control
    AUDIO_FU_BASS_BOOST_CONTROL = 0x09,                ///< Bass Boost Control
    AUDIO_FU_LOUDNESS_CONTROL = 0x0a                ///< Loudness Control
};

/**
	Up/down-mix processing unit control selectors.
*/
enum audio_updownmix_e {
    AUDIO_UD_ENABLE_CONTROL = 0x01,                ///< Enable
    AUDIO_UD_MODE_SELECT_CONTROL = 0x02                ///< Mode select
};

/**
	Dolby Prologic processing unit control selectors.
*/
enum audio_dolby_e {
    AUDIO_DP_ENABLE_CONTROL = 0x01,                ///< Enable
    AUDIO_DP_MODE_SELECT_CONTROL = 0x02                ///< Mode select
};

/**
	Stereo extender processing unit control selectors.

	@note Originally was AUDIO_3D_SPACIOUSNESS_CONTROL = 3, which is IMO
	a mistake.
*/
enum audio_3d_e {
    AUDIO_3D_ENABLE_CONTROL = 0x01,                ///< Enable
    AUDIO_3D_SPACIOUSNESS_CONTROL = 0x02                ///< Spaciousness
};

/**
	Reverberation processing unit control selectors.
*/
enum audio_reverberation_e {
    AUDIO_RV_ENABLE_CONTROL = 0x01,                ///< Enable
    AUDIO_RV_LEVEL_CONTROL = 0x02,                ///< Level
    AUDIO_RV_TIME_CONTROL = 0x03,                ///< Time
    AUDIO_RV_FEEDBACK_CONTROL = 0x04                ///< Feedback
};

/**
	Chorus processing unit control selectors.
*/
enum audio_chorus_e {
    AUDIO_CH_ENABLE_CONTROL = 0x01,                ///< Enable
    AUDIO_CH_LEVEL_CONTROL = 0x02,                ///< Level
    AUDIO_CH_RATE_CONTROL = 0x03,                ///< Rate
    AUDIO_CH_DEPTH_CONTROL = 0x04                ///< Depth
};

/**
	Dynamic range compressor processing unit control selectors.
*/
enum audio_compressor_e {
    AUDIO_DR_ENABLE_CONTROL = 0x01,                ///< Enable
    AUDIO_DR_RATIO_CONTROL = 0x02,                ///< Ratio
    AUDIO_DR_MAXAMPL_CONTROL = 0x03,                ///< MaxAmpl
    AUDIO_DR_THRESHOLD_CONTROL = 0x04,                ///< Threshold
    AUDIO_DR_ATTACK_TIME = 0x05,                ///< Attack
    AUDIO_DR_RELEASE_TIME = 0x06                ///< Release
};

/**
	Extension unit processing unit control selectors.
*/
enum audio_extension_e {
    AUDIO_XU_ENABLE_CONTROL = 0x01                ///< Enable
};

/**
	Endpoint control selectors.
*/
enum audio_endpoint_e {
    AUDIO_EP_SAMPLING_FREQ_CONTROL = 0x01,                ///< Sampling frequency
    AUDIO_EP_PITCH_CONTROL = 0x02                ///< Pitch
};

/**
	Audio Data Format Type I Codes
*/
enum audio_format_type_i_code_e {
    AUDIO_TYPEI_PCM = 0x0001,        ///< PCM
    AUDIO_TYPEI_PCM8 = 0x0002,        ///< PCM8
    AUDIO_TYPEI_IEEE_FLOAT = 0x0003,        ///< IEEE float
    AUDIO_TYPEI_ALAW = 0x0004,        ///< aLaw
    AUDIO_TYPEI_MULAW = 0x0005        ///< uLaw
};

/**
	Audio Data Format Type II Codes
*/
enum audio_format_type_ii_code_e {
    AUDIO_TYPEII_MPEG = 0x1001,        ///< MPEG
    AUDIO_TYPEII_AC3 = 0x1002        ///< AC-3
};

/**
	Audio Data Format Type III Codes
*/
enum audio_format_type_iii_code_e {
    AUDIO_TYPEIII_IEC1937_AC3 = 0x2001,        ///< IEC1937 AC3
    AUDIO_TYPEIII_IEC1937_MPEG1_LAYER1 = 0x2002,        ///< MPEG-1 Layer 1
    AUDIO_TYPEIII_IEC1937_MPEG1_LAYER23 = 0x2003,        ///< MPEG-1 Layer 2/3
    AUDIO_TYPEIII_IEC1937_MPEG2_NOEXT = 0x2003,        ///< MPEG-2 NoExt
    AUDIO_TYPEIII_IEC1937_MPEG2_EXT = 0x2004,        ///< MPEG-2 Ext
    AUDIO_TYPEIII_IEC1937_MPEG2_LAYER1_LS = 0x2005,        ///< MPEG-2 Layer 1 LS
    AUDIO_TYPEIII_IEC1937_MPEG2_LAYER23_LS = 0x2006,        ///< MPEG-2 Layer 2/3 LS
};

/**
	Audio Format Type Codes
*/
enum audio_format_type_code_e {
    AUDIO_FORMAT_TYPE_I = 0x01,                ///< Format Type I
    AUDIO_FORMAT_TYPE_II = 0x02,                ///< Format Type II
    AUDIO_FORMAT_TYPE_III = 0x03                ///< Format Type III
};

/**
	MPEG Control Selectors
*/
enum audio_mpeg_e {
    AUDIO_MPEG_DUAL_CHANNEL_CONTROL = 0x01,        ///< Dual Channel
    AUDIO_MPEG_SECOND_STEREO_CONTROL = 0x02,        ///< Second Stereo
    AUDIO_MPEG_MULTILINGUAL_CONTROL = 0x03,        ///< Multilingual
    AUDIO_MPEG_DYN_RANGE_CONTROL = 0x04,        ///< Dynamic Range
    AUDIO_MPEG_SCALING_CONTROL = 0x05,        ///< Scaling
    AUDIO_MPEG_HILO_SCALING_CONTROL = 0x06 ///< Hi/Lo Scaling
};

/**
	AC-3 Control Selectors
*/
enum audio_ac3_e {
    AUDIO_AC3_MODE_CONTROL = 0x01,        ///< Mode
    AUDIO_AC3_DYN_RANGE_CONTROL = 0x02,        ///< Dynamic Range
    AUDIO_AC3_SCALING_CONTROL = 0x03,        ///< Scaling
    AUDIO_AC3_HILO_SCALING_CONTROL = 0x04        ///< Hi/Lo Scaling
};

/**
	MIDI Endpoint Control Selectors
*/
enum audio_midi_endpoint_e {
    AUDIO_MIDI_ASSOCIATION_CONTROL = 0x01 ///< Association Control
};

/**
	Terminal Types
*/
enum audio_terminal_type_e {
    /**
	    A Terminal dealing with a signal carried over an endpoint in an
	    AudioStreaming interface. The AudioStreaming interface
	    descriptor points to the associated Terminal through the
	    bTerminalLink field.
    */
	    AUDIO_TERM_USB_STREAMING = 0x0101,

    /**
	    A Terminal dealing with a signal carried over a vendor-specific
	    interface. The vendor-specific interface descriptor must
	    contain a field that references the Terminal.
    */
	    AUDIO_TERM_USB_VENDOR_SPECIFIC = 0x01ff,

    /**
	    A generic microphone that does not fit under any of the other
	    classifications.
    */
	    AUDIO_TERM_IN_MICROPHONE = 0x201,

    /**
	    A microphone normally placed on the desktop or integrated into
	    the monitor.
    */
	    AUDIO_TERM_IN_DESKTOP_MICROPHONE = 0x0202,

    /**
	    A head-mounted or clip-on microphone.
    */
	    AUDIO_TERM_IN_PERSONAL_MICROPHONE = 0x0203,

    /**
	    A microphone designed to pick up voice from more than one
	    speaker at relatively long ranges.
    */
	    AUDIO_TERM_IN_OMNIDIRECTIONAL_MICROPHONE = 0x0204,

    /**
	    An array of microphones designed for directional processing
	    using host-based signal processing algorithms.
    */
	    AUDIO_TERM_IN_MICROPHONE_ARRAY = 0x0205,

    /**
	    An array of microphones with an embedded signal processor.
    */
	    AUDIO_TERM_IN_PROCESSING_MICROPHONE_ARRAY = 0x0206,

    /**
	    A generic speaker or set of speakers that does not fit under
	    any of the other classifications.
    */
	    AUDIO_TERM_OUT_SPEAKER = 0x0301,

    /**
	    A head-mounted audio output device.
    */
	    AUDIO_TERM_OUT_HEADPHONES = 0x0302,

    /**
	    The audio part of a VR head mounted display. The Associated
	    Interfaces descriptor can be used to reference the HID
	    interface used to report the position and orientation of the
	    HMD.
    */
	    AUDIO_TERM_OUT_HMD_AUDIO = 0x0303,

    /**
	    Relatively small speaker or set of speakers normally placed on
	    the desktop or integrated into the monitor. These speakers are
	    close to the user and have limited stereo separation.
    */
	    AUDIO_TERM_OUT_DESKTOP_SPEAKER = 0x0304,

    /**
	    Larger speaker or set of speakers that are heard well anywhere
	    in the room.
    */
	    AUDIO_TERM_OUT_ROOM_SPEAKER = 0x0305,

    /**
	    Speaker or set of speakers designed for voice communication.
    */
	    AUDIO_TERM_OUT_COMMUNICATION_SPEAKER = 0x0306,

    /**
	    Speaker designed for low frequencies (subwoofer). Not capable
	    of reproducing speech or music.
    */
	    AUDIO_TERM_OUT_LFE_SPEAKER = 0x0307,

    /**
	    Hand-held bi-directional audio device.
    */
	    AUDIO_TERM_IO_HANDSET = 0x0401,

    /**
	    Head-mounted bi-directional audio device.
    */
	    AUDIO_TERM_IO_HEADSET = 0x0402,

    /**
	    A hands-free audio device designed for host-based echo
	    cancellation.
    */
	    AUDIO_TERM_IO_SPEAKERPHONE = 0x0403,

    /**
	    A hands-free audio device with echo suppression capable of
	    half-duplex operation.
    */
	    AUDIO_TERM_IO_SPEAKERPHONE_SUPPRESSING = 0x0404,

    /**
	    A hands-free audio device with echo cancellation capable of
	    full-duplex operation.
    */
	    AUDIO_TERM_IO_SPEAKERPHONE_CANCELLING = 0x0405,

    /**
	    May be an analog telephone line jack, an ISDN line, a
	    proprietary PBX interface, or a wireless link.
    */
	    AUDIO_TERM_TEL_PHONE_LINE = 0x0501,

    /**
	    Device can be used as a telephone. When not in use as a
	    telephone, handset is used as a bi-directional audio device.
    */
	    AUDIO_TERM_TEL_TELEPHONE = 0x0502,

    /**
	    A standard telephone set connected to the device. When not in
	    use as a telephone, it can be used as a bi-directional audio
	    device.
    */
	    AUDIO_TERM_TEL_DOWN_LINE_PHONE = 0x0503,


    /**
	    A generic analog connector.
    */
	    AUDIO_TERM_EXT_ANALOG_CONNECTOR = 0x0601,

    /**
	    A generic digital audio interface.
    */
	    AUDIO_TERM_EXT_DIGITAL_AUDIO_INTERFACE = 0x0602,

    /**
	    An analog connector at standard line levels. Usually uses
	    3.5mm.
    */
	    AUDIO_TERM_EXT_LINE_CONNECTOR = 0x0603,

    /**
	    An input connector assumed to be connected to the lineout of
	    the legacy audio system of the host computer. Used for backward
	    compatibility.
    */
	    AUDIO_TERM_EXT_LEGACY_AUDIO_CONNECTOR = 0x0604,

    /**
	    An S/PDIF digital audio interface. The Associated Interface
	    descriptor can be used to reference an interface used for
	    controlling special functions of this interface.
    */
	    AUDIO_TERM_EXT_SPDIF_INTERFACE = 0x0605,

    /**
	    An interface to audio streams on a 1394 bus.
    */
	    AUDIO_TERM_EXT_1394_DA_STREAM = 0x0606,

    /**
	    An interface to soundtrack of A/V stream on a 1394 bus.
    */
	    AUDIO_TERM_EXT_1394_DV_STREAM_SOUNDTRACK = 0x0607,

    /**
	    Internal Noise source for level calibration (MPEG decoding,
	    Dolby Prologic‘, AC-3 etc.)
    */
	    AUDIO_TERM_INT_OUT_LEVEL_CALIBRATION_SOURCE = 0x0701,

    /**
	    Internal Noise source for measurements.
    */
	    AUDIO_TERM_INT_OUT_NOISE_SOURCE = 0x0702,

    /**
	    Audio compact disc player or CD-ROM capable of audio playback.
    */
	    AUDIO_TERM_INT_IN_CD_PLAYER = 0x0703,

    /**
	    Digital Audio Tape.
    */
	    AUDIO_TERM_INT_IO_DAT = 0x0704,

    /**
	    Digital Compact Casette.
    */
	    AUDIO_TERM_INT_IO_DCC = 0x0705,

    /**
	    Minidisk.
    */
	    AUDIO_TERM_INT_IO_MINIDISK = 0x0706,

    /**
	    Analog Audio Tape.
    */
	    AUDIO_TERM_INT_IO_ANALOG_TAPE = 0x0707,

    /**
	    Analog vinyl record player.
    */
	    AUDIO_TERM_INT_IN_PHONOGRAPH = 0x0708,

    /**
	    Audio track of VCR.
    */
	    AUDIO_TERM_INT_IN_VCR_AUDIO = 0x0709,

    /**
	    Audio track of VideoDisc player.
    */
	    AUDIO_TERM_INT_IN_VIDEO_DISC_AUDIO = 0x070a,

    /**
	    Audio track of DVD player.
    */
	    AUDIO_TERM_INT_IN_DVD_AUDIO = 0x070b,

    /**
	    Audio track of TV tuner.
    */
	    AUDIO_TERM_INT_IN_TV_TUNER_AUDIO = 0x070c,

    /**
	    Audio track of satellite receiver.
    */
	    AUDIO_TERM_INT_IN_SATELLITE_RECEIVER_AUDIO = 0x070d,

    /**
	    Audio track of cable tuner.
    */
	    AUDIO_TERM_INT_IN_CABLE_TUNER_AUDIO = 0x070e,

    /**
	    Audio track of DSS receiver.
    */
	    AUDIO_TERM_INT_IN_DSS_AUDIO = 0x070f,

    /**
	    AM/FM radio receiver.
    */
	    AUDIO_TERM_INT_IN_RADIO_RECEIVER = 0x0710,

    /**
	    AM/FM radio transmitter.
    */
	    AUDIO_TERM_INT_OUT_RADIO_TRANSMITTER = 0x0711,

    /**
	    A multi-track recording system.
    */
	    AUDIO_TERM_INT_IO_MULTITRACK_RECORDER = 0x0712,

    /**
	    Synthesizer.
    */
	    AUDIO_TERM_INT_IN_SYNTHESIZER = 0x0713
};

/**
	Standard Audio callbacks.
*/
#define INTERFACE_AUDIO INTERFACE( usb_audio_initcallback, usb_audio_deinitcallback, usb_audio_cmdcallback )


/**
	Callback for initializing CDC related systems.
*/
void usb_audio_initcallback();

/**
	Callback for deinitializing CDC related systems.
*/
void usb_audio_deinitcallback();

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
uint16_t usb_audio_cmdcallback(uint8_t bmRequestType, uint8_t bRequest,
			       uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t *data);


#endif // CLASS_AUDIO_H

/// @}
/// @}
///////////////////////////////////////////////////////////////////////////////
