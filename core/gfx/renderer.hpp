#pragma once

#include "gfx/shader_mgr.hpp"

/* Game renderer */

class Renderer {

  AssetManager &asset_manager_;
  ShaderManager shader_manager_;

public:
  Renderer(AssetManager &assetManager);

  void Clear() const;

  void Render() const;
};