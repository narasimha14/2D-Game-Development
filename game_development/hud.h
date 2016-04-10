#include "clock.h"
#include "ioManager.h"

class Hud{
public:
  Hud(int width, int height);
  void displayHUD(SDL_Surface* screen, int x, int y ) const;
  void displayPool(SDL_Surface* screen, int x, int y, unsigned int bulletListSize, unsigned int freeListSize) const;
  void drawChatBox(SDL_Surface* screen, int x, int y, int radius, int orientation) const;
  void drawEndGame( SDL_Surface* screen, int x, int y) const;
  enum orientation { RIGHT, LEFT };
private:
  int mWidth;
  int mHeight;
  Clock& clock;
  const IOManager& io;
  Hud(const Hud&);
  Hud& operator=(const Hud&);
};
