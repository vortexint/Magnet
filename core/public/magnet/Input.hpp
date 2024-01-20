#pragma once
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

namespace Magnet::Input {

class Observer {
 public:    
  Observer();
  virtual void onKeyEvent(int key, int action, int mods);
  virtual void onMouseButtonEvent(int button, int action, int mods);
  virtual void onMouseMoveEvent(double xpos, double ypos);
  virtual void onMouseScrollEvent(double xoffset, double yoffset);
};

void init();
void addObserver(Observer* observer);

}  // namespace Magnet::Input