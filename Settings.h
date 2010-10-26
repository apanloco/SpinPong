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

class Settings
{
public:
  Settings();
  virtual ~Settings() { }

  bool parse() { return true; }

  int step;
  
  int heightMargin;
  
  float paddleOffset;
  float paddleWidth;
  float paddleHeight;
  float ballSize;
  float ballSpeed;
  float ballRotation;
  
  float factBallStabilize;
  float factCollisionSpinAngle;
  float factCollisionSpinDamp;
  float factPaddleCollisionSpin;
  float factPaddleCollisionAngle;
  float factAirSpinDamp;  
  float factAirSpinAngle;
  float factAirPaddleSpeedDamp;
  float factBallAcceleration;
  float factPaddleAcceleration;
  float factPaddleCollisionDamp;
};
