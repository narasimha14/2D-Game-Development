#include <SDL.h>
#include <string>
#include <deque>
#include "vector2f.h"
class Manager;

class Clock {
public:
  static Clock& getInstance();  // This class is a Singleton

  void toggleSloMo();
  bool isStarted() const { return started; }
  bool isPaused() const  { return paused;  }

  void start();
  void pause();
  void unpause();
  void draw() const;  // For debugging

private:
  unsigned int ticks;
  bool started;
  bool paused;
  bool sloMo;
  unsigned int sumOfTicks;
  unsigned int totalTicks;
  unsigned int timeElapsed;
  unsigned int frames;
  Vector2f pos;
  friend class Manager;
  friend class Hud;
  unsigned int getTicksSinceLastFrame() const;
  unsigned int getTicksSinceInit() const { return sumOfTicks; }
  int getFps() const;
  int getTimeElapsed() const;
  void update();  // Increments time/ticks
  Clock();
  Clock(const Clock&);
  Clock&operator=(const Clock&);
};
