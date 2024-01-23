#pragma once

#include <vector>

namespace Magnet {

class UI {
  std::vector<unsigned char> fontData;
 public:
  UI();
  ~UI();
  void newFrame();
  void draw();
};
}  // namespace Magnet