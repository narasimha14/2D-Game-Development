#include <iostream>
#include <string>
#include <iomanip>
#include "vector2f.h"
#include "multisprite.h"
#include "twowaymultisprite.h"
#include "sprite.h"
#include "gamedata.h"
#include "manager.h"
#include "frameFactory.h"
#include "aaline.h"
#include <algorithm>

class ScaledSpriteCompare {
public:
  bool operator()(const ScaledSprite* lhs, const ScaledSprite* rhs) {
    return lhs->getScale() < rhs->getScale();
  }
};

Manager::~Manager() { 
  std::vector<ScaledSprite *>::const_iterator ptr = sprites.begin();
  while ( ptr != sprites.end() ) {
    delete (*ptr);
    ++ptr;
  }
  SDL_FreeSurface(leafSurface);
}

Manager::Manager() :
  env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
  io( IOManager::getInstance() ),
  clock( Clock::getInstance() ),
  hud( 250, 150 ),
  screen( io.getScreen() ),
  mountain("mountain", Gamedata::getInstance().getXmlInt("mountain/factor") ),
  trees("trees", Gamedata::getInstance().getXmlInt("trees/factor") ),
  treesFront("treesFront", Gamedata::getInstance().getXmlInt("treesFront/factor") ),
  grassland("grassland", Gamedata::getInstance().getXmlInt("grassland/factor") ),
  viewport( Viewport::getInstance() ),
  leafSurface( io.loadAndSet(Gamedata::getInstance().getXmlStr("leaf/file"),
              Gamedata::getInstance().getXmlBool("leaf/transparency")) ),
  girlFrames (FrameFactory::getInstance().getFrames("girl")),
  mowgliFrames (FrameFactory::getInstance().getFrames("mowgli")),
  monkeyFrames (FrameFactory::getInstance().getFrames("monkey")),
  sprites(),
  currentSprite(),
  player("mowgli", mowgliFrames),
  girl("girl", girlFrames),
  monkey("monkey", monkeyFrames),
  makeVideo( false ),
  displayHud(true),
  displayChatForGirl(true),
  displayChatForMowgli(false),
  frameCount( 0 ),
  deltaTime(0),
  username(  Gamedata::getInstance().getXmlStr("username") ),
  title( Gamedata::getInstance().getXmlStr("screenTitle"," ") ),
  frameMax( Gamedata::getInstance().getXmlInt("frameMax") )
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw string("Unable to initialize SDL: ");
  }
  SDL_WM_SetCaption(title.c_str(), NULL);
  atexit(SDL_Quit);
  
  unsigned int numberOfSprites = Gamedata::getInstance().getXmlInt("numberOfLeafSprites");
  sprites.reserve(numberOfSprites);
  for(unsigned int i = 0; i < numberOfSprites; ++i)
  {  
    sprites.push_back( 
      new ScaledSprite("leaf", leafSurface) 
    );
  }
  sort( sprites.begin(), sprites.end(), ScaledSpriteCompare() );
  currentSprite = sprites.begin();
}

void Manager::draw() const {
  mountain.draw();
  for(unsigned int i = 0; i < sprites.size()/3; ++i){
    sprites[i]->draw();
  }
  trees.draw();
  for(unsigned int i = sprites.size()/3; i < 2*sprites.size()/3; ++i){
    sprites[i]->draw();
  }
  treesFront.draw();
  for(unsigned int i = 2*sprites.size()/3 ; i < sprites.size(); ++i){
    sprites[i]->draw();
  }
  grassland.draw();
  clock.draw();
  if(displayHud){
    hud.displayHUD(screen, 40, 40);
  }
  if(displayChatForGirl){
    hud.drawChatBox(screen, 600, 210, 100, 1);
    io.printMessageAt("Somebody save me", 480, 210);
    io.printMessageAt("from this monkey", 480, 250);
  }
  if(displayChatForMowgli){
    hud.drawChatBox(screen, 450, 210, 100, 0);
    io.printMessageAt("Hold on babe !!!", 350, 210);
    io.printMessageAt(" I'm coming !!!", 350, 250);
  }

  int worldWidth = Gamedata::getInstance().getXmlInt("world/width");
  if(player.X() > worldWidth - player.getFrameWidth() - player.getOffsetX() ){
      hud.drawEndGame(screen, 300, 100);
  }
  girl.draw();
  monkey.draw();
  player.draw();
  
  io.printMessageAt(title, 10, 450);
  viewport.draw();

  SDL_Flip(screen);
}

// Move this to IOManager
void Manager::makeFrame() {
  std::stringstream strm;
  strm << "frames/" << username<< '.' 
       << std::setfill('0') << std::setw(4) 
       << frameCount++ << ".bmp";
  std::string filename( strm.str() );
  std::cout << "Making frame: " << filename << std::endl;
  SDL_SaveBMP(screen, filename.c_str());
}

void Manager::update() {
  clock.update();
  int girlChatTime = Gamedata::getInstance().getXmlInt("girlChatTime");
  int mowgliChatTime = Gamedata::getInstance().getXmlInt("mowgliChatTime");
  int hudTime = Gamedata::getInstance().getXmlInt("hudTime");
  Uint32 ticks = clock.getTicksSinceLastFrame();
  if(displayHud){
    deltaTime += ticks;
    if(deltaTime > hudTime){
      displayHud = false;
      deltaTime = 0;
    }
  }else{
    deltaTime = 0;
  }
  if(displayChatForGirl){
    if(clock.getTimeElapsed() >= girlChatTime){
      displayChatForGirl = false;
      displayChatForMowgli = true;
      viewport.setObjectToTrack(&player);
    }
  }
  if(displayChatForMowgli){
    if(clock.getTimeElapsed() >= girlChatTime + mowgliChatTime){
      displayChatForMowgli = false;
    }
  }
  std::vector<ScaledSprite*>::const_iterator ptr = sprites.begin();
  while ( ptr != sprites.end() ) {
    (*ptr)->update(ticks);
    ++ptr;
  }
  player.update(ticks);
  girl.update(ticks);
  monkey.update(ticks);
  if ( makeVideo && frameCount < frameMax ) {
    makeFrame();
  }
  mountain.update();
  trees.update();
  treesFront.update();
  grassland.update();
  viewport.update(); // always update viewport last
}

void Manager::play() {
  SDL_Event event;
  bool done = false;

  //Limiting the FPS
  int cappedFrameRate = Gamedata::getInstance().getXmlInt("frameCap");
  bool isCapped = Gamedata::getInstance().getXmlBool("framesAreCapped");
  clock.start();

  viewport.setObjectToTrack(&girl);
  while ( not done ) {
    player.halt();
    Uint8 *keystate = SDL_GetKeyState(NULL);
    if( ! (keystate[SDLK_a] && keystate[SDLK_d]) ){
      if (keystate[SDLK_d] && !displayChatForGirl && !displayChatForMowgli) {
        player.moveForward();
      }
      if (keystate[SDLK_a] && !displayChatForGirl && !displayChatForMowgli) {
        player.moveBackward();
      }
    }
   
    while ( SDL_PollEvent(&event) ) {
      Uint8 *keystate = SDL_GetKeyState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if(event.type == SDL_KEYDOWN) {
        if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
          done = true;
          break;
        }
        /*if ( keystate[SDLK_t] ) {
          switchSprite();
        }*/
        if ( keystate[SDLK_p] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        if (keystate[SDLK_s]) {
          clock.toggleSloMo();
        }
        if (keystate[SDLK_F1]) {
          displayHud = !displayHud;
        }
        if (keystate[SDLK_F4] && !makeVideo) {
          std::cout << "Making video frames" << std::endl;
          makeVideo = true;
        }
      }
    }
    
    // Time recorded to find the delta time required to cap the frame rate
    int currentTime = SDL_GetTicks();
    draw();
    update();
    // Time updated to find the delta time required to cap the frame rate
    int updatedTime = SDL_GetTicks();
    int delTime = updatedTime - currentTime;

    // Cap the frame rate using the SDL_Delay function
    if( isCapped && delTime < 1000/cappedFrameRate){
      SDL_Delay( (1000/cappedFrameRate) - delTime );
    }
  }
}
