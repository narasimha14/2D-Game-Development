#include "multisprite.h"
#include "gamedata.h"
#include "explodingSprite.h"
#include "frameFactory.h"
#include <cmath>

MultiSprite::~MultiSprite(){
  delete explosion;
}
void MultiSprite::advanceFrame(Uint32 ticks) {
  timeSinceLastFrame += ticks;
  if (timeSinceLastFrame > frameInterval) {
    currentFrame = (currentFrame+1) % numberOfFrames;
    timeSinceLastFrame = 0;
  }
}

MultiSprite::MultiSprite( const std::string& name, 
                          const std::vector<Frame*>& f, const Frame* const f2) :
  Drawable(name, 
           Vector2f( Gamedata::getInstance().getXmlBool(name+"/randomize") ? Gamedata::getInstance().getRandInRange( Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), Gamedata::getInstance().getXmlInt("world/width") - Gamedata::getInstance().getXmlInt( name + "/offsetX") ) : Gamedata::getInstance().getXmlInt(name+"/startLoc/x") , 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
           ),
  explosion( NULL ),
  visible( true ),
  collided( false ),
  end( false ),
  frames( f ),
  endFrame( f2 ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight())
{ }

MultiSprite::MultiSprite( const std::string& name, 
                          const std::vector<Frame*>& f) :
  Drawable(name, 
           Vector2f( Gamedata::getInstance().getXmlBool(name+"/randomize") ? Gamedata::getInstance().getRandInRange( Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), Gamedata::getInstance().getXmlInt("world/width") - Gamedata::getInstance().getXmlInt( name + "/offsetX") ) : Gamedata::getInstance().getXmlInt(name+"/startLoc/x") , 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
           ),
  explosion( NULL ),
  visible( true ),
  collided( false ),
  end( false ),
  frames( f ),
  endFrame( ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight())
{ }

void MultiSprite::draw() const { 
  if (explosion) {
    explosion->draw();
    return;
  }
  if(! visible) return;
  
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  if(end){
    endFrame->draw(x,y);
    return;
  }
  frames[currentFrame]->draw(x, y);
}

void MultiSprite::explode(){
  if ( explosion ) return;
  Sprite sprite(getName(), getPosition(), getVelocity(), getFrame());
  explosion = new ExplodingSprite(sprite);
}

bool MultiSprite::alreadyCollided() const{
  return collided;
}

void MultiSprite::setEnd(bool mEnd){
  end = mEnd;
}

void MultiSprite::update(Uint32 ticks) {
  if( explosion){
    explosion->update(ticks);
    if ( explosion->chunkCount() == 0 ) {
      delete explosion;
      explosion = NULL;
    }
    return;
  } 
  if(! visible) return;
  advanceFrame(ticks);
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
