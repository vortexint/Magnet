#pragma once

#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "mustache/ecs/ecs.hpp"

#include "qoi.h"

#include "aes.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#include "archive.h"
#include "archive_entry.h"
}
