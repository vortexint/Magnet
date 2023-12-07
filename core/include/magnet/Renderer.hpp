#pragma once
/**
 * @file Renderer.hpp
 * @brief GL rendering component
 */

#include <cstdint>
#include <flecs.h>
#include <glad/glad.h>

// Acquired by Window Manager
const uint32_t INIT_WIDTH = 1280;
const uint32_t INIT_HEIGHT = 720;

// forward declarations
class ShaderManager;
class Renderer {
  flecs::world* ecs;
  flecs::entity activeCamera;

  ShaderManager* shaderManager;

  int width = INIT_WIDTH;
  int height = INIT_HEIGHT;

 public:
  Renderer(flecs::world* ecs, ShaderManager* shaderManager);

  void initialize();

  void render() const;

  void setActiveCamera(flecs::entity cameraEntity);

  void resize(int width, int height);
};