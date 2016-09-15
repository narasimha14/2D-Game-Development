#include "playersprite.h"
#include "explodingSprite.h"
#include "gamedata.h"

PlayerSprite::~PlayerSprite() {
  delete strategy;
  delete bulletPool;
}

PlayerSprite::PlayerSprite( const std::string& name, 
                          const std::vector<Frame*>& f, const std::string& name2, const std::vector<Frame*>& f2) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
           ),
  explosion( NULL),
  visible(true),
  frames( f ),
  frames2( f2 ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

  currentFrame(0),
  secondFrameIdx(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  numberOfSecondFrames( Gamedata::getInstance().getXmlInt(name2+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  secondFrameInterval( Gamedata::getInstance().getXmlInt(name2+"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight()), 
  secondFrameWidth(frames2[0]->getWidth()),
  secondFrameHeight(frames2[0]->getHeight()), 
  offsetX( Gamedata::getInstance().getXmlInt(name+"/offsetX") ),
  offsetY( Gamedata::getInstance().getXmlInt(name+"/offsetY") ),
  bananaCount(0),
  bulletPool( new BulletPool(this) ),
  flip(false),
  forward(false),
  backward(false),
  jump(false),
  isFalling(false),
  jumpedHeight(0),
  maxJumpHeight( Gamedata::getInstance().getXmlInt(name+"/maxJumpHeight")),
  noIncr(false),
  changeFrame(false),
  strategy( new PerPixelCollisionStrategy)
{ }

void PlayerSprite::incrBananaCount(){
  ++bananaCount;
}

void PlayerSprite::resetBananaCount(){
  bananaCount = 0;
}

int PlayerSprite::getFrameWidth() const{
  return frameWidth;
}

int PlayerSprite::getOffsetX() const {
  return offsetX;
}

bool PlayerSprite::collidedWith(const Drawable* d) const {
  return strategy->execute(*this, *d);
}

unsigned int PlayerSprite::getBananaCount() const{
  return bananaCount;
}

unsigned int PlayerSprite::getBulletListSize() const{
  return bulletPool->getBulletListSize();
} 

unsigned int PlayerSprite::getFreeListSize() const{
  return bulletPool->getFreeListSize();
}

std::list<Bullet*>& PlayerSprite::getBulletList(){
  return bulletPool->getBulletList();
}

void PlayerSprite::setVisibility(bool visible){
  this->visible = visible;
}

void PlayerSprite::setJump(bool mJump){
  jump = mJump;
}

void PlayerSprite::setChangeFrame(bool change){
  changeFrame = change;
}

bool PlayerSprite::isJump() const{
  return jump;
}

void PlayerSprite::advanceFrame(Uint32 ticks) {
  timeSinceLastFrame += ticks;
  if (timeSinceLastFrame > frameInterval) {
    if(!forward && !backward && !jump && changeFrame){
      secondFrameIdx = (secondFrameIdx+1) % numberOfSecondFrames;
    }else{
      secondFrameIdx = 0;
      if(flip){
        currentFrame = (currentFrame-1) % (numberOfFrames/2) + (numberOfFrames/2);
      }else{
        currentFrame = (currentFrame+1) % (numberOfFrames/2);
      }
    }
    timeSinceLastFrame = 0;
  }
}

void PlayerSprite::resetFrame(){
  setVelocity( Vector2f( std::abs(getVelocity()[0]), std::abs( getVelocity()[1]) ));
  currentFrame = 0;
}


void PlayerSprite::draw() const { 
  bulletPool->draw();
  if (explosion) {
    explosion->draw();
    return;
  }
  if( ! visible) return;
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  if(!forward && !backward && !jump && changeFrame){
    frames2[secondFrameIdx]->draw(x,y - 40);
  }else{
    frames[currentFrame]->draw(x, y);
  }
}

void PlayerSprite::explode(){
  if ( explosion ) return;
  Sprite sprite(getName(), getPosition(), getVelocity(), getFrame());
  explosion = new ExplodingSprite(sprite);
}


void PlayerSprite::shoot() {
  if( explosion ) return;
  bulletPool->shoot();
}

void PlayerSprite::moveForward() {
  flip = false;
  if(! forward){
    forward = true;
    velocityX( std::abs( velocityX() ) );
    noIncr = false;
  }
  if(X() > worldWidth-frameWidth - offsetX){
    noIncr = true;
  }
  backward = false;
}

void PlayerSprite::moveBackward() {
  flip = true;
  if(! backward){
    backward = true;
    velocityX( -std::abs( velocityX() ) );
    noIncr = false;
  }
  if( X() < 0){
   noIncr = true; 
  }
  forward = false;
}

void PlayerSprite::halt() {
  forward = false;
  backward = false;
}

void PlayerSprite::reset() {
  resetBananaCount();
  setVisibility(true);
  setPosition( Vector2f(Gamedata::getInstance().getXmlInt( getName()+"/startLoc/x"),
                    Gamedata::getInstance().getXmlInt( getName() +"/startLoc/y")) );
  setJump(false);
  isFalling = false;
  jumpedHeight = 0;
  resetFrame();
}

void PlayerSprite::update(Uint32 ticks) { 
  bulletPool->process(ticks);
  if( explosion){
    explosion->update(ticks);
    if ( explosion->chunkCount() == 0 ) {
      delete explosion;
      explosion = NULL;
    }
    return;
  }
  if( ! visible) return;
  if(jump){
    if(isFalling){
      ++jumpedHeight;
      if(jumpedHeight < maxJumpHeight/5)
        setPosition( getPosition() + Vector2f(0,5));
      else{
        jump = false;
        isFalling = false;
        jumpedHeight = 0;
      }
    }else{
      ++jumpedHeight;
      if( jumpedHeight < maxJumpHeight/5){
        setPosition( getPosition() - Vector2f(0,5));
      }else{
        jumpedHeight = 0;
        isFalling = true;
      }
    }
  }

  advanceFrame(ticks);
  if( !forward && !backward) {
    if(! jump)
      currentFrame = velocityX() < 0 ? numberOfFrames - 1 : 0;
    return;
  }
  if(! noIncr){
    Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
    setPosition(getPosition() + incr);
  }

  if ( Y() < 0) {
    velocityY( std::abs( velocityY() ) );
  }
  if ( Y() > worldHeight-frameHeight - offsetY) {
    velocityY( -std::abs( velocityY() ) );
  }

  if ( X() < 0) {
    velocityX( std::abs( velocityX() ) );
    flip = false;
  }
  if ( X() > worldWidth-frameWidth - offsetX) {
    velocityX( -std::abs( velocityX() ) );
    flip = true;
  }

}
