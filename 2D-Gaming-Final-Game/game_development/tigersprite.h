#ifndef TIGERSPRITE__H
#define TIGERSPRITE__H
#include <string>
#include <vector>
#include "drawable.h"
#include "playersprite.h"

class ExplodingSprite;
class TigerSprite : public Drawable {
public:
  TigerSprite(const std::string&, const std::vector<Frame*>&, const std::string&, const std::vector<Frame*>&, const Frame * const ,const PlayerSprite& player);
  virtual ~TigerSprite(); 

  virtual void draw() const;
  void moveForward() ;
  void moveBackward() ;
  void roar();
  void halt() ;
  virtual void update(Uint32 ticks);
  virtual const Frame* getFrame() const { 
    if(!forward && !backward ){
      return roarFrames[roarFrameIdx];
    }
    return frames[currentFrame]; 
  }
  int getFrameWidth() const;
  int getOffsetX() const;
  bool collidedWith(const Drawable* d) const;  

  bool getVisibility()const{
    return visible;
  }
  void setDead(bool);
  void resetFrame();
  void reset();
  void explode(); 
  void setVisibility(bool visible);
  void setChangeFrame(bool change);

protected:
  ExplodingSprite * explosion;
  bool visible;
  const std::vector<Frame *> frames;
  const std::vector<Frame *> roarFrames;
  const Frame * deadFrame;
  const PlayerSprite& enemy;
  int worldWidth;
  int worldHeight;

  unsigned currentFrame;
  unsigned roarFrameIdx;
  unsigned numberOfFrames;
  unsigned numberOfRoarFrames;
  unsigned frameInterval;
  unsigned roarFrameInterval;
  float timeSinceLastFrame;
  int frameWidth;
  int frameHeight;
  int roarFrameWidth;
  int roarFrameHeight;
  int offsetX;
  int offsetY;
  bool flip;
  bool forward;
  bool backward;
  bool roars;
  bool noIncr;
  bool changeFrame;
  bool dead;
  void advanceFrame(Uint32 ticks);
  CollisionStrategy * strategy;
  TigerSprite(const TigerSprite&);
  TigerSprite& operator=(const TigerSprite&);
};
#endif
