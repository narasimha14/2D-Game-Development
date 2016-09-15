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

Manager::~Manager() { 
  std::list<Drawable*>::const_iterator ptr = sprites.begin();
  while ( ptr != sprites.end() ) {
    delete (*ptr);
    ++ptr;
  }
}

Manager::Manager() :
  env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
  io( IOManager::getInstance() ),
  clock( Clock::getInstance() ),
  screen( io.getScreen() ),
  world("forest", Gamedata::getInstance().getXmlInt("forest/factor") ),
  viewport( Viewport::getInstance() ),

  tigerFrames (FrameFactory::getInstance().getFrames("sher_khan")),
  pantherFrames (FrameFactory::getInstance().getFrames("bagheera")),
  mowgliFrames (FrameFactory::getInstance().getFrames("mowgli")),
  monkeyFrames (FrameFactory::getInstance().getFrames("monkey")),
  birdFrames (FrameFactory::getInstance().getFrames("bird")),
  leafFrame (FrameFactory::getInstance().getFrame("leaf")),
  sprites(),
  currentSprite(),

  makeVideo( false ),
  frameCount( 0 ),
  username(  Gamedata::getInstance().getXmlStr("username") ),
  title( Gamedata::getInstance().getXmlStr("screenTitle") ),
  frameMax( Gamedata::getInstance().getXmlInt("frameMax") )
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw string("Unable to initialize SDL: ");
  }
  SDL_WM_SetCaption(title.c_str(), NULL);
  atexit(SDL_Quit);
  
  sprites.push_back( new MultiSprite("sher_khan", tigerFrames) );
  sprites.push_back( new TwoWayMultiSprite("bagheera", pantherFrames) );
  sprites.push_back( new TwoWayMultiSprite("mowgli", mowgliFrames) );
  sprites.push_back( new MultiSprite("monkey", monkeyFrames) );
  sprites.push_back( new TwoWayMultiSprite("bird", birdFrames) );
  unsigned int numberOfSprites = Gamedata::getInstance().getXmlInt("numberOfLeafSprites");
  for(unsigned int i = 0; i < numberOfSprites; ++i)
  {  
    sprites.push_back( 
      new Sprite("leaf", Vector2f(Gamedata::getInstance().getXmlInt("leaf/startLoc/x"),Gamedata::getInstance().getXmlInt("leaf/startLoc/y")), 
                        Vector2f(Gamedata::getInstance().getXmlInt("leaf/speedX"),Gamedata::getInstance().getXmlInt("leaf/speedY")) , leafFrame) 
    );
  }
  currentSprite = sprites.begin();
  viewport.setObjectToTrack(*currentSprite);
}

void Manager::draw() const {
  world.draw();
  clock.draw();
  std::list<Drawable*>::const_iterator ptr = sprites.begin();
  while ( ptr != sprites.end() ) {
    (*ptr)->draw();
    ++ptr;
  }

  io.printMessageAt("Press T to switch sprites", 10, 70);
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

void Manager::switchSprite() {
  // Code changes made to jump over multiple instances of the same sprite. In my case, the leaf sprite
  string name = (*currentSprite)->getName();
  ++currentSprite;
  while(currentSprite != sprites.end() ){
    if( (*currentSprite)->getName() != name )
      break;
    ++currentSprite;
  } 
  if ( currentSprite == sprites.end() ) {
    currentSprite = sprites.begin();
  }
  viewport.setObjectToTrack(*currentSprite);
}

void Manager::update() {
  clock.update();
  Uint32 ticks = clock.getTicksSinceLastFrame();

  std::list<Drawable*>::const_iterator ptr = sprites.begin();
  while ( ptr != sprites.end() ) {
    (*ptr)->update(ticks);
    ++ptr;
  }
  if ( makeVideo && frameCount < frameMax ) {
    makeFrame();
  }
  world.update();
  viewport.update(); // always update viewport last
}

void Manager::play() {
  SDL_Event event;
  bool done = false;

  //Limiting the FPS
  int cappedFrameRate = Gamedata::getInstance().getXmlInt("frameCap");
  bool isCapped = Gamedata::getInstance().getXmlBool("framesAreCapped");

  clock.start();

  while ( not done ) {
    while ( SDL_PollEvent(&event) ) {
      Uint8 *keystate = SDL_GetKeyState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if(event.type == SDL_KEYDOWN) {
        if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
          done = true;
          break;
        }
        if ( keystate[SDLK_t] ) {
          switchSprite();
        }
        if ( keystate[SDLK_p] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        if (keystate[SDLK_s]) {
          clock.toggleSloMo();
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
    int deltaTime = updatedTime - currentTime;

    // Cap the frame rate using the SDL_Delay function
    if( isCapped && deltaTime < 1000/cappedFrameRate){
      SDL_Delay( (1000/cappedFrameRate) - deltaTime );
    }
  }
}
