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
#include "flecs.h"

#include "qoi.h"

#include "aes.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#include "archive.h"
#include "archive_entry.h"
}

const int INIT_WIDTH = 800;
const int INIT_HEIGHT = 600;

const char* const GAME_TITLE = "Uniforge";