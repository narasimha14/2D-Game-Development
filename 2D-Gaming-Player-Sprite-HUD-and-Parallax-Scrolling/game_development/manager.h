#include <list>
#include <vector>
#include <SDL.h>
#include "ioManager.h"
#include "hud.h"
#include "world.h"
#include "viewport.h"
#include "playersprite.h"
#include "multisprite.h"
#include "scaledSprite.h"
class Manager {
public:
  Manager ();
  ~Manager ();
  void play();

private:
  const bool env;
  const IOManager& io;
  Clock& clock;
  Hud hud;
  SDL_Surface * const screen;
  World mountain;
  World trees;
  World treesFront;
  World grassland;
  Viewport& viewport;

  SDL_Surface* leafSurface;
  std::vector<Frame*> girlFrames;
  std::vector<Frame*> mowgliFrames;
  std::vector<Frame*> monkeyFrames;

  std::vector<ScaledSprite*> sprites;
  std::vector<ScaledSprite*>::iterator currentSprite;
  PlayerSprite player;
  MultiSprite girl;
  MultiSprite monkey;
  bool makeVideo;
  bool displayHud;
  bool displayChatForGirl;
  bool displayChatForMowgli;
  int frameCount;
  int deltaTime;
  const std::string username;
  const std::string title;
  const int frameMax;
  void draw() const;
  void update();
  Manager(const Manager&);
  Manager& operator=(const Manager&);
  void makeFrame();
};
