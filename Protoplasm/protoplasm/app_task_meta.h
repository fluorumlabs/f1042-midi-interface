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
/// @addtogroup hilevel_ui		User Interface
/// @{
/// @defgroup hilevel_ui_meta		META Programming
/// @{

#ifndef APP_TASK_META_H
#define APP_TASK_META_H

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name UI State Machine
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Declare main UI state machine.

	This is the function, in which all UI event handling is implemented.

	@note There can be only one #UI.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		UI {
		STARTUP:
			STATE_BEGIN
				infinite_loop;
			STATE_END
		}
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define APPLICATION void _app_loop()

/**
	Define entry point for UI state machine.

	@note Should be the first state in #UI.
	@note To be used only within #UI declaration.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		UI {
		STARTUP:
			STATE_BEGIN
				infinite_loop;
			STATE_END
		}
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define STARTUP goto __startup; __startup

/**
	Begin state event handling implementation.

	All events are processed between #STATE_BEGIN and #STATE_END

	@note To be used only within #UI declaration.
	@note Each #STATE_BEGIN should have corresponding #STATE_END

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		UI {
		STARTUP:
			STATE_BEGIN
				infinite_loop;
			STATE_END
		}
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define STATE_BEGIN                                                                 \
        if(1) {                                                                        \
                app_event_t event;                                                \
                event = APP_EVENT_MASK;                                                \
                infinite_loop {                                                        \
                        switch ( event & APP_EVENT_MASK )                        \
                        {

/**
	End state event handling implementation.

	@note To be used only within #UI declaration.
	@note Each #STATE_END should have corresponding #STATE_BEGIN

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		UI {
		STARTUP:
			STATE_BEGIN
				infinite_loop;
			STATE_END
		}
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define STATE_END                                                                 \
        default: break;} do { event = app_popevent(); } while(event==APP_EVENT_NONE); } }                               \

/**
	Define handler for an event.

	@param _event Event type, `BUTTON`, `ENCODER`, `TIMEOUT`

	@note To be used only between #STATE_BEGIN and #STATE_END.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		EDIT:
			STATE_BEGIN
				ui_marker_brackets();
				ui_item_paint();
			EVENT( ENCODER ):
				ui_field_edit( event );
				ui_item_paint();
			EVENT( BUTTON ):
				if( EVENT_BUTTON( ENCODER,DOWN ) ) SWITCHTO( NAVIGATE );
			STATE_END
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define EVENT(_event)                        \
        case _event

/**
	Define handler for an event.

	@param _event Event type, `BUTTON`, `ENCODER`, `TIMEOUT`

	@note To be used only between #STATE_BEGIN and #STATE_END.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		EDIT:
			STATE_BEGIN
				ui_marker_brackets();
				ui_item_paint();
			EVENT( ENCODER ):
				ui_field_edit( event );
				ui_item_paint();
			EVENT( BUTTON ):
				if( EVENT_BUTTON( ENCODER,DOWN ) ) SWITCHTO( NAVIGATE );
			STATE_END
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define ENTER                        \
        case APP_EVENT_MASK

/**
	Define a handler for any event except timeout.

	@note To be used only between #STATE_BEGIN and #STATE_END.
	@note Can't be used with #EVENT( BUTTON ) or #EVENT( ENCODER )

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		NAVIGATE_IDLE:
			STATE_BEGIN
				backlight_set( PWM_BACKLIGHT_MAX/10 );
			ANYEVENT:
				TRANSLATETO( NAVIGATE );
			STATE_END
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define ANYEVENT case 0

/**
	Switch to another state.

	@param ctx New state name

	@note To be used only between #STATE_BEGIN and #STATE_END.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		EVENT( BUTTON ):
			if( EVENT_BUTTON( ENCODER,DOWN ) ) SWITCHTO( NAVIGATE );
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define SWITCHTO(ctx) do { goto ctx; } while(0)

/**
	Switch to another state and translate current event.

	@note To be used only between #STATE_BEGIN and #STATE_END.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		ANYEVENT:
			TRANSLATETO( NAVIGATE );
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define TRANSLATETO(ctx) do { app_pusheventback(); goto ctx; } while(0)

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name UI State Machine Helpers
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Helper expression for event value.

	@note To be used only between #STATE_BEGIN and #STATE_END.
*/
#define EVENT_VALUE (event & APP_EVENT_MASK_VALUE)

/// @}
///////////////////////////////////////////////////////////////////////////////

#else

#undef APPLICATION
#undef STARTUP
#undef STATE_BEGIN
#undef EVENT
#undef ANYEVENT
#undef SWITCHTO
#undef TRANSLATETO
#undef STATE_END
#undef EVENT_VALUE

#endif // APP_TASK_META_H
/// @}
/// @}
/// @}
///////////////////////////////////////////////////////////////////////////////
