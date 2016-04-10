#ifndef PLAYERSPRITE__H
#define PLAYERSPRITE__H
#include <string>
#include <vector>
#include "drawable.h"

class PlayerSprite : public Drawable {
public:
  PlayerSprite(const std::string&, const std::vector<Frame*>&);
  virtual ~PlayerSprite() { } 

  virtual void draw() const;
  void moveForward() ;
  void moveBackward() ;
  void halt() ;
  virtual void update(Uint32 ticks);
  virtual const Frame* getFrame() const { 
    return frames[currentFrame]; 
  }
  int getFrameWidth() const{
    return frameWidth;
  }
  int getOffsetX() const {
    return offsetX;
  } 
protected:
  const std::vector<Frame *> frames;
  int worldWidth;
  int worldHeight;

  unsigned currentFrame;
  unsigned numberOfFrames;
  unsigned frameInterval;
  float timeSinceLastFrame;
  int frameWidth;
  int frameHeight;
  int offsetX;
  int offsetY;
  bool flip;
  bool forward;
  bool backward;
  bool noIncr;
  void advanceFrame(Uint32 ticks);
  PlayerSprite(const PlayerSprite&);
  PlayerSprite& operator=(const PlayerSprite&);
};
#endif
