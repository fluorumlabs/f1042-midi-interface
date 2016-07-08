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

#include <defines.h>
#include <app_task.h>

// Internal stuff

void _app_loop() noreturn;

static volatile app_event_t _app_queue[APP_EVENT_QUEUE_SIZE];
static volatile int _app_queue_readpos;
static volatile int _app_queue_writepos;

void application_run() {
	_app_loop();
}


void app_pushevent(app_event_t e) {
	_app_queue[_app_queue_writepos++] = e;
	if (_app_queue_writepos >= APP_EVENT_QUEUE_SIZE) _app_queue_writepos = 0;
	if (_app_queue_readpos == _app_queue_writepos) _app_queue_readpos++;
	if (_app_queue_readpos >= APP_EVENT_QUEUE_SIZE) _app_queue_readpos = 0;
}


app_event_t app_popevent() {
	app_event_t e = APP_EVENT_NONE;

	if (_app_queue_readpos != _app_queue_writepos)
		e =
			_app_queue[_app_queue_readpos++];
	else e = APP_EVENT_NONE;

	if (e != APP_EVENT_NONE && _app_queue_readpos >= APP_EVENT_QUEUE_SIZE)
		_app_queue_readpos = 0;

	return e;
}

void app_clearevents() {
	_app_queue_readpos = _app_queue_writepos;
}


void ui_pusheventback() {
	_app_queue_readpos--;
	if (_app_queue_readpos == 0) _app_queue_readpos = APP_EVENT_QUEUE_SIZE - 1;
}

