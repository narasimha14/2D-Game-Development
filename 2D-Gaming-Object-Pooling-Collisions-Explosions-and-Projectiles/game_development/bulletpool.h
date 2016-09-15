#include "bullet.h"
#include <list>
#include "drawable.h"

class BulletPool{
public:
  BulletPool(Drawable* player);
  ~BulletPool();
  Bullet* getBullet();
  void draw();
  void shoot();
  void process(Uint32);
  unsigned int getBulletListSize() const;
  unsigned int getFreeListSize() const;
  std::list<Bullet*>& getBulletList();
private:
  std::list<Bullet*> bulletList;
  std::list<Bullet*> freeList;
  const Frame* const bulletFrame;
  Drawable* player;
  BulletPool(const BulletPool&);
  BulletPool& operator=(const BulletPool&);
};
