#pragma once

#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "tweeny.h"

#include "qoi.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#include "archive.h"
#include "archive_entry.h"

#include "aes.h"
}

const int INIT_WIDTH = 800;
const int INIT_HEIGHT = 600;

class AssetManager {
  struct archive *a;
  struct archive_entry *entry;
  int r;

public:
  AssetManager();
  ~AssetManager();

  std::unique_ptr<unsigned char[]> LoadAsset(const std::string &assetPath,
                                             size_t &dataSize);
};

class GameState {
  AssetManager assetManager_;
public:
  GameState();
};