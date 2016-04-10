#include <string>
#include <iostream>
#include <ctime>
#include "sound.h"
using std::string;

SDLSound::~SDLSound() {
//  std::cout << "Cleaning up sounds ..." << std::endl;
//  std::clock_t start = std::clock();
  Mix_HaltMusic();
  Mix_FreeMusic(music);
  for (unsigned int i = 0; i < sounds.size(); ++i) {
    Mix_FreeChunk(sounds[i]);
  }
  Mix_CloseAudio();
//  std::clock_t duration = std::clock() - start;
//  std::cout << "Clean up took " << duration << " ticks\n";
}

SDLSound::SDLSound() : 
  volume(SDL_MIX_MAXVOLUME/4), 
  currentSound(-1), 
  music(NULL),
  audioRate(22050), 
  audioChannels(2), 
  audioBuffers(4096),
  sounds(),
  channels() 
{
  if(Mix_OpenAudio(audioRate, MIX_DEFAULT_FORMAT, audioChannels, 
                   audioBuffers)){
    throw string("Unable to open audio!");
  }
//  music = Mix_LoadMUS("sounds/100Years.mp3");
  // Need to install midi to play the following:
  // music = Mix_LoadMUS("sound/ballad2.mid");
//  if (!music) throw string("Couldn't load 100Years.mp3")+Mix_GetError();

//  startMusic();
/*  sounds.push_back( Mix_LoadWAV("sound/Boom.wav") );
  sounds.push_back( Mix_LoadWAV("sound/Gun.wav") );
  sounds.push_back( Mix_LoadWAV("sound/Thump.wav") );
  sounds.push_back( Mix_LoadWAV("sound/phaser.wav") );
  sounds.push_back( Mix_LoadWAV("sound/Putt1.wav") );
  sounds.push_back( Mix_LoadWAV("sound/explosion.wav") );
  sounds.push_back( Mix_LoadWAV("sound/cannon.wav") );
  sounds.push_back( Mix_LoadWAV("sound/Larc.wav") );
*/
  sounds.push_back( Mix_LoadWAV("sounds/evil_laugh.wav"));
  sounds.push_back( Mix_LoadWAV("sounds/incoming_suspense.wav"));
  sounds.push_back( Mix_LoadWAV("sounds/woosh.wav"));
  sounds.push_back( Mix_LoadWAV("sounds/jump.wav"));
  
  for (unsigned int i = 0; i < sounds.size(); ++i) channels.push_back(i);
}

void SDLSound::toggleMusic() {
  if( Mix_PausedMusic() ) { 
    Mix_ResumeMusic(); 
  } 
  else { 
    Mix_PauseMusic(); 
  } 
}

void SDLSound::stopChannel(int index){
  if( index >= -1 && index < (int) sounds.size())
    Mix_HaltChannel(index);
}

void SDLSound::operator[](int index) {
  currentSound = index;
  Mix_VolumeChunk(sounds[index], volume);
  channels[index] = 
     Mix_PlayChannel(channels[index], sounds[index], 0);
}

void SDLSound::startMusic() {
  Mix_VolumeMusic(volume);
  Mix_PlayMusic(music, -1);
}

void SDLSound::stopMusic() {
  Mix_HaltMusic();
  Mix_FreeMusic(music);
}

