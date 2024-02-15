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
  const size_t NUM_TEST_AUDIO_FILES = sizeof(TEST_AUDIO_FILES) / sizeof(TEST_AUDIO_FILES[0]);
  for (size_t i = 0; i < NUM_TEST_AUDIO_FILES; ++i) {
    std::vector<uint8_t> fileBuffer;
    this->archiveMgr->loadFile(TEST_AUDIO_FILES[i], fileBuffer);
    std::span<const uint8_t> fileSpan(fileBuffer.data(), fileBuffer.size());
    RawAudioData rawAudio = AudioManager::LoadAudio(fileSpan).value();
    std::span<const uint8_t> rawAudioData(rawAudio.data.data(), rawAudio.data.size());

    audioManager.registerAudio(TEST_AUDIO_FILES[i], rawAudioData, rawAudio.format, rawAudio.sampleRate);
    trackNames.push_back(TEST_AUDIO_FILES[i]);
  }

  audioManager.registerAllEfxPresets();

  auto& ecs = Scene::getECS();

  auto mainEntity = ecs.entity("MainEntity");
  mainEntity.set<Magnet::Components::AudioSource>({});
  mainEntity.set<Magnet::Components::Transform>({});
  this->mainEntityId = mainEntity.id();
  sourceIds.push_back(mainEntity.id());
  
  auto entity = ecs.entity("Environment");
  entity.set<Magnet::Components::Transform>({});
  entity.set<Magnet::Components::Environment>(
    {"EFX_REVERB_PRESET_GENERIC"});
  environmentIds.push_back(entity.id());
    
}

void Interface::update() {
  auto& audioManager = AudioManager::getInstance();
  auto* mainAudioSource = Scene::getECS()
                            .entity(this->mainEntityId)
                            .get_mut<Magnet::Components::AudioSource>();
  auto* mainAudioTransform = Scene::getECS()
                               .entity(this->mainEntityId)
                               .get_mut<Magnet::Components::Transform>();

  ImGui::Begin("Environments Editor");
  if (ImGui::Button("Try Play")) {
    mainAudioSource->playSound(TEST_AUDIO_FILES[1]);
  }


  ImGui::Separator();
  ImGui::Text("Environments");

  if (ImGui::Button("New Environment")) {
    auto entity = Scene::getECS().entity();
    entity.set<Magnet::Components::Transform>({});
    entity.set<Magnet::Components::Environment>(
      {"EFX_REVERB_PRESET_CASTLE_LONGPASSAGE"});
    environmentIds.push_back(entity.id());
  }
  static size_t selectedEnvironment = 0;
  for (size_t i = 0; i < environmentIds.size(); ++i) {
    std::string environmentLabel = "Environment " + std::to_string(i);
    if (ImGui::Selectable(environmentLabel.c_str(), selectedEnvironment == i)) {
      selectedEnvironment = i;
    }
    if (selectedEnvironment == i) {
      const auto& environmentId = environmentIds[i];

      auto* environment = Scene::getECS()
                            .entity(environmentId)
                            .get_mut<Magnet::Components::Environment>();
      auto* environmentTransform = Scene::getECS()
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
  if (ImGui::Button("Play Track with selected source") && 
      selectedSourceId < sourceIds.size() && 
      selectedTrackId < trackNames.size()) {
    auto sourceId = sourceIds[selectedSourceId];
    auto* audioSource = Scene::getECS()
                          .entity(sourceId)
                          .get_mut<Magnet::Components::AudioSource>();
    audioSource->playSound(trackNames[selectedTrackId].c_str());
  }

  ImGui::Separator();
  if (ImGui::Button("New Audio Source")) {
    auto newAudioSource = Scene::getECS().entity();
    newAudioSource.set<Magnet::Components::AudioSource>({});
    newAudioSource.set<Magnet::Components::Transform>({});
    sourceIds.push_back(newAudioSource);
  }
  for (size_t i = 0; i < sourceIds.size(); ++i) {
    std::string name = "Source " + std::to_string(i);
    if (ImGui::Selectable(name.c_str(), selectedSourceId == i)) {
      selectedSourceId = i;
    }
    if (selectedSourceId == i) {
      auto sourceId = sourceIds[selectedSourceId];
      auto* audioSource = Scene::getECS()
                            .entity(sourceId)
                            .get_mut<Magnet::Components::AudioSource>();
      auto* sourceTransform = Scene::getECS()
                                .entity(sourceId)
                                .get_mut<Magnet::Components::Transform>();
      ImGui::DragFloat3("pos##AudioSourcePos", sourceTransform->position);
      ImGui::DragFloat("volume##AudioSourceVolume", &audioSource->volume, 0.01f, 0.f, 1.f);
      ImGui::DragFloat("pitch##AudioSourcePitch", &audioSource->pitch, 0.1f, 0.5f, 2.f);
      ImGui::Checkbox("looping##AudioSourceLooping", &audioSource->looping);

      const bool isSpatialAudioIsDisabled = audioSource->state != Components::AudioPlayState::STOPPED;
      if (isSpatialAudioIsDisabled) {
        ImGui::BeginDisabled();
      }
      ImGui::Checkbox("isSpatial##AudioSourceSpatial", &audioSource->isSpatial);
      if (isSpatialAudioIsDisabled) {
        ImGui::EndDisabled();
      }
      if (ImGui::Button("Stop Source##AudioSource")) {
        audioSource->stop();
      }
    }
  }

  ImGui::End();
  /*
  // DEBUG: Remove for testing purposes only

  ImGui::Begin("Sound Editor");

  auto& audioManager = AudioManager::getInstance();
  auto& ecs = Scene::getECS();
  auto* mainAudioSource = ecs.entity(this->mainEntityId)
    .get_mut<Magnet::Components::AudioSource>();
  auto* mainTransform = ecs.entity(this->mainEntityId)
    .get_mut<Magnet::Components::Transform>();

  ImGui::DragFloat("volume", &mainAudioSource->volume, 0.01f, 0.f, 1.f);

  ImGui::DragFloat("pitch", &mainAudioSource->pitch, 0.01f, 0.5f, 2.f);


  if (ImGui::Button("Stop Request")) {
    mainAudioSource->stop();
  }

  static int selectedFilterType = 0;
  static std::optional<AudioFilterDescription> desc;
  const char* FILTER_TYPES_STR[] = { "None", "Lowpass", "Highpass", "Bandpass" };
  std::optional<AudioFilterType> FILTER_TYPE[] = {
    std::nullopt, AudioFilterType::LOWPASS,
    AudioFilterType::HIGHPASS, AudioFilterType::BANDPASS
  };
  if (ImGui::BeginCombo("Filter Settings", FILTER_TYPES_STR[selectedFilterType])) {
    for (int i = 0; i < IM_ARRAYSIZE(FILTER_TYPES_STR); ++i) {
      if (ImGui::Selectable(FILTER_TYPES_STR[i], selectedFilterType == i)) {
        selectedFilterType = i;
        if (FILTER_TYPE[i] != std::nullopt) {
          desc = AudioFilterDescription(*FILTER_TYPE[i]);
        }
      }
    }
    ImGui::EndCombo();
  }

  if (FILTER_TYPE[selectedFilterType] && desc) {
    ImGui::SliderFloat("gain##Filter_gain", &desc->gain, 0.f, 1.f);

    if (
      *FILTER_TYPE[selectedFilterType] == AudioFilterType::LOWPASS ||
      *FILTER_TYPE[selectedFilterType] == AudioFilterType::BANDPASS) {
      ImGui::SliderFloat("High Frequency##Filter_HighFrequency", &desc->gainHighFrequency, 0.f, 1.f);
    }
    if (
      *FILTER_TYPE[selectedFilterType] == AudioFilterType::HIGHPASS ||
      *FILTER_TYPE[selectedFilterType] == AudioFilterType::BANDPASS) {
      ImGui::SliderFloat("Low Frequency##Filter_LowFrequency", &desc->gainLowFrequency, 0.f, 1.f);
    }
  }

  static std::optional<std::string> selectedPreset = std::nullopt;
  const char* selectedPresetCStr = nullptr;
  if (selectedPreset) {
    selectedPresetCStr = selectedPreset->c_str();
  }
  if (ImGui::BeginCombo("Reverb Settings", selectedPresetCStr)) {
    for (auto& [name, preset] : getEFXPresets()) {
      if (ImGui::Selectable(name.c_str(), selectedPreset == name)) {
        selectedPreset = name;
      }
    }

    ImGui::EndCombo();
  }

  static int selectedAudioFile = 0;
  if (ImGui::BeginCombo("Sounds", TEST_AUDIO_FILES[selectedAudioFile])) {
    for (size_t i = 0; i < sizeof(TEST_AUDIO_FILES) / sizeof(TEST_AUDIO_FILES[0]); ++i) {
      if (ImGui::Selectable(TEST_AUDIO_FILES[i], selectedAudioFile == i)) {
        selectedAudioFile = i;
      }
    }

    ImGui::EndCombo();
  }


  std::optional<EAXReverbDescription> reverb = std::nullopt;
  if (selectedPreset) {
    reverb = getEFXPresets().at(*selectedPreset);
  }
  auto testAudioFile = TEST_AUDIO_FILES[selectedAudioFile];
  auto tag = TEST_AUDIO_FILE_TAGS[selectedAudioFile];
  if (ImGui::Button("Play Audio File")) {
    mainAudioSource->play_sound(
      testAudioFile,
      tag,
      false,
      1.f,
      desc,
      reverb
    );
  }
  ImGui::SameLine();
  auto tagStr = to_string(tag);
  ImGui::Text("%s", tagStr.c_str());

  static std::vector<AudioBuffer*> customBuffers;
  static std::optional<Recorder> recorder = Recorder::create();
  if (!recorder) {
    if (ImGui::Button("Retry Create Audio Device")) {
      recorder = Recorder::create();
    }
    ImGui::Text("No audio capture device was found");
  }
  if (ImGui::Button("Start Recording") && recorder && !recorder->isRecording()) {
    recorder->startCapture();
  }
  if (recorder && recorder->isRecording()) {
    recorder->update();
  }
  ImGui::SameLine();
  if (ImGui::Button("Stop Recording") && recorder && recorder->isRecording()) {
    recorder->stopCapture();
    if (auto buffer = AudioBuffer::create(
      recorder->toSpan(), 
      recorder->getFormat(), 
      Recorder::FREQUENCY
    )) {
      auto* newBuffer = new AudioBuffer(*buffer);
      // This creates a memory leak but it is just a hold over untill
      // the asset id system is complete
      customBuffers.push_back(newBuffer); 
    } else {
      spdlog::warn("AudioBuffer could not be created from recording");
    }
    recorder->clear();
  }

  
  static std::optional<int> selectedCustomAudio = std::nullopt;
  std::string customAudioPreview = "";
  if (selectedCustomAudio) {
    customAudioPreview = "Custom Audio:" + std::to_string(*selectedCustomAudio);
  }

  if (ImGui::BeginCombo("Custom Audio List", customAudioPreview.c_str())) {
    for (size_t i = 0; i < customBuffers.size(); ++i) {
      std::string audioPreview = "Custom Audio:" + std::to_string(i);

      if (ImGui::Selectable(audioPreview.c_str(), selectedCustomAudio == i)) {
        selectedCustomAudio = i;
      }
    }

    ImGui::EndCombo();
  }
  if (ImGui::Button("Play Custom Audio") && selectedCustomAudio) {
    mainAudioSource->play_sound(customBuffers.at(*selectedCustomAudio), tag, false, 1.f, desc, reverb);
  }


  
  ImGui::End();

  ImGui::Begin("Sound Playground Demo");
  
  ImGui::Separator();
  
  ImGui::Text("Emitter");
  ImGui::DragFloat3("position##Emitter_position", mainTransform->position);
  static float angleDegrees = 0.f, radius = 2.f;
  ImGui::Text("This rotates the entity around the listeners head the xz axis");
  ImGui::DragFloat("Entity Rotation", &angleDegrees, 1.f, 0.f, 360.f);
  ImGui::DragFloat("Entity Radius from origin", &radius, 0.01f, 0.f, 10.f);

  mainTransform->position[2] = radius * cosf(angleDegrees * M_PI / 180);
  mainTransform->position[0] = radius * sinf(angleDegrees * M_PI / 180);

  ImGui::Separator();

  ImGui::Text("Volume Adjustor");
  ImGui::SliderFloat("Master Volume:", &audioManager.getMaster().volume, 0.f, 1.f);
  const char* AUDIO_TAG_STR[] = { "VOICE", "SOUND_EFFECTS", "MUSIC" };
  for (size_t i = 0; i < sizeof(AUDIO_TAG_STR) / sizeof(AUDIO_TAG_STR[0]); ++i) {
    auto *volume = &audioManager.getTagModifier(static_cast<Magnet::AudioTag>(i)).volume;
    ImGui::SliderFloat(AUDIO_TAG_STR[i], volume, 0.f, 1.f);;
  }

  ImGui::End();
  */
}
