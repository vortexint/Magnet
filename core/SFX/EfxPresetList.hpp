#pragma once

#include <unordered_map>
#include <string>

#include <cglm/cglm.h>

#include <magnet/AudioManager.hpp>
#include <SFX/EfxPresetList.hpp>


namespace Magnet {
  const std::unordered_map<std::string, EAXReverbDescription> &getEFXPresets();
} // namespace Magnet