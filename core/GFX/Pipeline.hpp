#pragma once

// Pipeline.hpp
// Manages the rendering pipeline

#include <cstdint>
#include <queue>

#include <magnet/ArchiveManager.hpp>

namespace Magnet::Rendering {

void setupPipeline(ArchiveManager& archiveMgr);
void drawFrame();

}  // namespace Magnet::Rendering