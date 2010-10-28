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

class Paddle
{
  public:
  Paddle() : _ready(false), _wants_reset(false), _acc(0), _speed(0), _maxSpeed(0) { }
  virtual ~Paddle() { }
  void set_size(float w1, float h1) { _w = w1; _h = h1; }
  void set_position(float x1, float y1) { _x = x1; _y = y1; }
  void set_color(float r, float g, float b) { _colorR = r; _colorG = g; _colorB = b; }  
  void set_acceleration(float acc) { _acc = acc; }
  void set_speed(float speed1) { _speed = speed1; }
  void render();
  void set_ready(bool ready1) { _ready = ready1; }
  void set_wants_reset(bool wants_reset1) { _wants_reset = wants_reset1; }
  void update(int time, float damp);
  void bounce(float col_damp);
  void clamp(float minY, float maxY) { if(_y < minY) _y = minY; if(_y > maxY) _y = maxY; }

  float h() { return _h; }
  float w() { return _w; }
  float y() { return _y; }
  float &x() { return _x; }
  float speed() { return _speed; }
  float maxSpeed() {return _maxSpeed; }
  bool ready() { return _ready; }
  bool wants_reset() { return _wants_reset; }

private:
  bool  _ready, _wants_reset;
  float _w, _h, _x, _y;
  float _colorR, _colorG, _colorB;
  float _acc, _speed, _maxSpeed;
};

