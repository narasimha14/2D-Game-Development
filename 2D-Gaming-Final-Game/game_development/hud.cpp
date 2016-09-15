#include "aaline.h"
#include "hud.h"
#include <cmath>

#ifndef PI
#define PI 3.1415926535897
#endif

#define RAD2DEG (180/PI)
#define DEG2RAD (PI/180)

#define DegToRad(x) ((x)*DEG2RAD)
#define RadToDeg(x) ((x)*RAD2DEG)

Hud::Hud(int width, int height) : 
mWidth(width),
mHeight(height),
clock( Clock::getInstance() ),
io(IOManager::getInstance() )
{

}

void Hud::drawEndGame( SDL_Surface* screen, int x, int y) const {
  const Uint32 RED = SDL_MapRGB(screen->format, 0xff, 0, 0);
  Draw_AALine(screen, x, y+75,
                      x+mWidth + 50,y+75,
                      150, 0 , 0, 0, 3*0xff/4);
  Draw_AALine(screen, x,y, x+mWidth+50,y, 5, RED);
  Draw_AALine(screen, x,y + 75, x+5,y + 75, 150, RED);
  Draw_AALine(screen, x+mWidth + 50 - 5 ,y + 75, x+mWidth + 50,y + 75, 150, RED);
  Draw_AALine(screen, x, y+150, x + mWidth + 50,y + 150, 5, RED);
  io.printMessageAt(" Ugh! So close!!!", x + 70, y+70);
}

void Hud::drawWin( SDL_Surface* screen, int x, int y) const {
  const Uint32 RED = SDL_MapRGB(screen->format, 0xff, 0, 0);
  Draw_AALine(screen, x, y+75,
                      x+mWidth + 50,y+75,
                      150, 0 , 0, 0, 3*0xff/4);
  Draw_AALine(screen, x,y, x+mWidth+50,y, 5, RED);
  Draw_AALine(screen, x,y + 75, x+5,y + 75, 150, RED);
  Draw_AALine(screen, x+mWidth + 50 - 5 ,y + 75, x+mWidth + 50,y + 75, 150, RED);
  Draw_AALine(screen, x, y+150, x + mWidth + 50,y + 150, 5, RED);
  io.printMessageAt(" And they lived happily", x + 35, y+60);
  io.printMessageAt("     ever after !!!", x + 50, y+90);
}

void Hud::displayHUD(SDL_Surface* screen, int x, int y ) const {
  const Uint32 RED = SDL_MapRGB(screen->format, 0xff, 0, 0);
  Draw_AALine(screen, x, y+mHeight/2,
                      x+mWidth + 50,y+mHeight/2,
                      mHeight, 0, 0, 0, 0xff/2);
  Draw_AALine(screen, x,y, x+mWidth+50,y, 5, RED);
  Draw_AALine(screen, x,y + mHeight/2, x+5,y + mHeight/2, mHeight, RED);
  Draw_AALine(screen, x+mWidth + 50 - 5 ,y + mHeight/2, x+mWidth + 50,y + mHeight/2, mHeight, RED);
  Draw_AALine(screen, x, y+mHeight, x + mWidth + 50,y + mHeight, 5, RED);
  io.printMessageValueAt("Avg Fps: ", clock.getFps(), x + 10, y+30);
  io.printMessageValueAt("Seconds: ", clock.getTimeElapsed(), x+10, y+50);
  io.printMessageAt("a -> backwards", x + 10, y+70);
  io.printMessageAt("d -> forwards", x + 10, y+95);
  io.printMessageAt("w -> jump", x + 10, y+120);
  io.printMessageAt("SPACE -> shoot", x + 10, y+145);
  io.printMessageAt("g -> God Mode", x + 10, y+170);
  io.printMessageAt("r -> reset the game", x + 10, y+195);
  io.printMessageAt("F1 -> toggle this HUD", x + 10, y+220);
}

void Hud::displayPool(SDL_Surface* screen, int x, int y, unsigned int bulletListSize, unsigned int freeListSize) const {
  const Uint32 RED = SDL_MapRGB(screen->format, 0xff, 0, 0);
  int yincr = 90;
  Draw_AALine(screen, x, y + yincr,
                      x + mWidth + 50,y + yincr,
                      yincr + 30, 0, 0, 0, 0xff/2);
  Draw_AALine(screen, x, y + 27, x+mWidth+50, y + 27, 5, RED);
  Draw_AALine(screen, x, y + yincr, x + 5 , y + yincr, yincr + 30, RED);
  Draw_AALine(screen, x + mWidth + 50 - 5 ,y + yincr, x+mWidth + 50, y + yincr, yincr + 30, RED);
  Draw_AALine(screen, x, y + yincr + 30 + 27, x + mWidth + 50,y + yincr + 30 + 27, 5, RED);
  io.printMessageValueAt("Bullet List: ", bulletListSize, x+10, y + 35);
  io.printMessageValueAt("Free List: ", freeListSize, x+10, y+65);
  io.printMessageAt("F2 toggles this HUD", x+10, y+95);
}

//Orientation 0 -> right, 1 -> left
void Hud::drawChatBox(SDL_Surface* screen, int xpos, int ypos, int mRadius, int orientation) const {
  Sint16 radius = mRadius + 1;
  Sint16 x = xpos;
  Sint16 y = ypos;
  const int NO_OF_POINTS = 360;
  int step = 360/NO_OF_POINTS;
  Draw_AALine(screen, x - mRadius - 50, y + mRadius/2 - 20,
                      x + mRadius + 10,y + mRadius/2 - 20,
                      mRadius, 0xff, 0, 0, 0xff);
  radius = mRadius/5;
  if(orientation == 0){
    x -= mRadius;
    y += mRadius;
  }else{
    x += mRadius;
    y += mRadius;
  }

  while(--radius > 0){
    for (int theta = 0; theta < 360; theta += step) {
      Draw_Pixel(screen, x + radius * cos(DegToRad(theta)),
                 y + radius * sin(DegToRad(theta)), 0xff, 0, 0,0xff);
    }
  }
  radius = mRadius/10;
  if(orientation == 0){
    x -= mRadius/5;
    y += mRadius/5;
  }else{
    x += mRadius/5;
    y += mRadius/5;
  }
  while(--radius > 0){
    for (int theta = 0; theta < 360; theta += step) {
      Draw_Pixel(screen, x + radius * cos(DegToRad(theta)),
                 y + radius * sin(DegToRad(theta)), 0xff, 0, 0,0xff);
    }
  }
}
