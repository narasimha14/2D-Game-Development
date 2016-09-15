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
  std::vector<StaticSprite *>::const_iterator ptr2 = bananaSprites.begin();
  while ( ptr2 != bananaSprites.end() ) {
    delete (*ptr2);
    ++ptr2;
  }
  std::vector<MultiSprite *>::const_iterator ptr3 = monkeySprites.begin();
  while ( ptr3 != monkeySprites.end() ) {
    delete (*ptr3);
    ++ptr3;
  }
  SDL_FreeSurface(leafSurface);
  SDL_FreeSurface(bananaSurface);
}

Manager::Manager() :
  env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
  io( IOManager::getInstance() ),
  clock( Clock::getInstance() ),
  hud( 250, 220 ),
  healthBar(),
  screen( io.getScreen() ),
  mountain("mountain", Gamedata::getInstance().getXmlInt("mountain/factor") ),
  trees("trees", Gamedata::getInstance().getXmlInt("trees/factor") ),
  treesFront("treesFront", Gamedata::getInstance().getXmlInt("treesFront/factor") ),
  grassland("grassland", Gamedata::getInstance().getXmlInt("grassland/factor") ),
  viewport( Viewport::getInstance() ),
  leafSurface( io.loadAndSet(Gamedata::getInstance().getXmlStr("leaf/file"),
              Gamedata::getInstance().getXmlBool("leaf/transparency")) ),
  bananaSurface( io.loadAndSet(Gamedata::getInstance().getXmlStr("banana/file"),
              Gamedata::getInstance().getXmlBool("banana/transparency")) ),
  girlFrames (FrameFactory::getInstance().getFrames("girl")),
  mowgliFrames (FrameFactory::getInstance().getFrames("mowgli")),
  secondMowgliFrames (FrameFactory::getInstance().getFrames("mowgli_frame2")),
  monkeyFrames (FrameFactory::getInstance().getFrames("monkey")),
  tigerFrames (FrameFactory::getInstance().getFrames("sher_khan")),
  bananaFrame (FrameFactory::getInstance().getFrame("banana")),
  sprites(),
  currentSprite(),
  bananaSprites(),
  monkeySprites(),
  player("mowgli", mowgliFrames, "mowgli_frame2", secondMowgliFrames),
  girl("girl", girlFrames),
  tiger("sher_khan", tigerFrames),
  makeVideo( false ),
  displayHud(true),
  displayPool(true),
  displayChatForGirl(true),
  displayChatForMowgli(false),
  displaySecondChatForMowgli(false),
  displayChatForTiger(false),
  displayEnd(false),
  alreadyDisplayed(false),
  shoot(false),
  frameCount( 0 ),
  deltaTime(0),
  deltaPoolTime(0),
  deltaChatTime(0),
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
  unsigned int numberOfBananaSprites = Gamedata::getInstance().getXmlInt("numberOfBananaSprites");
  bananaSprites.reserve(numberOfBananaSprites);
  for(unsigned int i = 0; i < numberOfBananaSprites; ++i)
  {  
    bananaSprites.push_back( 
      new StaticSprite("banana", Vector2f(Gamedata::getInstance().getXmlInt("banana/startLoc/x"),Gamedata::getInstance().getXmlInt("banana/startLoc/y")),
                        Vector2f(Gamedata::getInstance().getXmlInt("banana/speedX"),Gamedata::getInstance().getXmlInt("banana/speedY")), bananaFrame) 
    );
  }
  unsigned int numberOfMonkeySprites = Gamedata::getInstance().getXmlInt("numberOfMonkeySprites");
  monkeySprites.reserve(numberOfMonkeySprites);
  for(unsigned int i = 0; i < numberOfMonkeySprites; ++i)
  {  
    monkeySprites.push_back( 
      new MultiSprite("monkey", monkeyFrames) 
    );
  }
  tiger.setVisibility(false);
  sort( sprites.begin(), sprites.end(), ScaledSpriteCompare() );
  currentSprite = sprites.begin();
}

void Manager::checkForCollisions() {
  std::vector<StaticSprite*>::const_iterator bananaSprite = bananaSprites.begin();
  while ( bananaSprite != bananaSprites.end() ) {
    if ( player.collidedWith(*bananaSprite) && ! (*bananaSprite)->alreadyCollided() ){
      (*bananaSprite)->setVisibility(false);
      (*bananaSprite)->setCollided(true);
      player.incrBananaCount();
      healthBar.incrHealth();
    }
    ++bananaSprite;
  }
  std::list<Bullet*> bulletList = player.getBulletList();
  std::list<Bullet*>::iterator bulletPtr = bulletList.begin(); 
  while( bulletPtr != bulletList.end() ){
    std::vector<MultiSprite*>::iterator monkeyPtr = monkeySprites.begin();
    while( monkeyPtr != monkeySprites.end() ){
      if( (*bulletPtr)->collidedWith( *monkeyPtr) && ! (*monkeyPtr)->alreadyCollided() ){
        (*monkeyPtr)->setVisibility(false);
        (*monkeyPtr)->setCollided(true);
        (*monkeyPtr)->explode();
        (*bulletPtr)->setTooFar(true);
        break;
      }
      ++monkeyPtr;
    }
    ++bulletPtr;
  }
  std::vector<MultiSprite*>::iterator monkeyPtr = monkeySprites.begin();
  while( monkeyPtr != monkeySprites.end() ){
    if( player.collidedWith( *monkeyPtr) && ! (*monkeyPtr)->alreadyCollided() ){
      (*monkeyPtr)->setVisibility(false);
      (*monkeyPtr)->setCollided(true);
      (*monkeyPtr)->explode();
      player.explode();
    }
    ++monkeyPtr;
  }
}

void Manager::reset() {
 std::vector<StaticSprite*>::const_iterator bananaSprite = bananaSprites.begin();
  while ( bananaSprite != bananaSprites.end() ) {
      (*bananaSprite)->X( (int)Gamedata::getInstance().getRandInRange( Gamedata::getInstance().getXmlInt("banana/startLoc/x"), 
        Gamedata::getInstance().getXmlInt("world/width") - Gamedata::getInstance().getXmlInt("banana/offsetX")) );
      (*bananaSprite)->setVisibility(true);
      (*bananaSprite)->setCollided(false);
      ++bananaSprite;
  }

  player.reset();
  healthBar.reset();
  alreadyDisplayed = false;
  tiger.setVisibility(false);
  displayEnd = false;
  displayChatForTiger = false;
  displaySecondChatForMowgli = false;
  std::list<Bullet*> bulletList = player.getBulletList();
  std::list<Bullet*>::iterator bulletPtr = bulletList.begin(); 
  while( bulletPtr != bulletList.end() ){
    (*bulletPtr)->setTooFar(true);
    ++bulletPtr;
  }

  std::vector<MultiSprite*>::iterator monkeyPtr = monkeySprites.begin();
  while( monkeyPtr != monkeySprites.end() ){
    (*monkeyPtr)->X( (int)Gamedata::getInstance().getRandInRange( Gamedata::getInstance().getXmlInt("monkey/startLoc/x"), 
      Gamedata::getInstance().getXmlInt("world/width") - Gamedata::getInstance().getXmlInt("monkey/offsetX")) );
    (*monkeyPtr)->setVisibility(true);
    (*monkeyPtr)->setCollided(false);
    ++monkeyPtr;
  }
  //No need to reset clock for this game
  //clock.reset();
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
  for(unsigned int i = 0; i < bananaSprites.size(); ++i){
    bananaSprites[i]->draw();
  }
  for(unsigned int i = 0; i < monkeySprites.size(); ++i){
    monkeySprites[i]->draw();
  }
  clock.draw();
  if(displayHud){
    hud.displayHUD(screen, 40, 40);
  }
  if(displayPool){
    hud.displayPool(screen, 500, 40, player.getBulletListSize(), player.getFreeListSize());
  }
  if(displayChatForGirl){
    hud.drawChatBox(screen, 600, 210, 100, Hud::LEFT);
    io.printMessageAt("Somebody save me", 480, 210);
    io.printMessageAt("from this monkey", 480, 250);
  }
  if(displayChatForMowgli){
    hud.drawChatBox(screen, 450, 210, 100, Hud::RIGHT);
    io.printMessageAt("Hold on babe !!!", 350, 210);
    io.printMessageAt(" I'm coming !!!", 350, 250);
  }

  if(displaySecondChatForMowgli && !displayEnd){
    hud.drawChatBox(screen, 400, 100, 150, Hud::LEFT);
    io.printMessageAt("  Yayy !!!", 300, 100);
    io.printMessageAt("I got her !!!", 300, 140);
    io.printMessageAt("I got her !!!", 300, 180);
  }
  if(displayChatForTiger && !displayEnd){
    hud.drawChatBox(screen, 450, 100, 100, Hud::RIGHT);
    io.printMessageAt("Not so soon", 350, 100);
    io.printMessageAt("  Kiddo !!!", 350, 140);
  }
  if(displayEnd){
    hud.drawEndGame(screen, 300, 100);
  }
  healthBar.draw();
  girl.draw();
  tiger.draw();
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
  int poolDisplayTime = Gamedata::getInstance().getXmlInt("poolDisplayTime");
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
  if(displayPool){
    deltaPoolTime += ticks;
    if(deltaPoolTime > poolDisplayTime){
      displayPool = false;
      deltaPoolTime = 0;
    }
  }else{
    deltaPoolTime = 0;
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
  player.update(ticks);
  if(displaySecondChatForMowgli && !displayEnd ){ 
    player.moveBackward();
    player.moveBackward();
    player.moveForward();
    player.setJump(true);
    deltaChatTime += ticks;
    if( deltaChatTime > Gamedata::getInstance().getXmlInt("mowgliSecondChatTime")){
      player.moveBackward();
      displaySecondChatForMowgli = false;
      deltaChatTime = 0;
      displayChatForTiger = true;
      tiger.setVisibility(true) ;
    }
  }

  healthBar.update(ticks);
  trees.update();
  tiger.update(ticks);
  if(displayChatForTiger && !displayEnd){
    tiger.setCurrentFrame(0);
    deltaChatTime += ticks;
    if(deltaChatTime > Gamedata::getInstance().getXmlInt("tigerChatTime")){
      displayChatForTiger = false;
      deltaChatTime = 0;
      displayEnd = true;
    }
  }

  std::vector<ScaledSprite*>::const_iterator ptr = sprites.begin();
  while ( ptr != sprites.end() ) {
    (*ptr)->update(ticks);
    ++ptr;
  }
  std::vector<StaticSprite*>::const_iterator ptr2 = bananaSprites.begin();
  while ( ptr2 != bananaSprites.end() ) {
    (*ptr2)->update(ticks);
    ++ptr2;
  }
  std::vector<MultiSprite*>::const_iterator ptr3 = monkeySprites.begin();
  while ( ptr3 != monkeySprites.end() ) {
    (*ptr3)->update(ticks);
    ++ptr3;
  }
  girl.update(ticks);
  if ( makeVideo && frameCount < frameMax ) {
    makeFrame();
  }
  mountain.update();
  treesFront.update();
  grassland.update();
  int worldWidth = Gamedata::getInstance().getXmlInt("world/width");
  if(player.X() > worldWidth - player.getFrameWidth() - player.getOffsetX() && !alreadyDisplayed){
      displaySecondChatForMowgli = true;
      alreadyDisplayed = true;
  }

  viewport.update(); // always update viewport last
}

void Manager::play() {
  SDL_Event event;
  bool done = false;
  SDLSound sound;
  bool tigerSound = false;
  bool endSound = false;
  //Limiting the FPS
  int cappedFrameRate = Gamedata::getInstance().getXmlInt("frameCap");
  bool isCapped = Gamedata::getInstance().getXmlBool("framesAreCapped");
  clock.start();

  viewport.setObjectToTrack(&girl);
  while ( not done ) {
    player.halt();
    Uint8 *keystate = SDL_GetKeyState(NULL);
    if( ! (keystate[SDLK_a] && keystate[SDLK_d]) ){
      if (keystate[SDLK_d] && !displayChatForGirl && !displayChatForMowgli && !displaySecondChatForMowgli && !displayChatForTiger && !displayEnd) {
        player.moveForward();
      }
      if (keystate[SDLK_a] && !displayChatForGirl && !displayChatForMowgli && !displaySecondChatForMowgli && !displayChatForTiger && !displayEnd) {
        player.moveBackward();
      }
    }
    if(displayChatForTiger){
      if(! tigerSound){
        tigerSound = true;
        sound[0];
      }
    }else{
      tigerSound = false;
    }
    if(displayEnd ){
      if(! endSound){
        endSound = true;
        sound[1];
      }
    }else{
      endSound = false;
    }

    if(!displayChatForMowgli && !displaySecondChatForMowgli && !displayChatForTiger && !displayEnd){
      player.setChangeFrame(true);
    }else{
      player.setChangeFrame(false);
    }
    while ( SDL_PollEvent(&event) ) {
      Uint8 *keystate = SDL_GetKeyState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if(event.type == SDL_KEYDOWN) {
        if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
          done = true;
          break;
        }
        if ( keystate[SDLK_SPACE] ) {
          if( !displayChatForGirl && !displayChatForMowgli && !displaySecondChatForMowgli && !displayChatForTiger && !displayEnd ){
            sound[2];
            shoot = true;
          }
        }
        if ( keystate[SDLK_p] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        if( keystate[SDLK_w] ) {
          if( !displayChatForGirl && !displayChatForMowgli && !displaySecondChatForMowgli && !displayChatForTiger && !displayEnd){
            if(!player.isJump()){
              sound[3];
              player.setJump(true);
            }
          }
        }
        if( keystate[SDLK_r] ) {
          sound.stopChannel(-1); 
          sound[3];
          reset();
        }
        if (keystate[SDLK_RCTRL]) {
          clock.toggleSloMo();
        }
        if (keystate[SDLK_F1]) {
          displayHud = !displayHud;
        }
        if (keystate[SDLK_F2]) {
          displayPool = !displayPool;
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
    if(shoot){
      player.shoot();
      shoot = false;
    }
    // Check if there are any collisions
    checkForCollisions(); 
    if(healthBar.isZero()){
      if(! displayEnd){
        player.explode();
        player.setVisibility(false);
        displayEnd = true;
      }
    }
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
