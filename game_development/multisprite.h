#ifndef MULTISPRITE__H
#define MULTISPRITE__H
#include <string>
#include <vector>
#include "drawable.h"

class ExplodingSprite;

class MultiSprite : public Drawable {
public:
  MultiSprite(const std::string&, const std::vector<Frame*>&);
  virtual ~MultiSprite(); 

  virtual void draw() const;
  virtual void update(Uint32 ticks);
  virtual const Frame* getFrame() const { 
    return frames[currentFrame]; 
  }

  void setCurrentFrame(unsigned int idx){
    if(idx < numberOfFrames){
      currentFrame = idx;
    }
  }
  void explode();
  void setVisibility(bool visible){
    this->visible = visible;
  }
  void setCollided(bool collided){
    this->collided = collided;
  }
  bool alreadyCollided() const;
protected:
  ExplodingSprite* explosion;
  bool visible;
  bool collided;
  const std::vector<Frame *> frames;
  int worldWidth;
  int worldHeight;

  unsigned currentFrame;
  unsigned numberOfFrames;
  unsigned frameInterval;
  float timeSinceLastFrame;
  int frameWidth;
  int frameHeight;
  void advanceFrame(Uint32 ticks);
  MultiSprite(const MultiSprite&);
  MultiSprite& operator=(const MultiSprite&);
};
#endif
