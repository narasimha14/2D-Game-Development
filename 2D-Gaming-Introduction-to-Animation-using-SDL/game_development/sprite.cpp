#include <iostream>
#include <cmath>
#include "sprite.h"
#include "gamedata.h"

Sprite::Sprite(const std::string& name, const Frame* const frame) :
  Drawable(name,
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y") + (int)Gamedata::getInstance().getRandInRange(20, 80)), 
           Vector2f((Gamedata::getInstance().getXmlInt(name+"/speedX")), 
                    (Gamedata::getInstance().getXmlInt(name+"/speedY")))
          ),
  frame( frame ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ 
}

Sprite::Sprite(const Sprite& s) :
  Drawable(s),
  frame(s.frame),  // shallow copy; Manager may reuse it
  frameWidth(s.getFrame()->getWidth()),
  frameHeight(s.getFrame()->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{
  setVelocity(Vector2f( Gamedata::getInstance().getXmlInt(getName()+"/speedX") != 0 ? (((int)Gamedata::getInstance().getRandInRange(100, 3000)) % 2 == 1 ? Gamedata::getInstance().getXmlInt(getName()+"/speedX") + Gamedata::getInstance().getRandInRange(100, 300) : -Gamedata::getInstance().getXmlInt(getName()+"/speedX") - Gamedata::getInstance().getRandInRange(0, 300)) : 0,
                        Gamedata::getInstance().getXmlInt(getName()+"/speedY") != 0 ? (((int)Gamedata::getInstance().getRandInRange(100, 3000)) % 2 == 1 ? Gamedata::getInstance().getXmlInt(getName()+"/speedY") + Gamedata::getInstance().getRandInRange(100, 300) : -Gamedata::getInstance().getXmlInt(getName()+"/speedY") - Gamedata::getInstance().getRandInRange(0, 300)) : 0 ) );
}

Sprite& Sprite::operator=(const Sprite& rhs) {
  Drawable::operator=(rhs);
  frame = rhs.frame;  // shallow copy; Manager may reuse it
  frameWidth = rhs.frameWidth;
  frameHeight = rhs.frameHeight;
  worldWidth = rhs.worldWidth;
  worldHeight = rhs.worldHeight;
  return *this;
}

void Sprite::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y); 
}

void Sprite::update(Uint32 ticks) { 
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);

  if ( Y() < 0) {
    velocityY( std::abs( velocityY() ) );
  }
  if ( Y() > worldHeight-frameHeight) {
    velocityY( -std::abs( velocityY() ) );
  }

  if ( X() < 0) {
    velocityX( std::abs( velocityX() ) );
  }
  if ( X() > worldWidth-frameWidth) {
    velocityX( -std::abs( velocityX() ) );
  }  
}
