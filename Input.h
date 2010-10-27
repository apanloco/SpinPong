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

#include "FWInput.h"

#define MAX_PAD (2)
#define MAX_INPUT_EVENTS (11)

class InputEvent
{
public:
  enum InputEventType
  {
    QUIT,
    RELOAD,
    P1_ACC,
    P1_READY,
    P1_NOT_READY,
    P1_RESET,
    P1_NOT_RESET,
    P2_ACC,
    P2_READY,
    P2_NOT_READY,
    P2_RESET,
    P2_NOT_RESET,
    PAUSE,
    ON_SCREEN_LOG_ON,
    ON_SCREEN_LOG_OFF
  };

  InputEventType type;
  float acc;
};

class Input
{
public:
  Input() { }
  virtual ~Input() { }

  bool reset();
  bool teardown();

  InputEvent *get_events(int *numEvents);

private:
  void addEvent(InputEvent *e, int *numEvents);
  InputEvent _events[MAX_INPUT_EVENTS];
  FWInputFilter  *_p1Y0;
  FWInputFilter  *_p2Y0;
  FWInputFilter  *_p1Cross;
  FWInputFilter  *_p1Triangle;
  FWInputFilter  *_p1Start;
  FWInputFilter  *_p2Cross;
  FWInputFilter  *_p2Start;
};
