#pragma once

#include <magnet/AssetManager.hpp>
#include <magnet/AudioManager.hpp>
#include <memory>

// forward declarations
struct GLFWwindow;
namespace Magnet {

class ProjectInterface {
 public:
  virtual void init() = 0;
  virtual void update() = 0;
};

class ApplicationContext {
  AssetManager assetManager;

  ApplicationContext();
  ApplicationContext(const ApplicationContext&) = delete;
  void operator=(const ApplicationContext&) = delete;

 public:
  static ApplicationContext& getInstance() {
    static ApplicationContext instance;
    return instance;
  }

  void registerInterface(ProjectInterface* interface);
  void initialize(const char* gameTitle);

  /* Getters */
  
  AssetManager& getAssetManager();
};

}  // namespace Magnet