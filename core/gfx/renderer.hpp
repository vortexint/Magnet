#pragma once

#include "gfx/shader_mgr.hpp"
#include "components.hpp"

/* Game renderer */

class Renderer {

  AssetManager &assetMgr_;
  ShaderManager shaderMgr_;

public:
  Renderer(AssetManager &assetManager);

  void Clear() const;

  void Render() const;

  void SetCamera(const magnetar::Camera &camera);
};