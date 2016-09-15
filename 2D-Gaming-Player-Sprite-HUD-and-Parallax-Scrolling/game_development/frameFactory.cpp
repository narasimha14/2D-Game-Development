#include "frameFactory.h"
#include "extractSurface.h"
#include "ioManager.h"
#include "vector2f.h"
#include <cmath>
#include "SDL/SDL_rotozoom.h"

FrameFactory::~FrameFactory() {
  std::map<std::string, SDL_Surface*>::const_iterator ptr = surfaces.begin();
  while ( ptr != surfaces.end() ) {
    SDL_FreeSurface(ptr->second);
    ++ptr;
  }
  std::map<std::string, std::vector<SDL_Surface*> >::const_iterator ptr2 = multiSurfaces.begin();
  while ( ptr2 != multiSurfaces.end() ) {
    std::vector<SDL_Surface*>::const_iterator itr = (ptr2->second).begin();
    while(itr != (ptr2->second).end()){
      SDL_FreeSurface(*itr);
      ++itr;
    }
    ++ptr2;
  }
  
  std::map<std::string, Frame*>::const_iterator ptr3 = frames.begin();
  while(ptr3 != frames.end()){
    delete ptr3->second;
    ++ptr3;
  }
  std::map<std::string, std::vector<Frame*> >::const_iterator ptr4 = multiFrames.begin();
  while(ptr4 != multiFrames.end()){
    std::vector<Frame*>::const_iterator itr2 = (ptr4->second).begin();
    while( itr2 != (ptr4->second).end()){
      delete *itr2;
      ++itr2;
    }
    ++ptr4;
  }

}

FrameFactory& FrameFactory::getInstance() {
  static FrameFactory factory;
  return factory;
}

Frame* FrameFactory::getFrame(const std::string& name) {
  std::map<std::string, Frame*>::const_iterator pos = frames.find(name); 
  if ( pos == frames.end() ) {
    SDL_Surface * const surface =
      IOManager::getInstance().loadAndSet(
          gdata.getXmlStr(name+"/file"),
          gdata.getXmlBool(name+"/transparency"));
    surfaces[name] = surface;
    Frame * const frame =new Frame(name, surface);
    frames[name] = frame;
    return frame;
  }
  else {
    return pos->second;
  }
}

std::vector<Frame*> FrameFactory::getFrames(const std::string& name) {
  // First search map to see if we've already made it:
  std::map<std::string, std::vector<Frame*> >::const_iterator 
    pos = multiFrames.find(name); 
  if ( pos != multiFrames.end() ) {
    return pos->second;
  }

  // It wasn't in the map, so we have to make the vector of Frames:
  SDL_Surface* surface = IOManager::
     getInstance().loadAndSet(gdata.getXmlStr(name+"/file"), true);
  unsigned numberOfFrames = gdata.getXmlInt(name+"/frames");
  std::vector<Frame*> frames;
  std::vector<SDL_Surface*> surfaces;
  frames.reserve(numberOfFrames);
  Uint16 srcX = gdata.getXmlInt(name+"/srcX");
  Uint16 srcY = gdata.getXmlInt(name+"/srcY");
  Uint16 width = gdata.getXmlInt(name+"/width");
  Uint16 height = gdata.getXmlInt(name+"/height");

  SDL_Surface* surf;
  for (unsigned i = 0; i < numberOfFrames; ++i) {
    unsigned frameX = i * width + srcX;
   surf = ExtractSurface::getInstance().
               get(surface, width, height, frameX, srcY); 
    surfaces.push_back( surf );
    frames.push_back( new Frame(name, surf) );
  }
  SDL_FreeSurface(surface);
  multiSurfaces[name] = surfaces;
  multiFrames[name] = frames;
  return frames;
}

