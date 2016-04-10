#ifndef PLAYERSPRITE__H
#define PLAYERSPRITE__H
#include <string>
#include <vector>
#include "drawable.h"
#include "bulletpool.h"

class ExplodingSprite;
class PlayerSprite : public Drawable {
public:
  PlayerSprite(const std::string&, const std::vector<Frame*>&, const std::string&, const std::vector<Frame*>&);
  virtual ~PlayerSprite(); 

  virtual void draw() const;
  void shoot();
  void moveForward() ;
  void moveBackward() ;
  void halt() ;
  virtual void update(Uint32 ticks);
  virtual const Frame* getFrame() const { 
    if(!forward && !backward && !jump){
      return frames2[secondFrameIdx];
    }
    return frames[currentFrame]; 
  }
  int getFrameWidth() const;
  int getOffsetX() const;
  bool collidedWith(const Drawable* d) const;  
  void incrBananaCount();
  void resetBananaCount();

  unsigned int getBananaCount() const;
  unsigned int getBulletListSize() const;
  unsigned int getFreeListSize() const;

  std::list<Bullet*>& getBulletList();

  void resetFrame();
  void reset();
  void explode(); 
  void setVisibility(bool visible);
  bool getVisibility() const;
  void setJump(bool mJump);
  void setChangeFrame(bool change);
  bool isJump() const;
  void setStartOffset(unsigned int val);
protected:
  ExplodingSprite * explosion;
  bool visible;
  const std::vector<Frame *> frames;
  const std::vector<Frame *> frames2;
  int worldWidth;
  int worldHeight;

  unsigned currentFrame;
  unsigned secondFrameIdx;
  unsigned numberOfFrames;
  unsigned numberOfSecondFrames;
  unsigned frameInterval;
  unsigned secondFrameInterval;
  float timeSinceLastFrame;
  int frameWidth;
  int frameHeight;
  int secondFrameWidth;
  int secondFrameHeight;
  int offsetX;
  int offsetY;
  unsigned int startOffset;
  unsigned int bananaCount;
  BulletPool* bulletPool;
  bool flip;
  bool forward;
  bool backward;
  bool jump;
  bool isFalling;
  int jumpedHeight;
  int maxJumpHeight;
  bool noIncr;
  bool changeFrame;
  void advanceFrame(Uint32 ticks);
  CollisionStrategy * strategy;
  PlayerSprite(const PlayerSprite&);
  PlayerSprite& operator=(const PlayerSprite&);
};
#endif
