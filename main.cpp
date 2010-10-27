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

#include "main.h"

#include <stdio.h>
#include <time.h>
#include <sys/process.h>

#include "FWDebugFont.h"

// change these to log to another host
#define LOG_HOST "192.168.1.2"
#define LOG_PORT 6160

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (720)

// bit mask
#define COLLISION_TYPE_BALL_WALL    (0x01)
#define COLLISION_TYPE_BALL_PADDLE1 (0x02)
#define COLLISION_TYPE_BALL_PADDLE2 (0x04)
#define COLLISION_TYPE_PADDLE1_WALL (0x08)
#define COLLISION_TYPE_PADDLE2_WALL (0x10)
#define COLLISION_TYPE_BALL_WALL_L  (0x20)
#define COLLISION_TYPE_BALL_WALL_R  (0x40)

// instantiate the class
Main app;

//-----------------------------------------------------------------------------
// Description: Constructor
//-----------------------------------------------------------------------------
Main::Main() : _paddle1(_paddle[0]), _paddle2(_paddle[1]), _paused(false), _onScreenLog(false), _bounces(-1) {
  strcpy(mStartupInfo.mWindowTitle, "Sping Pong");
  strcpy(mStartupInfo.mArgumentsFilename, "fwArgs.txt");
  mDispInfo.mAntiAlias = true;
  mStartupInfo.mUseDebugConsole = false;
}

//-----------------------------------------------------------------------------
// Description: Initialization callback
//-----------------------------------------------------------------------------
bool Main::onInit(int argc, char **ppArgv) {
  FWGLApplication::onInit(argc, ppArgv);

  setProjectionMode(ProjectionMode_Orthographic);

  log_init(LOG_HOST, LOG_PORT);
  
  LOG("Log initiated");

  srand(time(NULL));

  if(!_settings.parse())
  {
    LOG("could not parse settings");
    panic();
  }

  if(!_input.reset())
  {
    LOG("Could not reset input devices");
    panic();
  }

  if(!_sound.init())
  {
    LOG("WARNING: Could not initialize sound");
  }

  _tc.reset(_settings.step);

  _score1 = 0;
  _score2 = 0;

  reset();
  
  setClearRed(0.0f);
  setClearGreen(0.0f);
  setClearBlue(0.0f);

  return true;
}

bool Main::onUpdate() {
  LOG("onUpdate()");
  return FWGLApplication::onUpdate();
}

void Main::onRender() {
  LOG("onRender()");

  FWGLApplication::onRender();

  handle_events();

  if(_paddle1.wants_reset() && _paddle2.wants_reset())
  {
    _score1 = _score2 = 0;
    reset();
    _paddle1.set_wants_reset(false);
    _paddle2.set_wants_reset(false);
  }
  
  if(_paddle1.ready() && _paddle2.ready() &&!_ready)
  {
    _ready = true;
  }
  
  _tc.measure();

  int steps = 0;
  while(_tc.step() && !_paused)
  {
    //LOG("step");

    steps++;
    _paddle[0].update(_settings.step, _settings.factAirPaddleSpeedDamp);
    _paddle[1].update(_settings.step, _settings.factAirPaddleSpeedDamp);
    
    // rotate ball due to spin and air friction

    if(_ready)
    {
      _ball.damp_spin(_settings.factAirSpinDamp);
      _ball.rotate(_settings.step, _settings.factAirSpinAngle * _ball.spin());
      _ball.update(_settings.step, _settings.factBallStabilize, _settings.ballRotation);
    }

    unsigned int collisions = check_collisions();
    
    if(collisions & COLLISION_TYPE_BALL_WALL) {
      LOG("COLLISION_TYPE_BALL_WALL");
      _sound.playPong(_ball.x() / (SCREEN_WIDTH - _settings.paddleOffset - _settings.paddleWidth));
      _ball.bounce_tb();
      _ball.bounce_rotate(_settings.factCollisionSpinAngle * _ball.spin(), true);
      _ball.clamp_tb(_settings.ballSize / 2 + _settings.heightMargin, SCREEN_HEIGHT - _settings.ballSize / 2 - _settings.heightMargin);
      _ball.damp_spin(_settings.factCollisionSpinDamp);
    }
    if(collisions & COLLISION_TYPE_BALL_PADDLE1) {
      LOG("COLLISION_TYPE_BALL_PADDLE1");
      _sound.playPing1(0.1f); // pan left
      _ball.bounce_lr();
      _ball.damp_spin(_settings.factCollisionSpinDamp);
      /* HACK */ _ball.add_spin(_settings.factPaddleCollisionAngle * -_paddle[0].speed() * _settings.factPaddleCollisionSpin);
      _ball.bounce_rotate(_settings.factCollisionSpinAngle * _ball.spin(), false);
      /* HACK */ _ball.add_spin(_settings.factPaddleCollisionAngle * _paddle[0].speed() * _settings.factPaddleCollisionSpin);
      _ball.add_spin(_paddle[0].speed() * _settings.factPaddleCollisionSpin);
      _ball.clamp_spin();
      _ball.clamp_lr(_settings.paddleOffset + _settings.paddleWidth + _settings.ballSize / 2,
                    SCREEN_WIDTH - _settings.paddleOffset - _settings.paddleWidth - _settings.ballSize / 2);
    }
    if(collisions & COLLISION_TYPE_BALL_PADDLE2) {
      LOG("COLLISION_TYPE_BALL_PADDLE2");
      _sound.playPing2(0.9f); // pan right
      _ball.bounce_lr();
      _ball.damp_spin(_settings.factCollisionSpinDamp);
      /* HACK */ _ball.add_spin(_settings.factPaddleCollisionAngle * -_paddle[1].speed() * -_settings.factPaddleCollisionSpin);
      _ball.bounce_rotate(_settings.factCollisionSpinAngle * _ball.spin(), false);
      /* HACK */ _ball.add_spin(_settings.factPaddleCollisionAngle * _paddle[1].speed() * -_settings.factPaddleCollisionSpin);
      _ball.add_spin(_paddle[1].speed() * -_settings.factPaddleCollisionSpin);
      _ball.clamp_spin();
      _ball.clamp_lr(_settings.paddleOffset + _settings.paddleWidth + _settings.ballSize / 2,
                    SCREEN_WIDTH - _settings.paddleOffset - _settings.paddleWidth - _settings.ballSize / 2);
    }
    if(collisions & COLLISION_TYPE_PADDLE1_WALL) {
      LOG("COLLISION_TYPE_PADDLE1_WALL");
      _paddle[0].bounce(_settings.factPaddleCollisionDamp);
      _paddle[0].clamp(_settings.paddleHeight / 2 + _settings.heightMargin, SCREEN_HEIGHT - _settings.paddleHeight / 2 - _settings.heightMargin);
      if(_paddle[0].speed() > 0.01 || _paddle[0].speed() < -0.01)
      {
        _sound.playPaddle1(_paddle[0].speed() / _paddle[0].maxSpeed());
      }
    }
    if(collisions & COLLISION_TYPE_PADDLE2_WALL) {
      LOG("COLLISION_TYPE_PADDLE2_WALL");
      _paddle[1].bounce(_settings.factPaddleCollisionDamp);
      _paddle[1].clamp(_settings.paddleHeight / 2 + _settings.heightMargin, SCREEN_HEIGHT - _settings.paddleHeight / 2 - _settings.heightMargin);
      if(_paddle[1].speed() > 0.01 || _paddle[1].speed() < -0.01)
      {
        _sound.playPaddle2(_paddle[1].speed() / _paddle[1].maxSpeed());
      }
    }
    if(collisions & COLLISION_TYPE_BALL_WALL_L) {
      LOG("COLLISION_TYPE_BALL_WALL_L");
      _score2++;
      reset();
    }
    if(collisions & COLLISION_TYPE_BALL_WALL_R) {
      LOG("COLLISION_TYPE_BALL_WALL_R");
      _score1++;
      reset();
    }
  }

  //LOG("steps: %d", steps);
  _paddle[0].render();
  _paddle[1].render();
  _ball.render();
  renderArena();

  renderDebugInfo();
}

void Main::renderArena()
{
  // render arena lines
  float vertst[] =
  {
    0, _settings.heightMargin, 0.0f,
    SCREEN_WIDTH, _settings.heightMargin, 0.0f,
  };

  float vertsb[] =
  {
    0, SCREEN_HEIGHT - _settings.heightMargin + 1, 0.0f,
    SCREEN_WIDTH, SCREEN_HEIGHT - _settings.heightMargin + 1, 0.0f,
  };

  float colors[] =
  {
    1.f, 1.f, 1.f, 1.f,
    1.f, 1.f, 1.f, 1.f,
  };

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertst);
  glColorPointer(4, GL_FLOAT, 0, colors);
  glDrawArrays(GL_LINES, 0, 2);
  glVertexPointer(3, GL_FLOAT, 0, vertsb);
  glColorPointer(4, GL_FLOAT, 0, colors);
  glDrawArrays(GL_LINES, 0, 2);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}

void Main::renderDebugInfo()
{
  FWDebugFont::setColor(1.f, 1.f, 1.f, 1.f);

  if (_onScreenLog) {
    FWTimeVal  time = FWTime::getCurrentTime();
    float fps = 1.0f / ((float)(time - _lastTime) + 0.00001f);
    _lastTime = time;

#define POS(p) (200 + (15 * p))
    FWDebugFont::setPosition(0, POS(0));
    FWDebugFont::printf("FPS: %.2f\n", fps);
    FWDebugFont::setPosition(0, POS(3));
    FWDebugFont::printf("P1 PAD X: %f\n", _paddle1.x());
    FWDebugFont::setPosition(0, POS(4));
    FWDebugFont::printf("P1 PAD Y: %f\n", _paddle1.y());
    FWDebugFont::setPosition(0, POS(5));
    FWDebugFont::printf("P1 PAD W: %f\n", _paddle1.w());
    FWDebugFont::setPosition(0, POS(6));
    FWDebugFont::printf("P1 PAD H: %f\n", _paddle1.h());
    FWDebugFont::setPosition(0, POS(7));
    FWDebugFont::printf("P2 PAD X: %f\n", _paddle2.x());
    FWDebugFont::setPosition(0, POS(8));
    FWDebugFont::printf("P2 PAD Y: %f\n", _paddle2.y());
    FWDebugFont::setPosition(0, POS(9));
    FWDebugFont::printf("P2 PAD W: %f\n", _paddle2.w());
    FWDebugFont::setPosition(0, POS(10));
    FWDebugFont::printf("P2 PAD H: %f\n", _paddle2.h());
    FWDebugFont::setPosition(0, POS(11));
    FWDebugFont::printf("P1 READY: %d\n", _paddle1.ready());
    FWDebugFont::setPosition(0, POS(12));
    FWDebugFont::printf("P2 READY: %d\n", _paddle2.ready());
    FWDebugFont::setPosition(0, POS(13));
    FWDebugFont::printf("BALL X: %f\n", _ball.x());
    FWDebugFont::setPosition(0, POS(14));
    FWDebugFont::printf("BALL Y: %f\n", _ball.y());
#undef POS
  }

#define IS_READY "[READY] "
#define WANTS_RESET "[WANTS_RESET]"

  FWDebugFont::setPosition(200, 600);
  FWDebugFont::printf("%d %s%s\n", _score1, _paddle1.ready() ? IS_READY : "", _paddle1.wants_reset() ? WANTS_RESET : "");
  FWDebugFont::setPosition(950, 600);
  FWDebugFont::printf("%d %s%s\n", _score2, _paddle2.ready() ? IS_READY : "", _paddle2.wants_reset() ? WANTS_RESET : "");
}

void Main::onSize(const FWDisplayInfo& rDispInfo){
  LOG("onSize, w=%d, h=%d", rDispInfo.mWidth, rDispInfo.mHeight);
  FWGLApplication::onSize(rDispInfo);

  // set 2d projection matrix, FWGLApplication flips Y dir
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2Df(0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0);

  _displayInfo = rDispInfo;
}

void Main::onShutdown(){
  LOG("onShutdown()");
  log_close();
  FWGLApplication::onShutdown();
}

unsigned int Main::check_collisions()
{
//  LOG("check_collisions()");

  unsigned int collisions = 0;

  // ball->Wall (top/bottom)
  if(_ball.y() < (_ball.size() / 2 + _settings.heightMargin) ||
     _ball.y() > (SCREEN_HEIGHT - _ball.size() / 2 - _settings.heightMargin))
  {
    collisions |= COLLISION_TYPE_BALL_WALL;
  }
  
  // ball->wall, left
  if(_ball.x() <= (-_ball.size() / 2))
  {
     collisions |= COLLISION_TYPE_BALL_WALL_L;
  }

  // ball->wall, right
  if(_ball.x() >= (SCREEN_WIDTH + _ball.size() / 2))
  {
     collisions |= COLLISION_TYPE_BALL_WALL_R;
  }

  if(_paddle1.y() <= (_paddle1.h() / 2 + _settings.heightMargin) ||
     _paddle1.y() >= (SCREEN_HEIGHT - _paddle1.h() / 2 - _settings.heightMargin))
  {
    collisions |= COLLISION_TYPE_PADDLE1_WALL;
  }
  
  if(_paddle2.y() <= (_paddle2.h() / 2 + _settings.heightMargin) ||
     _paddle2.y() >= (SCREEN_HEIGHT - _paddle2.h() / 2 - _settings.heightMargin))
  {
    collisions |= COLLISION_TYPE_PADDLE2_WALL;
  }

#define MARGIN 2
  
  if(_ball.x() - _settings.ballSize / 2 <= (_settings.paddleOffset + _settings.paddleWidth) &&
     _ball.x() - _settings.ballSize / 2 >= (_settings.paddleOffset - MARGIN + _settings.paddleWidth) &&
     _ball.y() < (_paddle1.y() + _settings.paddleHeight / 2 + _ball.size() / 4) &&
     _ball.y() > (_paddle1.y() - _settings.paddleHeight / 2 - _ball.size() / 4))
  {
    collisions |= COLLISION_TYPE_BALL_PADDLE1;
    _bounces++;
  }

  if(_ball.x() + _settings.ballSize / 2 >= (SCREEN_WIDTH - _settings.paddleOffset - _settings.paddleWidth) &&
     _ball.x() + _settings.ballSize / 2 <= (SCREEN_WIDTH - _settings.paddleOffset + MARGIN - _settings.paddleWidth) &&
     _ball.y() < (_paddle2.y() + _settings.paddleHeight / 2 + _ball.size() / 4) &&
     _ball.y() > (_paddle2.y() - _settings.paddleHeight / 2 - _ball.size() / 4))
  {
    collisions |= COLLISION_TYPE_BALL_PADDLE2;
    _bounces++;
  }

#undef MARGIN
  
  return collisions;
}

void Main::handle_events()
{
//  LOG("handle_events()");

  int numEvents;
  InputEvent *events = _input.get_events(&numEvents);

  for(int i = 0; i < numEvents; i++)
  {
    InputEvent *ie = &events[i];
    switch(ie->type)
    {
      case InputEvent::QUIT:
        LOG("QUIT (NOT WORKING)");
        break;
      case InputEvent::RELOAD:
        LOG("RELOAD");
        break;
      case InputEvent::PAUSE:
        LOG("PAUSE");
        _paused = !_paused;
        break;
      case InputEvent::P1_ACC:
        LOG("P1_ACC: %f", ie->acc);
        _paddle1.set_acceleration(_settings.factPaddleAcceleration * ie->acc);
        break;
      case InputEvent::P1_READY:
        LOG("P1_READY");
        _paddle1.set_ready(true);
        break;
      case InputEvent::P1_NOT_READY:
        LOG("P1_NOT_READY");
        _paddle1.set_ready(false);
        break;
      case InputEvent::P1_RESET:
        LOG("P1_RESET");
        _paddle1.set_wants_reset(true);
        break;
      case InputEvent::P1_NOT_RESET:
        LOG("P1_NOT_RESET");
        _paddle1.set_wants_reset(false);
        break;
      case InputEvent::P2_ACC:
        LOG("P2_ACC: %f", ie->acc);
        _paddle2.set_acceleration(_settings.factPaddleAcceleration * ie->acc);
        break;
      case InputEvent::P2_READY:
        LOG("P2_READY");
        _paddle2.set_ready(true);
        break;
      case InputEvent::P2_NOT_READY:
        LOG("P2_NOT_READY");
        _paddle2.set_ready(false);
        break;
      case InputEvent::P2_RESET:
        LOG("P2_RESET");
        _paddle2.set_wants_reset(true);
        break;
      case InputEvent::P2_NOT_RESET:
        LOG("P2_NOT_RESET");
        _paddle2.set_wants_reset(false);
        break;
      case InputEvent::ON_SCREEN_LOG_ON:
        LOG("ON_SCREEN_LOG_ON");
        _onScreenLog = true;
        break;
      case InputEvent::ON_SCREEN_LOG_OFF:
        LOG("ON_SCREEN_LOG_OFF");
        _onScreenLog = false;
        break;
    }
  }
}

void Main::panic()
{
  LOG("PANIC");
  sys_process_exit(1);
}

void Main::reset()
{
  LOG("reset()");

  _paddle[0].set_size(_settings.paddleWidth, _settings.paddleHeight);
  _paddle[0].set_position(_settings.paddleOffset + _settings.paddleWidth / 2, SCREEN_HEIGHT/2);
  _paddle[0].set_color(0.9f, 0.1f, 0.1f);
  _paddle[0].set_acceleration(0);
  _paddle[0].set_speed(0);
  _paddle[1].set_size(_settings.paddleWidth, _settings.paddleHeight);
  _paddle[1].set_position(SCREEN_WIDTH - _settings.paddleOffset - _settings.paddleWidth / 2, SCREEN_HEIGHT/2);
  _paddle[1].set_color(0.1f, 0.1f, 0.9f);
  _paddle[1].set_acceleration(0);
  _paddle[1].set_speed(0);
  
  _ball.set_size(_settings.ballSize);
  _ball.set_position(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
  _ball.set_direction(((rand() % 2)*2) - 1, 0.0f);
  _ball.set_speed(_settings.ballSpeed);
  _ball.set_acceleration(_settings.factBallAcceleration);
  _ball.reset_spin();
  _ball.reset_rotation_angle();

  _ready = false;

  // only play cough when there are no bounces, but not when the game is being reset
  if(_bounces == 0 && _paddle1.wants_reset() == false)
  {
    _sound.playCough();
  }
  else
  {
    _sound.playStart();
  }

  _bounces = 0;
}
