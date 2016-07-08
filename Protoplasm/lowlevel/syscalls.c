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

#include <util.h>
#include <defines.h>

#include <sys/stat.h>
#include <sys/types.h>

void _exit(int status) {
	unused status;
	Error_Handler();
	infinite_loop;
}

pid_t _getpid(void) {
	return 1;
}

int _kill(int pid, int sig) {
	unused pid;
	unused sig;

	Error_Handler();
	return -1;
}

