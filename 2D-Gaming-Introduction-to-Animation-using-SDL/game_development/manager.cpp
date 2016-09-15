#include <iostream>
#include <string>
#include <iomanip>
#include "sprite.h"
#include "gamedata.h"
#include "manager.h"

Manager::~Manager() { 
  // Manager made it, so Manager needs to delete it
  SDL_FreeSurface(orbSurface);
  SDL_FreeSurface(backSurface);
  SDL_FreeSurface(saucerSurface);
  delete orbFrame;
  delete saucerFrame;
}

Manager::Manager() :
  env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
  io( IOManager::getInstance() ),
  clock( Clock::getInstance() ),
  screen( io.getScreen() ),
  backSurface( io.loadAndSet(
    Gamedata::getInstance().getXmlStr("background/file"), 
    Gamedata::getInstance().getXmlBool("background/transparency")) ),
  saucerSurface( io.loadAndSet(
    Gamedata::getInstance().getXmlStr("saucer/file"), 
    Gamedata::getInstance().getXmlBool("saucer/transparency")) ),
  orbSurface( io.loadAndSet(
    Gamedata::getInstance().getXmlStr("flameorb/file"), 
    Gamedata::getInstance().getXmlBool("flameorb/transparency")) ),
  backFrame( "background", backSurface),
  saucerFrame( new Frame("saucer", saucerSurface) ),
  orbFrame( new Frame("flameorb", orbSurface) ),
  saucers(),
  orb("flameorb", orbFrame),
  orbs(),
  makeVideo( false ),
  frameCount( 0 ),
  username(  Gamedata::getInstance().getXmlStr("username") ),
  frameMax( Gamedata::getInstance().getXmlInt("frameMax") ),
  TITLE( Gamedata::getInstance().getXmlStr("screenTitle") )
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw string("Unable to initialize SDL: ");
  }
  int numberOfSaucers = Gamedata::getInstance().getXmlInt("saucer/numberOfSprites");
  saucers.reserve(numberOfSaucers);
  for(unsigned int i = 0; i < saucers.capacity(); ++i){
    saucers.push_back(Sprite("saucer", saucerFrame));
  }
  int numberOfSprites = Gamedata::getInstance().getXmlInt("flameorb/numberOfSprites");
  orbs.reserve(numberOfSprites);
  for(unsigned int i = 0; i < orbs.capacity(); ++i){
    orbs.push_back(orb);
  }
  atexit(SDL_Quit);
}

void Manager::draw() const {
  SDL_Rect dest = {0, 0, 0, 0};
  SDL_BlitSurface( backSurface, NULL, screen, &dest );
  backFrame.draw(0,0);
  for(unsigned int i = 0; i < saucers.size(); ++i){
    saucers[i].draw();;
  }
  orb.draw();
  for(unsigned int i = 0; i < orbs.size(); ++i){
    orbs[i].draw();
  }
  clock.draw();
  SDL_Flip(screen);
}

void Manager::update() {
  clock.update();
  Uint32 ticks = clock.getTicksSinceLastFrame();
  for(unsigned int i = 0; i < saucers.size(); ++i){
    saucers[i].update(ticks);;
  }
  orb.update(ticks);
  for(unsigned int i = 0; i < orbs.size(); ++i){
    orbs[i].update(ticks);
  }

  if ( makeVideo && frameCount < frameMax ) {
    std::stringstream strm;
    strm << "frames/" << username<< '.' 
         << std::setfill('0') << std::setw(4) 
         << frameCount++ << ".bmp";
    std::string filename( strm.str() );
    std::cout << "Making frame: " << filename << std::endl;
    SDL_SaveBMP(screen, filename.c_str());
  }
}

void Manager::play() {
  SDL_Event event;

  bool done = false;
  while ( not done ) {

    while ( SDL_PollEvent(&event) ) {
      Uint8 *keystate = SDL_GetKeyState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if (event.type == SDL_KEYDOWN) {
        if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
          done = true;
          break;
        }

        if (keystate[SDLK_F4] && !makeVideo) {
          std::cout << "Making video frames" << std::endl;
          makeVideo = true;
        }
        if (keystate[SDLK_p] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        if (keystate[SDLK_s] ) {
          clock.toggleSloMo();
        }
      }
    }

    draw();
    update();
  }
}
