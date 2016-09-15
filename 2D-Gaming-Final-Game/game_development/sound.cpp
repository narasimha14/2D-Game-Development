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
  volume(SDL_MIX_MAXVOLUME /*/4*/), 
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

  sounds.push_back( Mix_LoadWAV("sounds/evil_laugh.wav"));
  sounds.push_back( Mix_LoadWAV("sounds/incoming_suspense.wav"));
  sounds.push_back( Mix_LoadWAV("sounds/woosh.wav"));
  sounds.push_back( Mix_LoadWAV("sounds/jump.wav"));
  sounds.push_back( Mix_LoadWAV("sounds/girl_scream.wav"));
  
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

void SDLSound::playFightMusic(){
  Mix_FreeMusic(music);
  Mix_VolumeMusic(volume/4);
  music = Mix_LoadMUS("sounds/ugramm_cut.mp3");
  if(!music)
    throw string("Couldn't load ugramm_cut.mp3")+Mix_GetError();
  startMusic();
}

void SDLSound::playBackgroundMusic(){
  Mix_FreeMusic(music);
  Mix_VolumeMusic(volume*16);
  music = Mix_LoadMUS("sounds/we_have_a_romeo_2.mp3");
  if(!music)
    throw string("Couldn't load we_have_a_romeo_2.mp3")+Mix_GetError();
  startMusic();
}

void SDLSound::playEndMusic(){
  Mix_FreeMusic(music);
  Mix_VolumeMusic(volume*16);
  music = Mix_LoadMUS("sounds/beautiful.mp3");
  if(!music)
    throw string("Couldn't load beautiful.mp3")+Mix_GetError();
  startMusic();
}

void SDLSound::stopChannel(int index){
  if( index >= -1 && index < (int) sounds.size())
    Mix_HaltChannel(index);
}

void SDLSound::operator[](int index) {
  currentSound = index;
  Mix_VolumeChunk(sounds[index], volume/4);
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

