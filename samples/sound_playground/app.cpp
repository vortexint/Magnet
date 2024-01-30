#include <magnet/Components.hpp>

#include "app.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


const char* CREATIVE_COMMONS_LICENSE = ""
"Cipher\", \"Latin Industries\", \"Sergio's Magic Dustbin\""
", \"Spellbound\", \"The Ice Giants\""
" Kevin MacLeod (incompetech.com)"
" Licensed under Creative Commons: By Attribution 3.0"
" http ://creativecommons.org/licenses/by/3.0/";
const char* TEST_AUDIO_FILES[] = {
  "tracks/Cipher2.ogg",
  "tracks/Latin Industries.ogg",
  "tracks/Sergio%27s Magic Dustbin.ogg",
  "tracks/Spellbound.ogg",
  "tracks/ding-idea-40142.ogg",
  "tracks/cipher-mono.ogg",
  "tracks/concretefootsteps-7093.ogg",
  "tracks/footsteps_grass_1-6810.ogg",
  "tracks/indoor-footsteps-6385.ogg",
  "tracks/rapid-wind-sound-effect-1-108398.ogg",
  "tracks/rapid-wind-sound-effect-2-108400.ogg",
  "tracks/running-in-grass-6237.ogg",
  "tracks/surprise-sound-effect-99300.ogg",
  "tracks/whoosh-cinematic-161021.ogg",
  "tracks/9mm-pistol-shot-6349.ogg",
};
Magnet::AudioTag TEST_AUDIO_FILE_TAGS[] = {
  AudioTag::MUSIC,
  AudioTag::MUSIC,
  AudioTag::MUSIC,
  AudioTag::MUSIC,
  AudioTag::SOUND_EFFECTS,
  AudioTag::MUSIC,
  AudioTag::SOUND_EFFECTS,
  AudioTag::SOUND_EFFECTS,
  AudioTag::SOUND_EFFECTS,
  AudioTag::SOUND_EFFECTS,
  AudioTag::SOUND_EFFECTS,
  AudioTag::SOUND_EFFECTS,
  AudioTag::SOUND_EFFECTS,
  AudioTag::SOUND_EFFECTS,
  AudioTag::SOUND_EFFECTS,
};

void Interface::init() {
  ArchiveManager archiveMgr(ARCH_data, ARCH_data_KEY);

  window = glfwGetCurrentContext();

  // Load icon
  {
    std::vector<unsigned char> imageData;
    archiveMgr.loadFile("icon.png", imageData);

    GLFWimage icons[1] = {};
    icons[0].pixels =
      stbi_load_from_memory(imageData.data(), imageData.size(), &icons[0].width,
                            &icons[0].height, nullptr, STBI_rgb_alpha);

    glfwSetWindowIcon(window, 1, icons);

    free(icons[0].pixels);
  }

  SceneManager& sceneManager = SceneManager::getInstance();
  auto& ecs = sceneManager.getECS();

  auto mainEntity = ecs.entity("MainEntity");
  mainEntity.set<Magnet::Components::AudioSource>({});
  mainEntity.set<Magnet::Components::Transform>({});
  this->mainEntityId = mainEntity.id();
}

void Interface::update() {

  ImGui::Begin("Sound Editor");

  auto& audioManager = AudioManager::getInstance();
  auto& ecs = SceneManager::getInstance().getECS();
  auto* mainAudioSource = ecs.entity(this->mainEntityId)
    .get_mut<Magnet::Components::AudioSource>();
  auto* mainTransform = ecs.entity(this->mainEntityId)
    .get_mut<Magnet::Components::Transform>();

  ImGui::DragFloat("volume", &mainAudioSource->volume, 0.01f, 0.f, 1.f);

  ImGui::DragFloat("pitch", &mainAudioSource->pitch, 0.01f, 0.5f, 2.f);


  if (ImGui::Button("Stop Request")) {
    mainAudioSource->stop();
  }

  if (ImGui::CollapsingHeader("Sounds")) {
    for (size_t i = 0; i < sizeof(TEST_AUDIO_FILES) / sizeof(TEST_AUDIO_FILES[0]); ++i) {
      auto testAudioFile = TEST_AUDIO_FILES[i];
      auto tag = TEST_AUDIO_FILE_TAGS[i];
      if (ImGui::Button(testAudioFile)) {
        mainAudioSource->play_sound(testAudioFile, tag);
      }
      ImGui::SameLine();
      auto tagStr = to_string(tag);
      ImGui::Text("%s", tagStr.c_str());
    }
  }

  ImGui::End();

  ImGui::Begin("Sound Playground Demo");
  
  ImGui::Separator();
  
  ImGui::Text("Emitter");
  ImGui::DragFloat3("position##Emitter_position", mainTransform->position);
  static float angleDegrees = 0.f;
  ImGui::Text("This rotates the entity around the listeners head the xz axis");
  ImGui::DragFloat("Entity Rotation", &angleDegrees, 1.f, 0.f, 360.f);

  mainTransform->position[2] = 2.f * cosf(angleDegrees * M_PI / 180);
  mainTransform->position[0] = 2.f * sinf(angleDegrees * M_PI / 180);

  ImGui::Separator();

  ImGui::Text("Volume Adjustor");
  ImGui::SliderFloat("Master Volume:", &audioManager.getMaster().volume, 0.f, 1.f);
  const char* AUDIO_TAG_STR[] = { "VOICE", "SOUND_EFFECTS", "MUSIC" };
  for (size_t i = 0; i < sizeof(AUDIO_TAG_STR) / sizeof(AUDIO_TAG_STR[0]); ++i) {
    auto *volume = &audioManager.getTagModifier(static_cast<Magnet::AudioTag>(i)).volume;
    ImGui::SliderFloat(AUDIO_TAG_STR[i], volume, 0.f, 1.f);;
  }

  ImGui::End();
}
