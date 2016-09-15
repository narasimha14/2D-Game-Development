#ifndef SCALEDSPRITE__H
#define SCALEDSPRITE__H

#include <string>
#include <iostream>

#include "drawable.h"
class ExplodingSprite;
class ScaledSprite : public Drawable {
public:
  ScaledSprite(const std::string&, SDL_Surface*);
  ScaledSprite(const ScaledSprite& s);
  ~ScaledSprite();

  ScaledSprite& operator=(const ScaledSprite&);
  bool operator<(const ScaledSprite& rhs) const {
    return scale < rhs.scale;
  }

  virtual const Frame* getFrame() const { return frame; }
  virtual void draw() const;
  virtual void update(Uint32 ticks);
  unsigned getPixel(Uint32, Uint32) const;
  float getScale() const { return scale; }

  Vector2f getCenter() const { 
    return Vector2f( X()+frame->getWidth()/2, Y()+frame->getHeight()/2 );
  }
  void explode();
private:
  double scale;
  SDL_Surface* scaledSurface;
  ExplodingSprite* explosion;
  const Frame * frame;
  int frameWidth;
  int frameHeight;
  int worldWidth;
  int worldHeight;
};
#endif
