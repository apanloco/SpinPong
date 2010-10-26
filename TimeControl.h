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

#include <stdint.h>

class TimeControl
{
public:
  TimeControl() { }
  virtual ~TimeControl() { }

  void reset(int interval); // in ms
  void measure();
  bool step();

private:
  uint32_t get_ticks();

  uint32_t _frame;  
  uint32_t _interval;
  uint32_t _left;
  uint32_t _total;
  uint32_t _time0;
  uint32_t _time1;
};
