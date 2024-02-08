#pragma once

// Renderer.hpp
// Manages the rendering pipeline

#include <cstdint>
#include <magnet/ArchiveManager.hpp>

namespace Magnet::Renderer {

void setupPipeline(ArchiveManager& archiveMgr);
void drawFrame();

}  // namespace Magnet::Renderer