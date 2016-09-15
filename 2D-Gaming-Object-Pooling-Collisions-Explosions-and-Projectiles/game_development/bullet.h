#ifndef BULLET__H
#define BULLET__H
#include <string>
#include "drawable.h"
#include "collisionStrategy.h"

class Bullet : public Drawable {
public:
  Bullet(const std::string&, 
         const Vector2f& pos, const Vector2f& vel, const Frame*);

  Bullet(const Bullet& s);
  virtual ~Bullet() ; 
  Bullet& operator=(const Bullet&);

  virtual const Frame* getFrame() const { return frame; }
  virtual void draw() const;

  virtual void update(Uint32 ticks);
  bool goneTooFar();
  bool collidedWith(const Drawable* d) const;
  void setTooFar(bool far);
  void reset(Vector2f pos, Vector2f vel);
private:
  const Frame * frame;
  Vector2f startPos;
  bool tooFar;
  CollisionStrategy * strategy;
  int frameWidth;
  int frameHeight;
  int worldWidth;
  int worldHeight;
  int getDistance(const Bullet*) const;
};
#endif
