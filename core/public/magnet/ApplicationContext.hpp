#pragma once

#include <magnet/ArchiveManager.hpp>
#include <magnet/Time.hpp>

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
  ArchiveManager archiveManager;
  Time::TimeState timeState;

  ApplicationContext(const ApplicationContext&) = delete;
  void operator=(const ApplicationContext&) = delete;
 public:
  ApplicationContext();
  
  void registerInterface(ProjectInterface* interface);
  void initialize(const char* gameTitle);

  /* Getters */

  ArchiveManager& getArchiveManager();
  const Time::TimeState& getTimeState() const;
};

}  // namespace Magnet