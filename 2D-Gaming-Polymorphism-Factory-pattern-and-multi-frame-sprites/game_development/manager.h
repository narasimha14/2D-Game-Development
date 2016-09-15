#include <list>
#include <vector>
#include <SDL.h>
#include "ioManager.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"

class Manager {
public:
  Manager ();
  ~Manager ();
  void play();
  void switchSprite();

private:
  const bool env;
  const IOManager& io;
  Clock& clock;

  SDL_Surface * const screen;
  World world;
  Viewport& viewport;

//  SDL_Surface* orbSurface;
//  const Frame * const orbFrame;
//  SDL_Surface* starSurface;
//  std::vector<Frame*> starFrames;
  std::vector<Frame*> tigerFrames;
  std::vector<Frame*> pantherFrames;
  std::vector<Frame*> mowgliFrames;
  std::vector<Frame*> monkeyFrames;
  std::vector<Frame*> birdFrames;
  const Frame * const leafFrame;
//  std::vector<SDL_Surface*> surfaces;

  std::list<Drawable*> sprites;
  std::list<Drawable*>::iterator currentSprite;

  bool makeVideo;
  int frameCount;
  const std::string username;
  const std::string title;
  const int frameMax;

  void draw() const;
  void update();

  Manager(const Manager&);
  Manager& operator=(const Manager&);
  void makeFrame();
};
