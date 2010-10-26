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

#include "Sound.h"

#include "Log.h"

#include <stdlib.h>
#include <sys/paths.h>
#include <cell/cell_fs.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

#include <cell/audio.h>
#include <sys/event.h>
#include <sys/ppu_thread.h>
#include <sys/synchronization.h>

#define AUDIO_BLOCK_COUNT 16 
#define AUDIO_CHANNEL_COUNT 2

#define SOUND_PATH "/dev_hdd0/game/SPNP12345/USRDIR/sound/"

#define TIMEOUT 5000000 // mutex timeout, 5 secs

Sound::Sound()
{
  _currentSound = NULL;
  _soundPos = 0;
  sys_mutex_attribute_t mut_attr;
  sys_mutex_attribute_initialize(mut_attr);
  int error = sys_mutex_create(&_mutex, &mut_attr);
  if(error != CELL_OK) {
    LOG("WARNING: failed to create mutex: %x", error);
  }
  for(int i = 0; i < SOUND_NUM; i++) {
    memset(&_sounds[i], 0, sizeof(struct SoundInfo));
  }
}

Sound::~Sound()
{
  (void) cellAudioRemoveNotifyEventQueue(_queueKey);
  (void) sys_event_queue_destroy(_eventQueue, 0);
  (void) cellAudioPortClose(_audioPort);
  (void) cellAudioQuit();
  (void) sys_mutex_destroy(_mutex);

  for(int i = 0; i < SOUND_NUM; i++) {
    if(_sounds[i].data != NULL) { free(_sounds[i].data); }
  }
}

void Sound::soundMain(uint64_t arg) {
  LOG("");
  Sound *self = reinterpret_cast<Sound *>(arg);
  self->soundMainInternal();  
}

void Sound::soundMainInternal() {
  LOG("");
  if(initCellAudio() == false) {
    LOG("Failed to init cell audio");
    return;
  }
  while(1) {
    sys_event_t event;
    int error = sys_event_queue_receive(_eventQueue, &event, 4 * 1000 * 1000);
    if(error == ETIMEDOUT) {
      LOG("Received ETIMEDOUT from sys_event_queue_receive");
      continue;
    }
    writeAudio();
  }
}

void Sound::writeAudio() {
  int error;
  int currentReadBlock = *(uint64_t *)_readIndexAddr;
  int samplesToWrite = CELL_AUDIO_BLOCK_SAMPLES * AUDIO_CHANNEL_COUNT;
  float *outBuffer = _audioBuffer + AUDIO_CHANNEL_COUNT * CELL_AUDIO_BLOCK_SAMPLES * currentReadBlock;

  error = sys_mutex_lock(_mutex, TIMEOUT);
  if(error != CELL_OK) {
    LOG("call to sys_mutex_lock() failed: %x", error);
  }

  if(_currentSound == NULL) {
    goto done;
  }

  LOG("currentSound: %08X, currentReadBlock: %d, samplesToWrite: %d, outBuffer: %08X", _currentSound, currentReadBlock, samplesToWrite, outBuffer);

  for(int i = 0; i < samplesToWrite; i++) {
    float sample = _currentSound->data[_soundPos];
    outBuffer [i * 2]     = sample * _leftVolume;
    outBuffer [i * 2 + 1] = sample * _rightVolume;
    _soundPos++;
    if(_soundPos == _currentSound->samples) {
      LOG("Sound complete");
      _currentSound = NULL;
      _soundPos = 0;
      break;
    }
  }

done:
  error = sys_mutex_unlock(_mutex);
  if(error != CELL_OK) {
    LOG("call to sys_mutex_unlock() failed: %x", error);
  }
}

bool Sound::init()
{
  LoadRAW(SOUND_PATH "start1.raw",  &_sounds[SOUND_START1]);
  LoadRAW(SOUND_PATH "start2.raw",  &_sounds[SOUND_START2]);
  LoadRAW(SOUND_PATH "start3.raw",  &_sounds[SOUND_START3]);
  LoadRAW(SOUND_PATH "start4.raw",  &_sounds[SOUND_START4]);
  LoadRAW(SOUND_PATH "start5.raw",  &_sounds[SOUND_START5]);
  LoadRAW(SOUND_PATH "start6.raw",  &_sounds[SOUND_START6]);
  LoadRAW(SOUND_PATH "cough.raw",   &_sounds[SOUND_COUGH]);
  LoadRAW(SOUND_PATH "paddle1.raw", &_sounds[SOUND_PADDLE1]);
  LoadRAW(SOUND_PATH "paddle2.raw", &_sounds[SOUND_PADDLE2]);
  LoadRAW(SOUND_PATH "ping1.raw",   &_sounds[SOUND_PING1]);
  LoadRAW(SOUND_PATH "ping2.raw",   &_sounds[SOUND_PING2]);
  LoadRAW(SOUND_PATH "pong1.raw",   &_sounds[SOUND_PONG1]);
  LoadRAW(SOUND_PATH "pong2.raw",   &_sounds[SOUND_PONG2]);
  LoadRAW(SOUND_PATH "pong3.raw",   &_sounds[SOUND_PONG3]);

#if 0
  Mix_VolumeChunk(_chunks[SOUND_PADDLE1], 50);
  Mix_VolumeChunk(_chunks[SOUND_PADDLE2], 50);

  Mix_VolumeChunk(_chunks[SOUND_START1], 50);
  Mix_VolumeChunk(_chunks[SOUND_START2], 50);
  Mix_VolumeChunk(_chunks[SOUND_START3], 50);
  Mix_VolumeChunk(_chunks[SOUND_START4], 30);
  Mix_VolumeChunk(_chunks[SOUND_START5], 50);
  Mix_VolumeChunk(_chunks[SOUND_START6], 30);
  Mix_VolumeChunk(_chunks[SOUND_COUGH], 50);
#endif

  int error;
  error = sys_ppu_thread_create(&_threadId, soundMain, (uint64_t)this, 100, 0x8000, 0 /*SYS_PPU_THREAD_CREATE_JOINABLE*/, (char *)"sound thread");
  if(error != CELL_OK) {
    LOG("call to sys_ppu_thread_create failed: %x", error);
  }

  return true;
}

bool Sound::initCellAudio() {
  CellAudioPortParam params;
  CellAudioPortConfig config;
  int error;

  error = cellAudioInit();
  if(error != CELL_OK) {
    LOG("WARNING: call to cellAudioInit failed: %x", error);
  }

  params.nChannel = AUDIO_CHANNEL_COUNT;
  params.nBlock = AUDIO_BLOCK_COUNT;
  params.attr = 0;

  error = cellAudioPortOpen(&params, &_audioPort);
  if(error != CELL_OK) {
    LOG("call to cellAudioPortOpen failed: %x", error);
    cellAudioQuit();
    return false;
  }

  cellAudioGetPortConfig(_audioPort, &config);
  
  if(config.nChannel != AUDIO_CHANNEL_COUNT) {
    LOG("WARNING: Requested %d channels, got %d", AUDIO_CHANNEL_COUNT, config.nChannel);
  }
  
  if(config.nBlock != AUDIO_BLOCK_COUNT) {
    LOG("WARNING: Requested %d blocks, got %d", AUDIO_BLOCK_COUNT, config.nBlock);
  }

  _audioBuffer = (float *)config.portAddr;
  _readIndexAddr = config.readIndexAddr;

  error = cellAudioCreateNotifyEventQueue(&_eventQueue, &_queueKey);
  if(error != CELL_OK){
    cellAudioPortClose(_audioPort);
    cellAudioQuit();
    return false;
  }

  error = cellAudioSetNotifyEventQueue(_queueKey);
  if(error != CELL_OK){
    if(sys_event_queue_destroy(_eventQueue, 0) != CELL_OK) {
      LOG("WARNING: call to sys_event_queue_destroy failed");
    }
    cellAudioPortClose(_audioPort);
    cellAudioQuit();
    return false;
  }

  sys_event_queue_drain(_eventQueue);

  error = cellAudioPortStart(_audioPort);
  if(error != CELL_OK) {
    LOG("call to cellAudioPortStart failed: %x", error);
    cellAudioQuit();
    return false;
  }

  LOG("audio initialized");

  return true;
}

bool Sound::LoadRAW(const char *const filename, struct SoundInfo *si) {
  CellFsErrno err;
  CellFsStat stat;
  err = cellFsStat(filename,  &stat);
  if(err != CELL_FS_SUCCEEDED) {
    LOG("Could not stat '%s': %x", filename, err);
    return false;
  } else {
    //LOG("'%s' was stat:ed OK! (%u bytes)", filename, (unsigned int)stat.st_size);
  }

  // file exists, lets load it
  int fd = open(filename, O_RDONLY);
  if(fd == -1) {
    LOG("call to open on '%s' failed");
    return false;
  }
  float *data = (float *)malloc(stat.st_size); // "16 bytes alignment is guaranteed by normal malloc calls"
  if(data == NULL) {
    LOG("failed to allocate %lu bytes of memory (memalign 16)", stat.st_size);
    return false;
  }
  
  ssize_t bytes_read = read(fd, data, stat.st_size);

  if(bytes_read != (ssize_t)stat.st_size) {
    LOG("failed to read '%s', %d != %lu", bytes_read, stat.st_size);
    free(data);
    return false;
  }

  si->data = data;
  si->samples = stat.st_size / sizeof(float) * 1 /* MONO */;

  LOG("Sound file '%s' loaded, %u samples, (SoundInfo address: %08X)", filename, si->samples, si);

  return true;
}

void Sound::play(SoundInfo *si, float volume, float pan) {
  int error;

  _leftVolume = (1.0f - pan) * volume;
  _rightVolume = (pan) * volume;

  error = sys_mutex_lock(_mutex, TIMEOUT);
  if(error != CELL_OK) {
    LOG("call to sys_mutex_lock() failed: %x", error);
  }

  _soundPos = 0;
  _currentSound = si;

  LOG("new sound: %08X (pan: %f, leftVolue: %f, rightVolume: %f (volume: %f))", si, pan, _leftVolume, _rightVolume, volume);

  error = sys_mutex_unlock(_mutex);
  if(error != CELL_OK) {
    LOG("call to sys_mutex_unlock() failed: %x", error);
  }
}

bool Sound::playStart()
{
  play(&_sounds[rand() % 6 + SOUND_START1], 0.9f, 0.5f);
  return true;
}

bool Sound::playCough()
{
  play(&_sounds[SOUND_COUGH], 0.8f, 0.5f);
  return true;
}

bool Sound::playPing1(float lr)
{
  play(&_sounds[SOUND_PING1], 1.0f, lr);
  return true;
}

bool Sound::playPing2(float lr)
{
  play(&_sounds[SOUND_PING2], 1.0f, lr);
  return true;
}

bool Sound::playPong(float lr)
{
  play(&_sounds[rand() % 3 + SOUND_PONG1], 1.0f, lr);
  return true;    
}

bool Sound::playPaddle1(float volume)
{
  play(&_sounds[rand() % 1 + SOUND_PADDLE1], volume, 0.0f);
  return true;
}

bool Sound::playPaddle2(float volume)
{
  play(&_sounds[rand() % 1 + SOUND_PADDLE2], volume, 1.0f);
  return true;
}

