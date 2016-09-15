#include <iostream>
#include <cmath>
#include "SDL/SDL_rotozoom.h"
#include "scaledSprite.h"
#include "gamedata.h"
#include "explodingSprite.h"

ScaledSprite::ScaledSprite(const std::string& name, 
                           SDL_Surface* surface) :
  Drawable(name,
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x") + (int)Gamedata::getInstance().getRandInRange(0, 1920), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y") + (int)Gamedata::getInstance().getRandInRange(0, 500)), 
           Vector2f(Gamedata::getInstance().getRandInRange(
                    Gamedata::getInstance().getXmlInt(name+"/cushionX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedX")),
                    Gamedata::getInstance().getRandInRange(
                    Gamedata::getInstance().getXmlInt(name+"/cushionY"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
                   )
  ), 
  scale(Gamedata::getInstance().getRandFloat(Gamedata::getInstance().getXmlFloat(name+"/scale/min"),
                     Gamedata::getInstance().getXmlFloat(name+"/scale/max"))
  ),
  scaledSurface( rotozoomSurface(surface, 0, scale, SMOOTHING_ON) ),
  explosion(NULL),
  frame( new Frame(scaledSurface, scaledSurface->w, scaledSurface->h,
                   Gamedata::getInstance().getXmlInt(name+"/srcX"), 
                   Gamedata::getInstance().getXmlInt(name+"/srcY"))
  ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

ScaledSprite::ScaledSprite(const ScaledSprite& s) :
  Drawable(s.getName(), s.getPosition(), s.getVelocity()), 
  scale(s.scale),
  scaledSurface(s.scaledSurface),
  explosion(s.explosion),
  frame(s.frame),
  frameWidth(s.frameWidth),
  frameHeight(s.frameHeight),
  worldWidth(s.worldWidth),
  worldHeight(s.worldHeight)
{ }

ScaledSprite& ScaledSprite::operator=(const ScaledSprite& rhs) {
  setName( rhs.getName() );
  setPosition(rhs.getPosition());
  setVelocity(rhs.getVelocity());
  scale = rhs.scale;
  scaledSurface = rhs.scaledSurface;
  explosion = rhs.explosion;
  frame = rhs.frame;
  frameWidth = rhs.frameWidth;
  frameHeight = rhs.frameHeight;
  worldWidth = rhs.worldWidth;
  worldHeight = rhs.worldHeight;
  return *this;
}

ScaledSprite::~ScaledSprite() {
  SDL_FreeSurface( scaledSurface );
  delete frame;
  delete explosion;
}

void ScaledSprite::draw() const { 
  if(explosion){
    explosion->draw();
    return;
  }
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y); 
}

unsigned ScaledSprite::getPixel(Uint32 i, Uint32 j) const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  x = i - x;
  y = j - y;
  Uint32 *pixels = static_cast<Uint32 *>(frame->getSurface()->pixels);
  return pixels[ ( y * frame->getWidth() ) + x ];
}

void ScaledSprite::explode(){
  if ( explosion ) return;
  Sprite sprite(getName(), getPosition(), getVelocity(), getFrame());
  explosion = new ExplodingSprite(sprite);
}

void ScaledSprite::update(Uint32 ticks) {
  if ( explosion ) {
    explosion->update(ticks);
    if ( explosion->chunkCount() == 0 ) {
      delete explosion;
      explosion = NULL;
    }
    return;
  } 
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);

  if ( Y() < 0) {
    velocityY( std::abs( velocityY() ) );
  }
  if ( Y() > worldHeight-frameHeight) {
    //velocityY( -std::abs( velocityY() ) );
    // Allowing for leaf to re-start from top instead of bouncing
    setPosition(Vector2f( getPosition()[0], 10 ));
  }

  if ( X() < 0) {
    velocityX( std::abs( velocityX() ) );
  }
  if ( X() > worldWidth-frameWidth) {
    velocityX( -std::abs( velocityX() ) );
  }  
}
