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
/// @file				ui_task.h
///
/// @addtogroup hilevel			High-Level API
/// @{
/// @defgroup hilevel_ui		User Interface
/// @{

#ifndef APP_TASK_H
#define APP_TASK_H

#include <defines.h>

///////////////////////////////////////////////////////////////////////////////
/// @name Configuration
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Size of event queue.
*/
#define APP_EVENT_QUEUE_SIZE 16

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name Events
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Event button state flags.
*/
enum app_event_button_state_e {
    APP_EVENT_DOWN = 0x00010000,        ///< Button is pressed.
    APP_EVENT_UP = 0x00000000                ///< Button is not pressed.
};

/**
	Event masks.
*/
enum app_event_mask_e {
    APP_EVENT_MASK = 0xff000000,                ///< Event type mask.
    APP_EVENT_MASK_VALUE = 0x00ffffff        ///< Event value mask.
};

/**
	Event types.
*/
enum app_event_type_e {
    APP_EVENT_ANY = 0x3f000000,        ///< Any event except #UI_EVENT_NONE and #UI_EVENT_TIMEOUT.
    APP_EVENT_NONE = 0x80000000,        ///< No event.
};

/**
	This type represents UI event.
*/
typedef uint32_t app_event_t;

/**
 	Main application loop
 */
void application_run() noreturn;

/**
	Push new event to event queue.

	@param [in] e Event
*/
void app_pushevent(app_event_t e);

/**
	Push recently popped event back to queue.
*/
void app_pusheventback();

/**
	Pop and remove event from queue.

	@return Popped event.
*/
app_event_t app_popevent();

/**
	Clear event queue.
*/
void app_clearevents();

/// @}
///////////////////////////////////////////////////////////////////////////////

#endif // APP_TASK_H

/// @}
/// @}
///////////////////////////////////////////////////////////////////////////////
