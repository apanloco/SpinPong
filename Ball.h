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

#define POINTS (200)

class Ball
{
  public:
  Ball();
  virtual ~Ball();
  void set_size(float size1) { _size = size1; }
  void set_position(float x1, float y1) { _x = x1; _y = y1; }
  void set_direction(float dirX, float dirY) { _dirX = dirX; _dirY = dirY; } 
  void set_acceleration(float acc) { _acc = acc; }
  void set_speed(float speed) { _speed = speed; }
  void add_spin(float spin1) { _spin += spin1; }
  void clamp_spin() { if(_spin > 1.0) _spin = 1.0f; if(_spin < -1.0f) _spin = -1.0f; }
  void reset_spin() { _spin = 0.0; _angle = 0.0; }
  void set_spin(float spin1) { _spin = spin1; }
  void damp_spin(float damp) { _spin *= damp; } 
  void clamp_tb(float minY, float maxY) { if(_y < minY) _y = minY; if(_y > maxY) _y = maxY; }
  void clamp_lr(float minX, float maxX) { if(_x < minX) _x = minX; if(_x > maxX) _x = maxX; }
  void bounce_rotate(float amount, bool tb);
  void render();
  void rotate(int time, float angle);
  void update(int time, float stabilize, float factRotation);
  void get_info(int *size, int *x, int *y);
  void reset_rotation_angle() { _angle = -6; }

  void bounce_tb() { _dirY *= -1; }
  void bounce_lr() { _dirX *= -1; }

  float size() { return _size; }
  float x() { return _x; }
  float y() { return _y; }
  float spin() { return _spin; }

private:
  float _size, _x, _y;
  float _dirX, _dirY;
  float _speed;
  float _spin;
  float _angle;
  float _acc;
  float *_verts;
  void mat_mult(float M[16], float v[4]);
  void rotate_vector(float *x, float *y, float angle);
};

