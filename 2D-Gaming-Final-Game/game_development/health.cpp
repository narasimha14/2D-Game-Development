#include "health.h"

Health::Health() :
  screen(IOManager::getInstance().getScreen()),
  start(Vector2f(Gamedata::getInstance().getXmlInt("health/locX"), Gamedata::getInstance().getXmlInt("health/locY")) ), 
  totalLength(200), 
  currentLength(100), 
  currentLength2(200), 
  thick(14), 
  increments(20),
  tigerEntry(false),
  godMode(false),
  interval(Gamedata::getInstance().getXmlInt("health/interval")),
  deltaTime(0),
  decrTime(0),
  decrRate(Gamedata::getInstance().getXmlInt("health/decrRate")),
  playerDecrTime(Gamedata::getInstance().getXmlInt("health/playerDecrTime")),
  RED( SDL_MapRGB(screen->format, 0xff, 0x00, 0x00) ),
  GRAY( SDL_MapRGB(screen->format, 0xce, 0xb4, 0xb4) ),
  BLACK( SDL_MapRGB(screen->format, 0x00, 0x00, 0x00) ),
  color(RED) {
}

Health::Health(int sx, int sy, int tl, int cl,
               float t, int inc, Uint32 c, float sp):
  screen(IOManager::getInstance().getScreen()),
  start(Vector2f(sx, sy)), 
  totalLength(tl), 
  currentLength(cl), 
  currentLength2(cl), 
  thick(t), 
  increments(inc),
  tigerEntry(false),
  godMode(false),
  interval(sp),
  deltaTime(SDL_GetTicks()),
  decrTime(SDL_GetTicks()),
  decrRate(Gamedata::getInstance().getXmlInt("health/decrRate")),
  playerDecrTime(Gamedata::getInstance().getXmlInt("health/playerDecrTime")),
  RED( SDL_MapRGB(screen->format, 0xff, 0x00, 0x00) ),
  GRAY( SDL_MapRGB(screen->format, 0xff, 0xff, 0xff) ),
  BLACK( SDL_MapRGB(screen->format, 0x00, 0x00, 0x00) ),
    color(c) {
}

void Health::incrHealth(){
  currentLength += increments;
  currentLength = std::min(currentLength, totalLength);
}

void Health::decrHealth(Uint32 ticks){
  if(godMode){
    decrTime = 0;
    return;
  }
  if (decrTime == 0 && currentLength > 0){
    currentLength -= increments;
  }
  decrTime += ticks;
  if(decrTime > playerDecrTime){
    decrTime = 0;
  }
}

void Health::decrHealth2(){
  if (currentLength2 > 0){
    currentLength2 -= increments/decrRate;
  }
}

bool Health::isZero(){
  return (currentLength == 0);
}

bool Health::isEnemyDead(){
  return (currentLength2 == 0);
}

void Health::setTigerEntry(bool val){
  tigerEntry = val;
}

void Health::drawBox(Vector2f startIdx) const {
  Draw_AALine(screen, startIdx[0], startIdx[1], 
                      startIdx[0]+totalLength, startIdx[1], 
                      thick, GRAY);
  // Two Horizontal lines
  Draw_AALine(screen, startIdx[0], startIdx[1]-8, 
                      startIdx[0]+totalLength, startIdx[1]-8, 
                      1.0, BLACK);
  Draw_AALine(screen, startIdx[0], startIdx[1]+8, 
                      startIdx[0]+totalLength, startIdx[1]+8, 
                      1.0, BLACK);
  // Two Vertical lines
  Draw_AALine(screen, startIdx[0]-1, startIdx[1]-8, 
                      startIdx[0]-1, startIdx[1]+8, 
                      2.0, BLACK);
  Draw_AALine(screen, startIdx[0]+totalLength+1, startIdx[1]-8, 
                      startIdx[0]+totalLength+1, startIdx[1]+8, 
                      2.0, BLACK);
}

void Health::draw() const {
  drawBox(start);
  Draw_AALine(screen, start[0], start[1], 
                      start[0]+currentLength, start[1], 
                      thick, color);
  if(tigerEntry && currentLength2 != 0){
    drawBox(start - Vector2f(600,0));
    Draw_AALine(screen, start[0] - 600, start[1], 
                        start[0] - 600 +currentLength2, start[1], 
                        thick, color);
  }

}
void Health::update(Uint32 ticks) {
  deltaTime += ticks;
  if(godMode){
    deltaTime = 0;
    return;
  }
  if ( currentLength > 0 && deltaTime > interval && !tigerEntry) {
    deltaTime = 0;
    currentLength -= increments;
  }
}
