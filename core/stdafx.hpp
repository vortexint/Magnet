#pragma once

#include <cstddef>
#include <cstring>
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

extern "C" {
/* Single-header */
#include "qoi.h"

/* Libraries */
#include "cglm/cglm.h"

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#include "archive.h"
#include "archive_entry.h"

#include "aes.hpp"
}
