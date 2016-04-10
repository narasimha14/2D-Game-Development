#include "tigersprite.h"
#include "explodingSprite.h"
#include "gamedata.h"

TigerSprite::~TigerSprite() {
  delete strategy;
}

TigerSprite::TigerSprite( const std::string& name, 
                          const std::vector<Frame*>& f, const std::string& name2, const std::vector<Frame*>& f2, const Frame* const f3, const PlayerSprite& player) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
           ),
  explosion( NULL),
  visible(false),
  frames( f ),
  roarFrames( f2 ),
  deadFrame(f3),
  enemy( player ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

  currentFrame(0),
  roarFrameIdx(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  numberOfRoarFrames( Gamedata::getInstance().getXmlInt(name2+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  roarFrameInterval( Gamedata::getInstance().getXmlInt(name2+"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight()), 
  roarFrameWidth(roarFrames[0]->getWidth()),
  roarFrameHeight(roarFrames[0]->getHeight()), 
  offsetX( Gamedata::getInstance().getXmlInt(name+"/offsetX") ),
  offsetY( Gamedata::getInstance().getXmlInt(name+"/offsetY") ),
  flip(false),
  forward(false),
  backward(false),
  roars(false),
  noIncr(false),
  changeFrame(false),
  dead(false),
  strategy( new PerPixelCollisionStrategy)
{ }

void TigerSprite::reset(){
  setVisibility(false);
  setPosition( Vector2f(Gamedata::getInstance().getXmlInt( getName()+"/startLoc/x"),
                    Gamedata::getInstance().getXmlInt( getName() +"/startLoc/y")) );

  flip = false ;
  forward = false;
  backward = false;
  roars = false;
  noIncr = false;
  changeFrame = false;
  roarFrameIdx = 0;
  resetFrame();
  dead = false;
}

int TigerSprite::getFrameWidth() const{
  return frameWidth;
}

int TigerSprite::getOffsetX() const {
  return offsetX;
}

bool TigerSprite::collidedWith(const Drawable* d) const {
  return strategy->execute(*this, *d);
}

void TigerSprite::setVisibility(bool visible){
  this->visible = visible;
}

void TigerSprite::setChangeFrame(bool change){
  changeFrame = change;
}

void TigerSprite::setDead(bool mDead){
  dead = mDead;
  noIncr = mDead;
}

void TigerSprite::advanceFrame(Uint32 ticks) {
  timeSinceLastFrame += ticks;
  if (timeSinceLastFrame > frameInterval) {
    if(dead){
      return;
    }
    if(roars || (!forward && !backward && changeFrame)){
      if(flip)
        roarFrameIdx = (roarFrameIdx-1) % (numberOfRoarFrames/2) + numberOfRoarFrames/2;
      else
        roarFrameIdx = (roarFrameIdx+1) % (numberOfRoarFrames/2);
    }else{
      roarFrameIdx = 0;
      if(flip){
        currentFrame = (currentFrame-1) % (numberOfFrames/2) + (numberOfFrames/2);
      }else{
        currentFrame = (currentFrame+1) % (numberOfFrames/2);
      }
    }
    timeSinceLastFrame = 0;
  }
}

void TigerSprite::resetFrame(){
  setVelocity( Vector2f( std::abs(getVelocity()[0]), std::abs( getVelocity()[1]) ));
  currentFrame = 0;
}


void TigerSprite::draw() const { 
  if (explosion) {
    explosion->draw();
    return;
  }
  if( ! visible) return;
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  if(dead){
    deadFrame->draw(x,y);
  }else if(! enemy.getVisibility()){
    frames[0]->draw(x,y);
  }
  else if(roars || (!forward && !backward && changeFrame) ){
    roarFrames[roarFrameIdx]->draw(x,y);
  }else{
    frames[currentFrame]->draw(x, y);
  }
}

void TigerSprite::explode(){
  if ( explosion ) return;
  Sprite sprite(getName(), getPosition(), getVelocity(), getFrame());
  explosion = new ExplodingSprite(sprite);
}

void TigerSprite::moveForward() {
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
  if(roars){
    roars = false;
    noIncr = false;
  }
}

void TigerSprite::moveBackward() {
  flip = true;
  if(! backward){
    backward = true;
    velocityX( -std::abs( velocityX() ) );
    noIncr = false;
  }
  if( X() < 0 +  Gamedata::getInstance().getXmlInt("view/width")){
   noIncr = true; 
  }
  forward = false;
  if(roars){
    roars = false;
    noIncr = false;
  }
}


void TigerSprite::roar(){
  /* If enemy is already dead, do nothing*/
  if(! enemy.getVisibility() ){
    roars = false;
    forward = false;
    backward = false;
    noIncr = true;
    return;
  }
  if(backward && roarFrameIdx == 0){
    roarFrameIdx = numberOfRoarFrames/2;
    forward = false;
  }else if(forward){
    backward = false;
  }

  roars = true;
  changeFrame = true;
  noIncr = true;
}

void TigerSprite::halt() {
  forward = false;
  backward = false;
  roars = false;
}

void TigerSprite::update(Uint32 ticks) { 
  if( explosion){
    explosion->update(ticks);
    if ( explosion->chunkCount() == 0 ) {
      delete explosion;
      explosion = NULL;
    }
    return;
  }
  if( ! visible) return;

  unsigned int ex = enemy.X();
  int distance = X() - ex;
  if(distance < 0){
    /* Approach the enemy until a certain distance and then roar/jump */
    if( std::abs(distance) > 250){
      moveForward();
    }else{
      roar();
    } 
  }else{
    /* Enemy is behind. So move backwards and then jump/roar after a certain distance */
    if(distance > 50){
      moveBackward();
    }else{
      roar();
    }
  }

  /* Make the tiger stand still*/
  if( !forward && !backward && !roars) {
    currentFrame = velocityX() < 0 ? numberOfFrames - 1 : 0;
    return;
  }

  /* Keep advancing the frames*/
  advanceFrame(ticks);

  /* Update the position w.r.t it's velocity */
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

  if ( X() < 0 + Gamedata::getInstance().getXmlInt("view/width")) {
    velocityX( std::abs( velocityX() ) );
    flip = false;
  }
  if ( X() > worldWidth-frameWidth - offsetX) {
    velocityX( -std::abs( velocityX() ) );
    flip = true;
  }

}
