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

#include "Paddle.h"
#include <math.h>
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>

void Paddle::render()
{
  glPushMatrix();

  glTranslatef(_x, _y, 0.0f);

  static float verts[] =
  {
    -(_w / 2.0f), +(_h / 2.0f), 0.0f,
    +(_w / 2.0f), +(_h / 2.0f), 0.0f,
    +(_w / 2.0f), -(_h / 2.0f), 0.0f,
    -(_w / 2.0f), -(_h / 2.0f), 0.0f
  };

  float colors[]  =
  {
    _colorR, _colorG, _colorB, 1.f,
    _colorR, _colorG, _colorB, 1.f,
    _colorR, _colorG, _colorB, 1.f,
    _colorR, _colorG, _colorB, 1.f
  };

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, verts);
  glColorPointer(4, GL_FLOAT, 0, colors);
  glDrawArrays(GL_QUADS, 0, 4);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  
  glPopMatrix();
}

void Paddle::update(int time, float damp)
{
  _speed += _acc * time;
  _y += _speed * time;
  if(abs(_maxSpeed) < abs(_speed)) {
    _maxSpeed = abs(_speed);
  }
  _speed *= pow(damp, time);
}

void Paddle::bounce(float col_damp)
{
  _speed *= -1;
  _speed *= col_damp;
}
