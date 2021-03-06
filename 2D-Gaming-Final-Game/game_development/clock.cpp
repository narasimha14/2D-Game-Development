#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include "clock.h"
#include "gamedata.h"
#include "ioManager.h"

Clock& Clock::getInstance() {
  if ( SDL_WasInit(SDL_INIT_VIDEO) == 0) {
    throw std::string("Must init SDL before Clock");
  }
  static Clock clock; 
  return clock;
}

Clock::Clock() :
  ticks(0),
  started(false), 
  paused(false), 
  sloMo(false), 
  sumOfTicks(0),
  totalTicks(0),
  timeElapsed(0),
  frames(0),
  pos( Gamedata::getInstance().getXmlInt("clock/locX"),
       Gamedata::getInstance().getXmlInt("clock/locY"))
{
    start();
}

void Clock::reset() {
  ticks = 0;
  started = true;
  paused = false;
  sloMo = false;
  timeElapsed = 0; 
  frames = 0;
}

void Clock::draw() const { 
  IOManager::getInstance().printMessageAt("Jungle Book: Return of the Sher Khan", pos[0]+ 270, pos[1]+5);
  IOManager::getInstance().printMessageAt("By Narasimha Murthy", pos[0]+ 360, pos[1]+30);
}

void Clock::update() { 
  if(paused){
    ticks = 0;
    return;
  }
  ++frames;
  if(sloMo){
    ticks = 1;
    totalTicks = SDL_GetTicks();
    timeElapsed += totalTicks - sumOfTicks;
    sumOfTicks += totalTicks - sumOfTicks;
    return;
  }
  totalTicks = SDL_GetTicks();
  ticks = totalTicks - sumOfTicks;
  sumOfTicks += ticks;
  timeElapsed += ticks;
}

unsigned int Clock::getTicksSinceLastFrame() const {
  return ticks;
}

void Clock::toggleSloMo() {
  sloMo = !sloMo;
}

int Clock::getFps() const { 
  if (timeElapsed != 0 && ticks != 0){
    return frames*1000/timeElapsed;
  }
  return 0;
}

int Clock::getTimeElapsed() const {
  return timeElapsed/1000;
}

void Clock::start() { 
  started = true; 
  paused = false; 
}

void Clock::pause() { 
  ticks = 0;
  paused = true;
}

void Clock::unpause() { 
  totalTicks = SDL_GetTicks();
  sumOfTicks = totalTicks;
  paused = false;
}

