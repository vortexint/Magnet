#pragma once

#include <spdlog/spdlog.h>

#include <magnet/ArchiveManager.hpp>
#include <magnet/Time.hpp>

#include <memory>

// forward declarations
struct GLFWwindow;
namespace Magnet {

class ProjectInterface;

class ApplicationContext {
  static ArchiveManager archiveManager;

  Time::TimeState timeState;
  spdlog::logger logger;

  ProjectInterface& registeredInterface;

  ApplicationContext(const ApplicationContext&) = delete;
  void operator=(const ApplicationContext&) = delete;
 public:
  ApplicationContext(ProjectInterface& interface);

  void registerInterface(std::unique_ptr<ProjectInterface> interface);
  void initialize(const char* gameTitle);

  /* Getters */

  ArchiveManager& getArchiveManager();
  const Time::TimeState& getTimeState() const;
};

class ProjectInterface {
 public:
  virtual void init(ApplicationContext& ctx) = 0;
  virtual void update(ApplicationContext& ctx) = 0;
};

}  // namespace Magnet