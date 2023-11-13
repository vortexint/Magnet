#pragma once

#include "magnetar/AssetManager.hpp"
#include "magnetar/Renderer.hpp"
#include "magnetar/WindowManager.hpp"

#include <flecs.h>

#include <memory>

class ApplicationContext {
  static std::unique_ptr<flecs::world> ecs;
  static std::unique_ptr<AssetManager> assetManager;
  static std::unique_ptr<WindowManager> windowManager;
  static std::unique_ptr<Renderer>      renderer;

 public:
  static void initialize(const char *secureArchivePath, const char *gameTitle,
                         uint32_t width, uint32_t height);
  static void run(void);

  static AssetManager &getAssetManager() { return *assetManager; }
};