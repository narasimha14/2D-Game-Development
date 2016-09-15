#include <SDL.h>
#include "ioManager.h"
#include "clock.h"
#include "sprite.h"
#include <vector>

class Manager {
public:
  Manager ();
  ~Manager ();
  void play();

private:
  const bool env;
  const IOManager& io;
  Clock& clock;

  SDL_Surface * const screen;

  SDL_Surface* backSurface;
  SDL_Surface* saucerSurface;
  SDL_Surface* orbSurface;
  Frame backFrame;
  const Frame * const saucerFrame;
  const Frame * const orbFrame;
  std::vector<Sprite> saucers;
  Sprite orb;

  std::vector<Sprite> orbs;
  bool makeVideo;
  int frameCount;
  std::string username;
  int frameMax;
  const std::string TITLE;

  void draw() const;
  void update();

  Manager(const Manager&);
  Manager& operator=(const Manager&);
  void drawBackground() const;
};
