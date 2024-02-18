#include <magnet/Components.hpp>
#include <magnet/Scene.hpp>

#include <magnet/AudioManager.hpp>

#include <../SFX/EfxPresetList.hpp>

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

void App::init() {
  ArchiveManager archiveMgr(ARCH_data, ARCH_data_KEY);
  this->archiveMgr = new ArchiveManager("data.magnet", nullptr);

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
  

  auto& audioManager = AudioManager::getInstance();
  std::vector<uint8_t> fileBuffer;
  RawAudioData rawAudio;
  const size_t NUM_TEST_AUDIO_FILES = sizeof(TEST_AUDIO_FILES) / sizeof(TEST_AUDIO_FILES[0]);
  for (size_t i = 0; i < NUM_TEST_AUDIO_FILES; ++i) {
    fileBuffer.clear();
    rawAudio.clear();

    this->archiveMgr->loadFile(TEST_AUDIO_FILES[i], fileBuffer);
    std::span<const uint8_t> fileSpan(fileBuffer.data(), fileBuffer.size());
    if (AudioManager::LoadAudio(fileSpan, rawAudio)) {
      std::span<const uint8_t> rawAudioData(rawAudio.data.data(),
                                            rawAudio.data.size());

      audioManager.registerAudio(TEST_AUDIO_FILES[i], rawAudioData,
                                 rawAudio.format, rawAudio.sampleRate);
      trackNames.push_back(TEST_AUDIO_FILES[i]);
    }
  }

  audioManager.registerAllEfxPresets();

  auto& ecs = this->getECS();

  if (!AUDIO_DEBUG_ENABLED) {
    auto mainEntity = ecs.entity("MainEntity")
                        .add<Magnet::Components::AudioSource>()
                        .add<Magnet::Components::Transform>();
    sourceIds.push_back(mainEntity.id());

    auto entity = ecs.entity("Environment");
    entity.set<Magnet::Components::Transform>({});
    entity.set<Magnet::Components::Environment>({"EFX_REVERB_PRESET_GENERIC"});
    environmentIds.push_back(entity.id());
  }

  if (AUDIO_DEBUG_ENABLED) {
    audioDebugInit();
  }
    
}

void App::update() {
  
  auto& ecs = this->getECS();
  auto& audioManager = AudioManager::getInstance();

  
  if (AUDIO_DEBUG_ENABLED) {
    audioDebugInit(ecs);
  }

  ImGui::Begin("Environments Editor");
  ImGui::Separator();
  ImGui::Text("Environments");


  if (ImGui::Button("New Environment")) {
    newEnvironment(ecs, "EFX_REVERB_PRESET_CASTLE_LONGPASSAGE");
  }
  static size_t selectedEnvironment = 0;
  for (size_t i = 0; i < environmentIds.size(); ++i) {
    std::string environmentLabel = "Environment " + std::to_string(i);
    if (ImGui::Selectable(environmentLabel.c_str(), selectedEnvironment == i)) {
      selectedEnvironment = i;
    }
    if (selectedEnvironment == i) {
      const auto& environmentId = environmentIds[i];

      auto* environment = ecs
                            .entity(environmentId)
                            .get_mut<Magnet::Components::Environment>();
      auto* environmentTransform = ecs
                                     .entity(environmentId)
                                     .get_mut<Magnet::Components::Transform>();
      assert(environment && environmentTransform);

      ImGui::DragFloat3("pos##EnvironmentPos", environmentTransform->position);
      ImGui::DragFloat3("scale##EnvironmentScale", environmentTransform->scale);

      const char* effectName = "";
      if (environment->effectName) {
        effectName = *environment->effectName;
      }
      if (ImGui::BeginCombo("Reverb##EnvironmentReverb", effectName)) {
        for (auto& [name, preset] : getEFXPresets()) {
          if (ImGui::Selectable(name.c_str(), effectName == name)) {
            environment->effectName = name.c_str();
          }
        }

        ImGui::EndCombo();
      }
    }
  }

  ImGui::Separator();
  ImGui::Text("Volume");
  ImGui::SliderFloat("MASTER##VolumeMASTER",
                     &audioManager.volumes().masterVolume, 0.f, 1.f);
  for (auto tag : Components::allAudioTags()) {
    std::string name =
      Components::to_string(tag) + "##Volume" + Components::to_string(tag);


    ImGui::SliderFloat(name.c_str(), &audioManager.volumes().volumes[tag], 0.f,
                       1.f);
  }

  ImGui::End();

  ImGui::Begin("Audio Source Editor");

vec3 listenerPos = {};
AudioManager::getListenerPos(listenerPos);
ImGui::DragFloat3("Listener Pos", listenerPos);
AudioManager::setListenerPos(listenerPos);

ImGui::Text("Audio Sources");

static size_t selectedSourceId = 0;

static size_t selectedTrackId = 0;
const char* selectedTrackName = nullptr;
if (selectedTrackId < trackNames.size()) {
  selectedTrackName = trackNames[selectedTrackId].c_str();
}
if (ImGui::BeginCombo("Tracks", selectedTrackName)) {
  for (size_t i = 0; i < trackNames.size(); ++i) {
    std::string trackName = trackNames[i] + "##Tracks" +
      std::to_string(i);
    if (ImGui::Selectable(trackName.c_str(), selectedTrackId == i)) {
      selectedTrackId = i;
    }
    if (selectedTrackId == i) {
      ImGui::Text(trackNames[selectedTrackId].c_str());
    }
  }

  ImGui::EndCombo();
}

static bool playTrackIsSpatial = true;
if (ImGui::Button("Play Track with selected source") &&
  selectedSourceId < sourceIds.size() &&
  selectedTrackId < trackNames.size()) {
  auto sourceId = sourceIds[selectedSourceId];
  auto* audioSource = ecs
    .entity(sourceId)
    .get_mut<Magnet::Components::AudioSource>();
  audioSource->playSound(trackNames[selectedTrackId].c_str(), playTrackIsSpatial);
}
ImGui::Checkbox("Is Spatial##PlayTrack", &playTrackIsSpatial);
ImGui::Separator();
if (ImGui::Button("New Audio Source")) {
  newAudioSource(ecs);
}
for (size_t i = 0; i < sourceIds.size(); ++i) {
  std::string name = "Source " + std::to_string(i);
  if (ImGui::Selectable(name.c_str(), selectedSourceId == i)) {
    selectedSourceId = i;
  }
  if (selectedSourceId == i) {
    auto sourceId = sourceIds[selectedSourceId];
    auto* audioSource =
      ecs.entity(sourceId).get_mut<Magnet::Components::AudioSource>();
    auto* sourceTransform =
      ecs.entity(sourceId).get_mut<Magnet::Components::Transform>();
    ImGui::DragFloat3("pos##AudioSourcePos", sourceTransform->position);
    ImGui::DragFloat("volume##AudioSourceVolume", &audioSource->volume, 0.01f,
                     0.f, 1.f);
    ImGui::DragFloat("pitch##AudioSourcePitch", &audioSource->pitch, 0.1f, 0.5f,
                     2.f);
    ImGui::Checkbox("looping##AudioSourceLooping", &audioSource->looping);

    bool coneEnabled = audioSource->cone.has_value();
    if (ImGui::Checkbox("enable cone##AudioSourceFilterCone", &coneEnabled)) {
      if (coneEnabled) {
        audioSource->cone = Components::Cone();
      } else {
        audioSource->cone = std::nullopt;
      }
    }
    if (audioSource->cone) {
      ImGui::SliderFloat("cone angle (deg)##AudioSourceConeAngle", &audioSource->cone->angleDeg, 0.f, 360.f);
      ImGui::SliderFloat("outer cone volume##AudioSourceConeOuterVol",
                         &audioSource->cone->outerGain, 0.f, 1.f);
    }

    bool filterEnabled = audioSource->filter.has_value();
    if (ImGui::Checkbox("enable filter##AudioSourceFilterEnable",
                        &filterEnabled)) {
      if (filterEnabled) {
        audioSource->filter = Components::Filter();
      } else {
        audioSource->filter = std::nullopt;
      }
    }
    if (audioSource->filter) {
      auto filterTypeToStr = [](Components::FilterType type) -> const char* {
        switch (type) {
          case Components::FilterType::BANDPASS:
            return "BANDPASS";
          case Components::FilterType::HIGHPASS:
            return "HIGHPASS";
          case Components::FilterType::LOWPASS:
            return "LOWPASS";
        }

        return nullptr;
      };

      Components::FilterType filterTypes[] = {Components::FilterType::BANDPASS,
                                              Components::FilterType::HIGHPASS,
                                              Components::FilterType::LOWPASS};

      const char* filterTypeStr = filterTypeToStr(audioSource->filter->type);
      if (ImGui::BeginCombo("Filter Type##AudioSourceFilterType",
                            filterTypeStr)) {
        for (auto type : filterTypes) {
          std::string name = filterTypeToStr(type) +
                             std::string("##AudioSourceFilterType") +
                             filterTypeToStr(type);

          if (ImGui::Selectable(name.c_str(),
                                type == audioSource->filter->type)) {
            audioSource->filter->type = type;
          }
        }
        ImGui::EndCombo();
      }

        ImGui::SliderFloat("filter volume##AudioSourceFilterVolume",
                         &audioSource->filter->gain, 0.f, 1.f);
        if (audioSource->filter->type == Components::FilterType::LOWPASS ||
            audioSource->filter->type == Components::FilterType::BANDPASS) {
          ImGui::SliderFloat("High Frequency Volume##AudioSourceFilterHFVolume",
                           &audioSource->filter->gainHighFrequency, 0.f, 1.f);
        }
        if (audioSource->filter->type == Components::FilterType::HIGHPASS ||
            audioSource->filter->type == Components::FilterType::BANDPASS) {
          ImGui::SliderFloat("Low Frequency Volume##AudioSourceFilterLFVolume",
                           &audioSource->filter->gainLowFrequency, 0.f, 1.f);
        }
      }

      if (ImGui::Button("Stop Source##AudioSource")) {
        audioSource->stop();
      }
    }
  
}

  ImGui::End();
}
flecs::entity App::newAudioSource(flecs::world &ecs) {
  auto newAudioSource = ecs.entity()
  .add<Magnet::Components::AudioSource>()
  .add<Magnet::Components::Transform>();
  sourceIds.push_back(newAudioSource.id());

  return newAudioSource;
}
flecs::entity App::newEnvironment(flecs::world& ecs, const char *name) {
  auto entity = ecs.entity();
  entity.set<Magnet::Components::Transform>({});
  entity.set<Magnet::Components::Environment>({name});
  environmentIds.push_back(entity.id());
  return entity;
}

void App::audioDebugInit() {
  
}
void App::audioDebugInit(flecs::world &ecs) {
  ImGui::Begin("Fuzz Sound Playground");

  

  ImGui::Separator();
  ImGui::Text("Debug Sounds Variables");
  auto newDebugInfo = AudioManager::getInstance().getDebugInfo();
  int slotCounts = static_cast<int>(newDebugInfo.slotsCount);
  ImGui::DragInt("slots count##FuzzSlotsCount", &slotCounts);
  int usedSlots = static_cast<int>(newDebugInfo.usedSlotsCount);
  ImGui::DragInt("used slots##FuzzUsedSlots", &usedSlots);
  int nonZero = static_cast<int>(newDebugInfo.nonZeroSlotReferenceCount);
  ImGui::DragInt("not zero references##FuzzNonZeroReferences", &nonZero);
  ImGui::Separator();

  ImGui::Text("Load Testers");
  static int numSourceToAllocate = 0;
  ImGui::DragInt("Num Sources To Allocate", &numSourceToAllocate, 1.f, 0);

  static bool uniqueEffects = false;
  ImGui::Checkbox("Fuzz unique effects", &uniqueEffects);

  if (ImGui::Button("Allocate and play sources")) {
    const size_t oldSize = sourceIds.size();
    if (!uniqueEffects) {
      for (size_t i = 0; i < numSourceToAllocate; ++i) {
        newAudioSource(ecs);
      }
    } else {
      auto efxIter = getEFXPresets().cbegin();

      for (size_t i = 0; i < numSourceToAllocate; ++i) {
        auto entity = newAudioSource(ecs);
        auto *transform = entity.get_mut<Components::Transform>();

        transform->position[0] = 0.05f * (i + oldSize);
        entity.modified<Components::Transform>();

        auto envEntity = newEnvironment(ecs, efxIter->first.c_str());
        auto* envTransform = envEntity.get_mut<Components::Transform>();
        envTransform->position[0] = 0.05f * (i + oldSize);
        envTransform->scale[0] = 0.01f;
        envTransform->scale[1] = 0.01f;
        envTransform->scale[2] = 0.01f;
        envEntity.modified<Components::Transform>();

        
        
        ++efxIter;
        if (efxIter == getEFXPresets().cend()) {
          efxIter = getEFXPresets().cbegin();
        }
      }
    }
    
    for (size_t i = 0; i < numSourceToAllocate; ++i) {
      auto sourceId = sourceIds[i + oldSize];
      
      auto entity = ecs.entity(sourceId);
      auto *source = entity.get_mut<Components::AudioSource>();

      source->volume = 0.01f;
      source->playSound(TEST_AUDIO_FILES[0]);

      entity.modified<Components::AudioSource>();
    }
  }
  if (ImGui::Button("Destroy all sources")) {
    while (!sourceIds.empty()) {
      auto sourceId = sourceIds.back();
      sourceIds.pop_back();

      ecs.entity(sourceId).destruct();
    }
  }
  if (ImGui::Button("Destroy all environments")) {
    while (!environmentIds.empty()) {
      auto environmentId = environmentIds.back();
      environmentIds.pop_back();

      ecs.entity(environmentId).destruct();
    }
  }

  ImGui::End();
}