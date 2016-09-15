#include "bulletpool.h"
#include "frameFactory.h"
BulletPool::~BulletPool(){
  std::list<Bullet*>::iterator ptr = bulletList.begin();
  while(ptr != bulletList.end()){
    delete *ptr;
    ++ptr;
  }
  bulletList.clear();
  std::list<Bullet*>::iterator ptr2 = freeList.begin();
  while(ptr2 != freeList.end()){
    delete *ptr2;
    ++ptr2;
  }
  freeList.clear();
}

BulletPool::BulletPool(Drawable* playerSprite) :
  bulletList(),
  freeList(),
  bulletFrame( FrameFactory::getInstance().getFrame("bullet") ),
  player(playerSprite){

}
 
void BulletPool::shoot() {
  getBullet()->draw();
}

unsigned int BulletPool::getBulletListSize() const{
  return bulletList.size();
}

unsigned int BulletPool::getFreeListSize() const{
  return freeList.size();
}

std::list<Bullet*>& BulletPool::getBulletList(){
  return bulletList;
}

void BulletPool::draw(){
  std::list<Bullet*>::iterator ptr = bulletList.begin();
  while(ptr != bulletList.end()){
    (*ptr)->draw();
    ++ptr;
  }
}

void BulletPool::process(Uint32 ticks){
  std::list<Bullet*>::iterator ptr = bulletList.begin();
  while(ptr != bulletList.end()){
    (*ptr)->update(ticks);
    if( (*ptr)->goneTooFar()){
      freeList.push_front(*ptr);
      ptr = bulletList.erase(ptr);
    }else{
      ++ptr;
    }
  } 
}

Bullet* BulletPool::getBullet(){
  if(freeList.empty()){
    bulletList.push_front(new Bullet("bullet", Vector2f( 50, 20) + player->getPosition(), player->getVelocity()*2.5, bulletFrame ));
    return bulletList.front();
  }else{
    bulletList.push_front( freeList.front());
    bulletList.front()->reset(Vector2f( 50, 20) + player->getPosition() , player->getVelocity()*2.5);
    freeList.pop_front();
    return bulletList.front();
  }
}
