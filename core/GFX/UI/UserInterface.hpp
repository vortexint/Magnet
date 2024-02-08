#pragma once

namespace Magnet {
// Forward declaration
class ArchiveManager;
}

namespace Magnet::UI {

void setup(ArchiveManager& archiveMgr);
void shutdown();

void newFrame();
void draw();

}  // namespace Magnet::UI