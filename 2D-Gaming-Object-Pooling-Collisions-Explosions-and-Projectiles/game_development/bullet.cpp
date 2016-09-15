#include <cmath>
#include "bullet.h"
#include "gamedata.h"

Bullet::~Bullet(){ 
  delete strategy;
} 

Bullet::Bullet(const string& n, const Vector2f& pos, const Vector2f& vel,
               const Frame* frm):
  Drawable(n, Vector2f( pos[0] , pos[1] ), vel), 
  frame( frm ),
  startPos(pos),
  tooFar(false),
  strategy(new PerPixelCollisionStrategy),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }
/*
Bullet::Bullet(const string& n, const Vector2f& pos, const Vector2f& vel):
  Drawable(n, pos, vel),
  frame( FrameFactory::getInstance().getFrame(n) ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }
*/
Bullet::Bullet(const Bullet& s) :
  Drawable(s), 
  frame(s.frame),
  startPos(s.startPos),
  tooFar(s.tooFar),
  strategy(s.strategy),
  frameWidth(s.getFrame()->getWidth()),
  frameHeight(s.getFrame()->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

Bullet& Bullet::operator=(const Bullet& rhs) {
  Drawable::operator=( rhs );
  frame = rhs.frame;
  startPos = rhs.startPos;
  tooFar = rhs.tooFar;
  strategy = rhs.strategy;
  frameWidth = rhs.frameWidth;
  frameHeight = rhs.frameHeight;
  worldWidth = rhs.worldWidth;
  worldHeight = rhs.worldHeight;
  return *this;
}

void Bullet::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y); 
}

bool Bullet::goneTooFar(){
  return tooFar; 
}

bool Bullet::collidedWith(const Drawable* d) const {
  return strategy->execute(*this, *d);
}

void Bullet::setTooFar(bool far){
  tooFar = far;
}

void Bullet::reset(Vector2f pos, Vector2f vel){ 
  startPos = pos;
  setPosition(pos);
  setVelocity(vel);
  tooFar = false;
}

int Bullet::getDistance(const Bullet *obj) const { 
  return hypot(X()-obj->X(), Y()-obj->Y());
}

void Bullet::update(Uint32 ticks) { 
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
  //Not required for this bullet
  /*if ( Y() < 0) {
    velocityY( abs( velocityY() ) );
  }
  if ( Y() > worldHeight-frameHeight) {
    // Allowing for leaf to re-start from top instead of bouncing
    setPosition(Vector2f( getPosition()[0], 10 ));
  }*/

  if ( X() < 0 || X() > worldWidth - frameWidth || X() - startPos[0] > 410 || tooFar) {
    tooFar = true;
  }else{
    tooFar = false;
  }  
}
