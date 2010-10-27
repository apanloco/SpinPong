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

#include "Ball.h"

#include <math.h>
#include <string.h>

#include <PSGL/psgl.h>
#include <PSGL/psglu.h>

Ball::Ball() {
  _verts = new float[(POINTS + 1) * 2];

  int div = POINTS / 2;
  for(int i = 0; i < POINTS; i++)
  {
    float angle = 2 * M_PI * ((float)i / POINTS);
    int cut = i % div;

    float scale = 1.0f;

    if(cut <= 9) scale = 0.50f;
 
    _verts[i * 2 + 0] = (+cos(angle) * (0.5f) * scale);
    _verts[i * 2 + 1] = (-sin(angle) * (0.5f) * scale);
  }

  _verts[POINTS * 2 + 0] = (+cos(0.0f) * (0.5f));
  _verts[POINTS * 2 + 1] = (-sin(0.0f) * (0.5f));
}

Ball::~Ball() {
  delete[] _verts;
}

void Ball::render()
{
  float r, g, b;

  if(_spin < 0)
  {
    r = 1 + (_spin * 0.1f);
    g = 1 + (_spin * 0.9f);
    b = 1 + (_spin * 0.9f);
  }
  else
  {
    r = 1 - (_spin * 0.9f);
    g = 1 - (_spin * 0.9f);
    b = 1 - (_spin * 0.1f);
  }

  glPushMatrix();

  glTranslatef(_x, _y, 0.0f);
  glScalef(_size, _size, 1.0f);
  glRotatef(_angle, 0, 0, -1.0f);

  float colors[(POINTS + 1) * 4];

  // TODO: there must be a better way than to keep so many of the same colors
  for(int i = 0; i < POINTS + 1; i++)
  {
    colors[i * 4 + 0] = r;
    colors[i * 4 + 1] = g;
    colors[i * 4 + 2] = b;
    colors[i * 4 + 3] = 1.0f;
  }
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, _verts);
  glColorPointer(4, GL_FLOAT, 0, colors);
  glDrawArrays(GL_TRIANGLE_FAN, 0, POINTS + 1);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  glPopMatrix();
}

void Ball::rotate(int time, float angle)
{
  for(int i = 0; i < time; i++) {
    rotate_vector(&_dirX, &_dirY, angle);
  }
}

void Ball::bounce_rotate(float amount, bool tb) // amount = [-1..1]
{
  float angle;
  
  if(tb)
  {
    angle = atan(_dirY / _dirX);  
  }
  else
  {
    angle = atan(_dirX / _dirY);
  }
  
  
  angle *= (360/(2*3.1415));
  angle = fabs(angle);
  // a = [0..90]

  // if backspin, invert angle. _dirY is already inverted
  if(_dirY < 0 && _dirX > 0 && _spin > 0 ||
     _dirY < 0 && _dirX < 0 && _spin < 0 ||
     _dirY > 0 && _dirX > 0 && _spin < 0 ||
     _dirY > 0 && _dirX < 0 && _spin > 0)
  {
    angle = 90 - angle;
    // angle is now how much you can turn to go completely horizontal or vertical
    // lessen it more for back-spins (less back-spin allowed! difficult to foresee!)
    angle -= 30;
  }
  else
  {
    // angle is now how much you can turn to go completely horizontal or vertical
    // lessen it for top-spins (more top-spin allowed! not as difficult to counter)
    angle -= 8;  
  }

  // angle is now how much you can turn to go completely horizontal or vertical
  if(angle < 0) angle = 0;

  angle *= amount;

  rotate_vector(&_dirX, &_dirY, angle);

}

void Ball::update(int time, float stabilize, float factRotation)
{
  _speed += _acc * time;
  _x += _dirX * (_speed * time);
  _y += _dirY * (_speed * time);
  _angle += _spin * factRotation * time;

  _dirX *= pow(stabilize, time); // UGLY HACK

  float length = sqrt(_dirX * _dirX + _dirY * _dirY);
  _dirX /= length;
  _dirY /= length;  
}

void Ball::mat_mult(float M[16], float v[4])
{
  float res[4];
  res[0]=M[ 0]*v[0]+M[ 4]*v[1]+M[ 8]*v[2]+M[12]*v[3];
  res[1]=M[ 1]*v[0]+M[ 5]*v[1]+M[ 9]*v[2]+M[13]*v[3];
  res[2]=M[ 2]*v[0]+M[ 6]*v[1]+M[10]*v[2]+M[14]*v[3];
  res[3]=M[ 3]*v[0]+M[ 7]*v[1]+M[11]*v[2]+M[15]*v[3];
  v[0]=res[0];
  v[1]=res[1];
  v[2]=res[2];
  v[3]=res[3];
}

void Ball::rotate_vector(float *x1, float *y1, float angle)
{
  angle *= (3.1415*2)/360;
  float v[4];
  v[0] = *x1;
  v[1] = *y1;
  v[2] = 0;
  v[3] = 0;
  float m[16];
  memset(m, 0, sizeof(m));
  m[0] = cos(angle);
  m[1] = -sin(angle);
  m[4] = sin(angle);
  m[5] = cos(angle);
  m[10] = 1;
  m[15] = 1;
  mat_mult(m, v);
  *x1 = v[0];
  *y1 = v[1];
}

