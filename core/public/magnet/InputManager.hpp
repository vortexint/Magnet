#pragma once
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

namespace Magnet {

class Observer {
 public:    
  Observer();
  virtual void onKeyEvent(int key, int action, int mods) = 0;
  virtual void onMouseButtonEvent(int button, int action, int mods) = 0;
  virtual void onMouseMoveEvent(double xpos, double ypos) = 0;
  virtual void onMouseScrollEvent(double xoffset, double yoffset) = 0;
};

class InputManager {
  std::vector<Observer*> observers;

 public:
  InputManager();

  void addObserver(Observer* observer);

  void notifyKeyEvent(int key, int action, int mods);
  void notifyMouseButtonEvent(int button, int action, int mods);
  void notifyMouseMoveEvent(double xpos, double ypos);
  void notifyMouseScrollEvent(double xoffset, double yoffset);
};

}  // namespace Magnet