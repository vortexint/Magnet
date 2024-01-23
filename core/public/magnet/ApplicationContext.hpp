#pragma once

#include <memory>

#include <magnet/WindowManager.hpp>
#include <magnet/AssetManager.hpp>

namespace Magnet {

class ProjectInterface {
 public:
  virtual void init() = 0;
  virtual void update() = 0;
};

class ApplicationContext {
  static std::unique_ptr<AssetManager>  assetManager;
  static std::unique_ptr<WindowManager> windowManager;

 public:
  ApplicationContext() = delete;
  ~ApplicationContext();

  static void registerInterface(ProjectInterface* interface);
  static void initialize(const char *gameTitle);

  static AssetManager* getAssetManager()   { return assetManager.get(); }
  static WindowManager* getWindowManager() { return windowManager.get(); }
};

}  // namespace Magnet