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

#include "Input.h"
#include "Log.h"

#include <string.h>

bool Input::reset()
{
  FWInputDevice *pad1 = FWInput::getDevice(FWInput::DeviceType_Pad, 0);

  if(pad1 == NULL) {
    LOG("WARNING: Could not get pad 1");
    _p1Y0 = NULL; 
    _p1Cross = NULL;
  } else {
    _p1Y0 = pad1->bindFilter();
    _p1Y0->setChannel(FWInput::Channel_YAxis_0);
    _p1Y0->setDeadzone(0.1f);
    _p1Cross = pad1->bindFilter();
    _p1Cross->setChannel(FWInput::Channel_Button_Cross);
    _p1Triangle = pad1->bindFilter();
    _p1Triangle->setChannel(FWInput::Channel_Button_Triangle);
  }

  FWInputDevice *pad2 = FWInput::getDevice(FWInput::DeviceType_Pad, 1);

  if(pad2 == NULL) {
    LOG("WARNING: Could not get pad 2");
    _p2Y0 = NULL; 
    _p2Cross = NULL;
  } else {
    _p2Y0 = pad2->bindFilter();
    _p2Y0->setChannel(FWInput::Channel_YAxis_0);
    _p2Y0->setDeadzone(0.1f);
    _p2Cross = pad2->bindFilter();
    _p2Cross->setChannel(FWInput::Channel_Button_Cross);
  }

  return true;
}

bool Input::teardown()
{
  FWInputDevice  *pad1 = FWInput::getDevice(FWInput::DeviceType_Pad, 0);

  if(pad1 != NULL) {
    if(_p1Y0 != NULL) {
      pad1->unbindFilter(_p1Y0);
    }
    if(_p1Cross != NULL) {
      pad1->unbindFilter(_p1Cross);
    }
    if(_p1Triangle != NULL) {
      pad1->unbindFilter(_p1Triangle);
    }
  }

  _p1Y0 = NULL;
  _p1Cross = NULL;
  _p1Triangle = NULL;

  FWInputDevice  *pad2 = FWInput::getDevice(FWInput::DeviceType_Pad, 1);

  if(pad2 != NULL) {
    if(_p2Y0 != NULL) {
      pad2->unbindFilter(_p2Y0);
    }
    if(_p2Cross != NULL) {
      pad2->unbindFilter(_p2Cross);
    }
  }
  
  _p2Y0 = NULL;
  _p2Cross = NULL;
  
  return true;
}

InputEvent *Input::get_events(int *numEvents)
{
  InputEvent e;
  float val;
  *numEvents = 0;
 
  // ACCELERATION

  if(_p1Y0 != NULL) {
    static float oldValue = 0.f;
    val = _p1Y0->getFloatValue();
    if(val != oldValue) {
      e.type = InputEvent::P1_ACC;
      e.acc = val;
      addEvent(&e, numEvents);
      oldValue = val;
    }
  }

  if(_p2Y0 != NULL) {
    static float oldValue = 0.f;
    val = _p2Y0->getFloatValue();
    if(val != oldValue) {
      e.type = InputEvent::P2_ACC;
      e.acc = val;
      addEvent(&e, numEvents);
      oldValue = val;
    }
  }

  // READY

  if(_p1Cross != NULL) {
    if(_p1Cross->getBoolTrue()) {
      e.type = InputEvent::P1_READY;
      addEvent(&e, numEvents);
    } else if (_p1Cross->getBoolFalse()) {
      e.type = InputEvent::P1_NOT_READY;
      addEvent(&e, numEvents);
    }
  }

  if(_p2Cross != NULL) {
    if(_p2Cross->getBoolTrue()) {
      e.type = InputEvent::P2_READY;
      addEvent(&e, numEvents);
    } else if (_p2Cross->getBoolFalse()) {
      e.type = InputEvent::P2_NOT_READY;
      addEvent(&e, numEvents);
    }
  }

  // ON SCREEN LOGGING ON / OFF
  
  if(_p1Triangle != NULL) {
    if(_p1Triangle->getBoolTrue()) {
      e.type = InputEvent::ON_SCREEN_LOG_ON;
      addEvent(&e, numEvents);
    } else if (_p1Triangle->getBoolFalse()) {
      e.type = InputEvent::ON_SCREEN_LOG_OFF;
      addEvent(&e, numEvents);
    }
  }
  
  return _events;
}

void Input::addEvent(InputEvent *e, int *numEvents) {
  _events[*numEvents] = *e;
  (*numEvents)++;
}
