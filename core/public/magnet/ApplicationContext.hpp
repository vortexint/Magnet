#pragma once

#include <flecs.h>

#include <memory>

#include <magnet/AssetManager.hpp>
#include <magnet/WindowManager.hpp>

namespace Magnet {
// forward declarations
class InputManager;
class Renderer;
class ShaderManager;

class ProjectInterface {
 public:
  virtual void init() = 0;
  virtual void update() = 0;
};

class ApplicationContext {
  static std::unique_ptr<flecs::world>  ecs;
  static std::unique_ptr<AssetManager>  assetManager;
  static std::unique_ptr<ShaderManager> shaderManager;
  static std::unique_ptr<Renderer>      renderer;
  static std::unique_ptr<WindowManager> windowManager;

 public:
  ApplicationContext() = delete;
  ~ApplicationContext();

  static void registerInterface(ProjectInterface* interface);
  static void initialize(const char *gameTitle);

  static AssetManager* getAssetManager()   { return assetManager.get(); }
  static WindowManager* getWindowManager() { return windowManager.get(); }
  static ShaderManager* getShaderManager() { return shaderManager.get(); }
  static Renderer* getRenderer()           { return renderer.get(); }
};

}  // namespace Magnet