#pragma once

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <flecs.h>
#include <spdlog/spdlog.h>

#include <magnet/ArchiveManager.hpp>
#include <magnet/Time.hpp>
#include <memory>

namespace Magnet {

class Context {
  Time::TimeState timeState;
  flecs::world ecs;
  GLFWwindow* window = nullptr;

 public:
  virtual void init() = 0;
  virtual void update() = 0;

  void setWindow(GLFWwindow* window) { this->window = window; }

  Time::TimeState& getTimeState() { return timeState; }
  flecs::world& getECS() { return ecs; }
  GLFWwindow*& getWindow() { return window; }
};

}  // namespace Magnet

namespace Magnet::Application {

const char* const DEFAULT_LOG_FILENAME = "output.log";

/* This setup can only accomodate one context at once, but is designed in such a
 * way that it would be easy to support multiple contexts if needed... */

void registerContext(Context& applicationContext, const char* const title);
void initialize();

/* Getters */

ArchiveManager& getArchiveManager();

}  // namespace Magnet::Application