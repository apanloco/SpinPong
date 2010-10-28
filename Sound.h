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

#include <stdint.h>

struct SoundInfo {
  float *data;
  int samples;
};

class Sound
{
public:
	Sound();
	virtual ~Sound();
	
	bool init();
	
	bool playStart();
	bool playCough();
	bool playReady();
	bool playGo();
	bool playPing1(float lr);
	bool playPing2(float lr);
	bool playPong(float lr);
	bool playPaddle1(float volume);
	bool playPaddle2(float volume);
private:
	typedef enum {
		SOUND_START1,
		SOUND_START2,
		SOUND_START3,
		SOUND_START4,
		SOUND_START5,
		SOUND_START6,
		SOUND_COUGH,
		SOUND_PADDLE1,
		SOUND_PADDLE2,
		SOUND_PING1,
		SOUND_PING2,
		SOUND_PONG1,
		SOUND_PONG2,
		SOUND_PONG3,
		SOUND_NUM
	} sounds;

  void play(SoundInfo *si, float volume, float pan);
  void writeAudio();
  bool initCellAudio();
  bool LoadRAW(const char *const filename, struct SoundInfo *si);
  static void soundMain(uint64_t arg);
  void soundMainInternal();
  struct SoundInfo _sounds[SOUND_NUM];
  
  uint32_t _audioPort;
  sys_ppu_thread_t _threadId;
  sys_event_queue_t _eventQueue;
  sys_ipc_key_t _queueKey;
  float *_audioBuffer;
  sys_addr_t _readIndexAddr;
  SoundInfo *_currentSound;
  int _soundPos;
  sys_cond_t _cond;
  sys_mutex_t _mutex;
  float _leftVolume;
  float _rightVolume;
};
