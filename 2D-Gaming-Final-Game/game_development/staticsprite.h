#ifndef STATICSPRITE__H
#define STATICSPRITE__H
#include <string>
#include "drawable.h"

class ExplodingSprite;

class StaticSprite : public Drawable {
public:
  StaticSprite(const std::string&, 
         const Vector2f& pos, const Vector2f& vel, const Frame*);

  StaticSprite(const StaticSprite& s);
  virtual ~StaticSprite() { } 
  StaticSprite& operator=(const StaticSprite&);

  virtual const Frame* getFrame() const { return frame; }
  virtual void draw() const;

  virtual void update(Uint32 ticks);
  void explode();
  void setVisibility(bool visible);
  void setCollided(bool collided);
  bool alreadyCollided() const;
private:
  ExplodingSprite * explosion;
  bool visible;
  bool collided;
  const Frame * frame;
  int frameWidth;
  int frameHeight;
  int worldWidth;
  int worldHeight;
  int getDistance(const StaticSprite*) const;
};
#endif
