#include <cmath>
#include "sprite.h"
#include "gamedata.h"
#include "explodingSprite.h"
#include "staticsprite.h"

StaticSprite::StaticSprite(const string& n, const Vector2f& pos, const Vector2f& vel,
               const Frame* frm):
  Drawable(n, Vector2f( (int)Gamedata::getInstance().getRandInRange(pos[0], Gamedata::getInstance().getXmlInt("world/width") - Gamedata::getInstance().getXmlInt("banana/offsetX")), pos[1]) , vel), 
  explosion(NULL),
  visible(true),
  collided(false),
  frame( frm ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

StaticSprite::StaticSprite(const StaticSprite& s) :
  Drawable(s), 
  explosion(s.explosion),
  visible(s.visible),
  collided(s.collided),
  frame(s.frame),
  frameWidth(s.getFrame()->getWidth()),
  frameHeight(s.getFrame()->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

StaticSprite& StaticSprite::operator=(const StaticSprite& rhs) {
  Drawable::operator=( rhs );
  explosion = rhs.explosion;
  visible = rhs.visible;
  collided = rhs.collided;
  frame = rhs.frame;
  frameWidth = rhs.frameWidth;
  frameHeight = rhs.frameHeight;
  worldWidth = rhs.worldWidth;
  worldHeight = rhs.worldHeight;
  return *this;
}

void StaticSprite::draw() const { 
  if(! visible) return;
  if(explosion){
    explosion->draw();
    return;
  }
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y); 
}

void StaticSprite::setVisibility(bool visible){
  this->visible = visible;
}

void StaticSprite::setCollided(bool collided){
  this->collided = collided;
}

bool StaticSprite::alreadyCollided() const{
  return collided;
}

void StaticSprite::explode() {
  if( ! visible ) return;
  if ( explosion ) return;
  Sprite sprite(getName(), getPosition(), getVelocity(), getFrame());
  explosion = new ExplodingSprite(sprite);
}

int StaticSprite::getDistance(const StaticSprite *obj) const { 
  return hypot(X()-obj->X(), Y()-obj->Y());
}

void StaticSprite::update(Uint32 ticks) { 
  if(! visible) return;
  if ( explosion ) {
    explosion->update(ticks);
    if ( explosion->chunkCount() == 0 ) {
      delete explosion;
      explosion = NULL;
    }
    return;
  }
}
