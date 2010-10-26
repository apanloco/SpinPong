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

#include "Log.h"
#include "Paddle.h"
#include "Ball.h"
#include "Input.h"
#include "Settings.h"
#include "Sound.h"
#include "TimeControl.h"

#include "psgl/FWGLCamControlApplication.h"
#include "FWDisplayInfo.h"
#include "psgl/FWGLGrid.h"

class Main : public FWGLApplication {
public:
  Main();
  virtual bool onInit(int argc, char **ppArgv);
  virtual void onRender();
  virtual void onShutdown();
  virtual bool onUpdate();
  virtual void onSize(const FWDisplayInfo &dispInfo);

private:

  void renderArena();
  void renderDebugInfo();
  unsigned int check_collisions();
  void handle_events();
  void reset();
  void panic();

  Settings _settings;
  Paddle _paddle[2];
  Ball _ball;
  Paddle &_paddle1;
  Paddle &_paddle2;
  Input _input;
  Sound _sound;
  TimeControl _tc;
  FWTimeVal _lastTime;
  
  bool _paused;
  bool _onScreenLog;
  int _score1;
  int _score2;
  int _bounces;
  bool _ready;

  bool _log;
  
  FWDisplayInfo _displayInfo;
  FWGLGrid    mGrid;
};
