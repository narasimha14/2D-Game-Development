#include "twowaymultisprite.h"
#include "gamedata.h"

void TwoWayMultiSprite::advanceFrame(Uint32 ticks) {
  timeSinceLastFrame += ticks;
  if (timeSinceLastFrame > frameInterval) {
    if(flip){
      currentFrame = (currentFrame-1) % (numberOfFrames/2) + (numberOfFrames/2);
    }else{
      currentFrame = (currentFrame+1) % (numberOfFrames/2);
    }
    timeSinceLastFrame = 0;
  }
}

TwoWayMultiSprite::TwoWayMultiSprite( const std::string& name, 
                          const std::vector<Frame*>& f) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
           ),
  frames( f ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight()), 
  offsetX( Gamedata::getInstance().getXmlInt(name+"/offsetX") ),
  offsetY( Gamedata::getInstance().getXmlInt(name+"/offsetY") ),
  flip(false)
{ }

void TwoWayMultiSprite::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frames[currentFrame]->draw(x, y);
}

void TwoWayMultiSprite::update(Uint32 ticks) { 
  advanceFrame(ticks);
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);

  if ( Y() < 0) {
    velocityY( abs( velocityY() ) );
  }
  if ( Y() > worldHeight-frameHeight - offsetY) {
    velocityY( -abs( velocityY() ) );
  }

  if ( X() < 0) {
    velocityX( abs( velocityX() ) );
    flip = false;
  }
  if ( X() > worldWidth-frameWidth - offsetX) {
    velocityX( -abs( velocityX() ) );
    flip = true;
  }

}
