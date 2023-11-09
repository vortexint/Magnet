#pragma once

#include "gfx/shader_mgr.hpp"

/* Game renderer */

class Renderer {

  AssetManager &assetMgr_;
  ShaderManager shaderMgr_;

public:
  Renderer(AssetManager &assetManager);

  void Clear() const;

  void Render() const;
};