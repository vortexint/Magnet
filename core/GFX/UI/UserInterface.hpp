#pragma once

#include <vector>

namespace Magnet {

class UI {
  UI(const UI&) = delete;
  UI& operator=(const UI&) = delete;
 public:
  UI();
  ~UI();
  void newFrame();
  void draw();
};
}  // namespace Magnet