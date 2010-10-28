// Copyright 2010 apanloco
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

//#define ENABLE_LOG

void log_init(const char *host, unsigned short port);
void log(const char *format, ...);
void log_close();

extern unsigned int log_counter;

#ifdef ENABLE_LOG
#define LOG(format, ...) log("[%lu] %s> " format "\n", log_counter, __func__, ## __VA_ARGS__); log_counter++;
#else
#define LOG(format, ...)
#endif
