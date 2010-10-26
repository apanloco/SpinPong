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

#include "Settings.h"

Settings::Settings()
{
  step = 1;

  heightMargin = 100;

  paddleOffset = 20.0f;
  paddleWidth = 5.0f;
  paddleHeight = 70.0f;
  ballSize = 18.0f;
  ballSpeed = 0.30f;
  ballRotation = 2.0f;

  factBallStabilize = 1.00015;
  factCollisionSpinAngle = 1.0f;
  factCollisionSpinDamp = 0.8f;
  factPaddleCollisionSpin = 3.0f;
  factPaddleCollisionAngle = 0.5f;
  factAirSpinDamp = 1.0f;  
  factAirSpinAngle = 0.03f;
  factBallAcceleration = 0.00001f;
  factAirPaddleSpeedDamp = 0.997f;
  factPaddleAcceleration = 0.0016f;
  factPaddleCollisionDamp = 0.9f;
}
