#pragma once

#include <flecs.h>

#include <memory>

#include "magnetar/AssetManager.hpp"
#include "magnetar/WindowManager.hpp"

// forward declarations
class ShaderManager;
class Renderer;

class ApplicationContext {
  static std::unique_ptr<flecs::world>  ecs;
  static std::unique_ptr<AssetManager>  assetManager;
  static std::unique_ptr<ShaderManager> shaderManager;
  static std::unique_ptr<Renderer>      renderer;
  static std::unique_ptr<WindowManager> windowManager;

 public:
  ApplicationContext() = delete;

  static void initialize(const char *gameTitle);
                         
  static void run(void);

  static AssetManager* getAssetManager()   { return assetManager.get(); }
  static WindowManager* getWindowManager() { return windowManager.get(); }
  static ShaderManager* getShaderManager() { return shaderManager.get(); }
};