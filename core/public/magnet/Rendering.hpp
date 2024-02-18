#pragma once

#include <glad/glad.h>

namespace Magnet::Rendering {

enum AntiAliasing {
  NONE, FXAA, MSAA
};

enum TextureFiltering { NEAREST,  LINEAR, ANISOTROPIC };

struct PipelineConfig {
  AntiAliasing antiAliasing = AntiAliasing::NONE;
  TextureFiltering textureFiltering = TextureFiltering::LINEAR;
  bool VSync = false;
};

void setupPipeline(const PipelineConfig& config);

}  // namespace Magnet::Rendering