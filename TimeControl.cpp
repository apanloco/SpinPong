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

#include "TimeControl.h"

#include "FWTime.h"

uint32_t TimeControl::get_ticks()
{
  return (uint32_t) ((float) FWTime::getCurrentTime() * 1000.0f);
}

void TimeControl::reset(int interval) 
{ 
  _interval = interval;

  _total = 0;
  _left = 0;

  _frame = -1;

  _time0 = get_ticks();
  _time1 = _time0;
}

void TimeControl::measure() 
{
  _time1 = get_ticks();
  _total = (_time1 - _time0) + _left;
  _time0 = _time1;
  _left = 0;

  ++_frame;
}

bool TimeControl::step() 
{
  if(_total < _interval)
  {
    _left = _total;
    return false;
  }

  _total -= _interval;

  return true;
}
