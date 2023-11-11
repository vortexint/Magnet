#pragma once

#include "scene_mgr.hpp"

#include "gfx/shader_mgr.hpp"

/* Game renderer */

class Renderer {
  AssetManager &assetMgr_;
  SceneManager &sceneMgr_;

  ShaderManager shaderMgr_;

public:
  Renderer(AssetManager &assetManager, SceneManager &sceneManager);

  void Clear() const;
  void Render() const;

  void SetActiveCamera(flecs::entity camera);
};