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
#include "staticsprite.h"
#include "sound.h"
#include "health.h"

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
  Health healthBar;
  SDL_Surface * const screen;
  World mountain;
  World trees;
  World treesFront;
  World grassland;
  Viewport& viewport;

  SDL_Surface* leafSurface;
  SDL_Surface* bananaSurface;
  std::vector<Frame*> girlFrames;
  std::vector<Frame*> mowgliFrames;
  std::vector<Frame*> secondMowgliFrames;
  std::vector<Frame*> monkeyFrames;
  std::vector<Frame*> tigerFrames;
  const Frame* const bananaFrame;

  std::vector<ScaledSprite*> sprites;
  std::vector<ScaledSprite*>::iterator currentSprite;
  std::vector<StaticSprite*> bananaSprites;
  std::vector<MultiSprite*> monkeySprites;
  PlayerSprite player;
  MultiSprite girl;
  MultiSprite tiger;
  bool makeVideo;
  bool displayHud;
  bool displayPool;
  bool displayChatForGirl;
  bool displayChatForMowgli;
  bool displaySecondChatForMowgli;
  bool displayChatForTiger;
  bool displayEnd;
  bool alreadyDisplayed;
  bool shoot;
  int frameCount;
  int deltaTime;
  int deltaPoolTime;
  int deltaChatTime;
  const std::string username;
  const std::string title;
  const int frameMax;
  void reset();
  void draw() const;
  void update();
  void checkForCollisions() ;
  Manager(const Manager&);
  Manager& operator=(const Manager&);
  void makeFrame();
};
