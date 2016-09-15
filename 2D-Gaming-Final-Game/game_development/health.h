#include "vector2f.h"
#include "ioManager.h"
#include "aaline.h"
#include "gamedata.h"
class Health {
public:
  Health();
  Health(int sx, int sy, int tl, int cl, 
         float t, int inc, Uint32 c, float sp);
  void draw() const;
  void update(Uint32);
  void incrHealth();
  void decrHealth(Uint32 ticks);
  void decrHealth2();
  bool isZero();
  bool isEnemyDead();
  void setTigerEntry(bool val);
  void setGodMode(bool val){
    godMode = val;
  }
  void reset() { 
    currentLength = totalLength/2; 
    currentLength2 = totalLength; 
    tigerEntry = false;
  }
private:
  SDL_Surface* screen;
  Vector2f start;
  int totalLength;
  int currentLength;
  int currentLength2;
  int thick;
  int increments;
  bool tigerEntry;
  bool godMode;
  float interval;
  int deltaTime;
  int decrTime;
  int decrRate;
  int playerDecrTime;
  const Uint32 RED;
  const Uint32 GRAY;
  const Uint32 BLACK;
  const Uint32 color;
  void drawBox(Vector2f startIdx) const;
  Health(const Health&);
  Health& operator=(const Health&);
};
