#pragma once

#include <memory>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> // Useful to include for key codes

namespace Magnet::Input {

class Observer {
 public:
  Observer();
  virtual void onKeyEvent(int key, int scancode, int action, int mods);
  virtual void onMouseButtonEvent(int button, int action, int mods);
  virtual void onMouseMoveEvent(double xpos, double ypos);
  virtual void onMouseScrollEvent(double xoffset, double yoffset);
};

std::vector<Observer*>& getObservers();

void initialize(GLFWwindow* window);
void addObserver(Observer* observer);
void removeObserver(Observer* observer);

}  // namespace Magnet::Input