#pragma once

// Pipeline.hpp (private)
// Manages the rendering pipeline

#include <cstdint>
#include <queue>

#include <magnet/ArchiveManager.hpp>

struct GLFWwindow;
namespace Magnet::Rendering {

struct PipelineSettings;

void setupPipeline(const PipelineSettings& settings);
void setupPipeline();

void updatePipeline(GLFWwindow* window);

}  // namespace Magnet::Rendering