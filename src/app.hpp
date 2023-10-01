#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "tweeny.h"

extern "C" {
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"

  #include "aes.h"
}

const int INIT_WIDTH = 800;
const int INIT_HEIGHT = 600;

//const char* AES_ASSETS_KEY = CMAKE_ASSETS_AES_KEY;