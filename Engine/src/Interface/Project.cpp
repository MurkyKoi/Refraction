#include <fstream>

#include <json.hpp>

#include <Core/FileHandling.h>
#include <Classes/Objects/BasicObject.h>
#include <Classes/ClassSerialiser.h>
#include <Interface/AssetImporter.h>

#include "Project.h"

namespace Refraction::Engine {
	bool SaveProjectData(const std::filesystem::path& projectFilePath, const ProjectData& projectData) {
		const auto pathStr = projectFilePath.string();
		if (!(projectFilePath.has_extension() && projectFilePath.extension() == RFCT_PROJECT_EXTENSION)) {
			Log::Project.Error("Invalid project file extension in path " + pathStr);
			return false;
		}
		if (!std::filesystem::exists(projectFilePath.parent_path())) {
			Log::Project.Error("Invalid project directory in path " + pathStr);
			return false;
		}

		const auto serialised = Utilities::ClassSerialiser::AppendJSON({}, [&](nlohmann::json& json) {
			json["InitSceneUUID"] = projectData.InitSceneUUID.Serialise();
			json["ActiveCameraUUID"] = projectData.ActiveCamera->GetUUID().Serialise();
			json["Scenes"] = {};
			for (auto& scene : projectData.Scenes) {
				json["Scenes"][scene->GetUUID().Serialise()] = Utilities::ClassSerialiser::Serialise(scene);
				Log::Project.Info("Serialised scene with UUID " + scene->GetUUID().AsString());
			}
			json["GlobalObjects"] = {};
			for (auto& globalObj : projectData.GlobalObjects) {
				json["GlobalObjects"][globalObj->GetUUID().Serialise()] = Utilities::ClassSerialiser::Serialise(globalObj);
			}
		});
		
		std::ofstream dataFile(projectFilePath);
		if (!dataFile.is_open()) {
			Log::Project.Error("Could not open path " + pathStr + " for writing.");
			return false;
		}
		dataFile << serialised.dump(RFCT_JSON_INDENT);

		Log::Project.Info("Saved project data to " + pathStr);
		return true;
	}

	std::optional<ProjectData> LoadProjectData(const std::filesystem::path& projectFilePath) {
		const auto pathStr = projectFilePath.string();
		if (!(std::filesystem::exists(projectFilePath) && std::filesystem::is_regular_file(projectFilePath) && projectFilePath.has_extension() && projectFilePath.extension() == RFCT_PROJECT_EXTENSION)) {
			Log::Project.Warn("Invalid or missing project file at path " + pathStr);
			return std::nullopt;
		}
		
		std::optional<ProjectData> result = std::nullopt;
		const auto contents = FileHandling::ReadFile(projectFilePath);
		Utilities::ClassSerialiser::TryParseJSON(contents, [&](nlohmann::json& json) {
			ProjectData deserialised;
			deserialised.InitSceneUUID = UUID::Deserialise(json["InitSceneUUID"]);
			Log::Project.Info("Loaded InitSceneUUID as " + deserialised.InitSceneUUID.AsString());
			for (auto& sceneData : json.at("Scenes")) {
				auto scene = Utilities::ClassSerialiser::DeserialiseObject<Objects::SceneRoot>(sceneData.dump());
				Log::Project.Info("Loaded Scene with UUID " + scene->GetUUID().AsString());
				deserialised.Scenes.push_back(scene);
			}

			const bool cameraDefined = json.contains("ActiveCameraUUID");
			UUID cameraUUID;
			if (cameraDefined) cameraUUID = UUID::Deserialise(json["ActiveCameraUUID"]);

			for (auto& globalObjData : json.at("GlobalObjects")) {
				auto object = Utilities::ClassSerialiser::DeserialiseObject(globalObjData.dump());
				deserialised.GlobalObjects.push_back(object);
				if (cameraDefined && (object->GetUUID() == cameraUUID)) {
					const auto camera = dynamic_pointer_cast<Objects::Camera>(object);
					Objects::Camera::ActiveCamera = camera;
					deserialised.ActiveCamera = camera;
				};
			}

			if (!cameraDefined || !deserialised.ActiveCamera) {
				Log::Project.Warn("No active camera set, creating new camera");
				const auto camera = Common::NewShared<Objects::Camera>();
				Objects::Camera::ActiveCamera = camera;
				deserialised.ActiveCamera = camera;
				deserialised.GlobalObjects.push_back(camera);
			}
			result = std::make_optional(deserialised);
		});

		return result;
	}

	std::filesystem::path GetProjectFilePath(const std::filesystem::path& projectPath) {
		if (!std::filesystem::exists(projectPath)) {
			Log::Project.Error("Project path " + projectPath.string() + " does not exist");
			return {};
		}
		if (projectPath.empty()) {
			Log::Project.Error("Project path " + projectPath.string() + " is empty");
			return {};
		}
		const std::string projectName = projectPath.filename().string();
		if (projectName.empty()) {
			Log::Project.Error("Could not get project name from path " + projectPath.string());
			return {};
		}
		return projectPath / (projectName + RFCT_PROJECT_EXTENSION);
	}

	bool Project::New(const std::filesystem::path& projectPath, bool eraseExisting) {
		auto pathStr = projectPath.string();
		if (!std::filesystem::exists(projectPath)) {
			Log::Project.Warn("Could not create project, path does not exist");
			return false;
		}
		if (!std::filesystem::is_directory(projectPath)) {
			Log::Project.Warn(pathStr + " is not a valid directory");
			return false;
		}

		if (std::filesystem::exists(projectPath) && !std::filesystem::is_empty(projectPath)) {
			if (!eraseExisting) {
				if (FileHandling::GetFirstFileOfExtInFolder(projectPath, RFCT_PROJECT_EXTENSION).exists()) {
					// TODO: TEMP: ignore !eraseExisting if there is a project file inside
					for (auto items = FileHandling::GetItemsInFolder(projectPath); auto& item : items) {
						if(!std::filesystem::remove_all(item)) Log::Project.Warn("Failed to erase " + item.path().filename().string());
					}
				} else {
					Log::Project.Error(pathStr + " already exists and eraseExisting is false");
					return false;
				}
			} else {
				std::filesystem::remove_all(projectPath);
			}
		} else if (!std::filesystem::exists(projectPath)) {
			if (!std::filesystem::create_directory(projectPath)) {
				Log::Project.Error("Failed to create project directory at " + pathStr);
				return false;
			}
		}
		if (!std::filesystem::create_directory(projectPath / "Assets")) {
			Log::Project.Error("Failed to create project assets directory at " + pathStr);
			return false;
		}

		Log::Project.Info("Creating project at " + pathStr);

		// Placeholder project file
		auto projectFilePath = GetProjectFilePath(projectPath);
		std::ofstream dataFile(projectFilePath);
		if (!dataFile.is_open()) {
			Log::Project.Error("Could not open path " + pathStr + " for writing.");
			return false;
		}
		dataFile << "hi";
		dataFile.close();

		mRootObject = Common::NewShared<Objects::AObject>();
		mProjectPath = projectPath;
		mProjectData = ProjectData{};

		// Import default assets
		auto assetManager = AssetManager::MakeInstance(projectPath).lock();
		if (!assetManager) throw Common::RuntimeError("Failed to instantiate AssetManager");
		AssetImporter::Import(FileHandling::GetResourcesPath() / "textures" / "Basic.png");
		for (auto& shader : FileHandling::GetFoldersInFolder(FileHandling::GetResourcesPath() / "shaders")) {
			AssetImporter::Import(shader.path());
		}

		// Default global objects
		auto camera = Common::NewShared<Objects::Camera>();
		Objects::Camera::ActiveCamera = camera;
		mRootObject->AddChild(camera);
		mProjectData.ActiveCamera = camera;
		mProjectData.GlobalObjects.push_back(camera);

		auto nyenMeshUUID = AssetImporter::Import(FileHandling::GetResourcesPath() / "models/nyen/nyen plush.obj");
		auto testMesh = Common::NewShared<Objects::BasicObject>();
		camera->AddChild(testMesh);
		testMesh->GetComponent<Components::Mesh>()->mModel = assetManager->GetAsset<Assets::Model>(nyenMeshUUID);

		if (!Save()) {
			Log::Project.Error("Failed to create initial save of project data at " + pathStr);
			return false;
		}
		NewScene();
		Log::Project.Info("Created project at " + pathStr);
		return true;
	}

	bool Project::NewRemote() {
		return false;
	}

	bool Project::Open(const std::filesystem::path& projectPath) {
		const auto pathStr = projectPath.string();
		if (!std::filesystem::exists(projectPath) || !std::filesystem::is_regular_file(projectPath) || projectPath.extension() != RFCT_PROJECT_EXTENSION) {
			Log::Project.Error("Attempt to open invalid project path at " + pathStr);
			return false;
		}

		// Close any active project
		if (IsLoaded()) Close();

		const auto projectFolderPath = projectPath.parent_path();
		mProjectPath = projectFolderPath;

		const auto assetManager = AssetManager::MakeInstance(mProjectPath).lock();
		if (!assetManager) throw Common::RuntimeError("Failed to instantiate AssetManager");

		// Create new root
		mRootObject = Common::NewShared<Objects::AObject>();

		AssetManager::Try([&](const Common::Shared<AssetManager>& manager) {
			manager->RegisterAllAssets();
		});

		const auto actualProjectFilePath = GetProjectFilePath(projectFolderPath);
		const auto projectData = LoadProjectData(actualProjectFilePath);
		mProjectData = projectData.value_or(ProjectData{});

		if (!projectData) Log::Project.Warn("Failed to load project data at " + pathStr);

		for (auto& scene : mProjectData.Scenes) {
			mRootObject->AddChild(scene);
		}
		for (auto& globalObj : mProjectData.GlobalObjects) {
			mRootObject->AddChild(globalObj);
		}

		// Open init scene
		if (mProjectData.InitSceneUUID != UUID::Null()) {
			if (!OpenScene(mProjectData.InitSceneUUID)) {
				Log::Project.Warn("Failed to open specified InitScene, using first scene found instead");
				if (!mProjectData.Scenes.empty()) {
					mProjectData.InitSceneUUID = mProjectData.Scenes[0]->GetUUID();
					OpenScene(mProjectData.InitSceneUUID);
				}
			}
		}

		Log::Project.Info("Opened project at " + pathStr);
		return true;
	}

	bool Project::Save() const {
		if (!IsLoaded()) {
			Log::Project.Warn("Attempt to save project when one isn't loaded");
			return false;
		}

		bool success = true;
		if (const auto projectFilePath = GetProjectFilePath(mProjectPath); !SaveProjectData(projectFilePath, mProjectData)) {
			success = false;
			Log::Project.Error("Failed to save project data at " + projectFilePath.string());
		} else Log::Project.Info("Saved project data at " + projectFilePath.string());

		return success;
	}

	void Project::Close() {
		if (!IsLoaded()) {
			Log::Project.Warn("Attempt to close project when one isn't loaded");
			return;
		}

		Log::Project.Info("Closing project at " + mProjectPath.string());

		Objects::Camera::ActiveCamera = nullptr;
		for (auto& scene : mProjectData.Scenes) {
			scene.reset();
		}
		for (auto& globalObject : mProjectData.GlobalObjects) {
			globalObject.reset();
		}
		mProjectPath.clear();
		mProjectData = ProjectData{};

		AssetManager::Try([&](const Common::Shared<AssetManager>& manager) {
			manager->UnloadAll();
		});
		Log::Project.Info("Closed project successfully");
	}

	void Project::ProcessRemoteMessage(const std::string& message) {
		if (!IsRemote()) return; // Not a remote project so this shouldn't run

		Utilities::ClassSerialiser::TryParseJSON(message, [&](nlohmann::json json) {
			if (!json.contains("Command")) {
				Log::Editor.Warn("Unable to process remote message");
				return;
			}

			switch (json.at("Command").get<RemoteProjectCommand>()) {
				case RemoteProjectCommand::AddObject:
				{
					if (!json.contains("ParentUUID")) {
						Log::Editor.Warn("No Parent UUID provided with command, ignoring message");
						return;
					}
					if (!json.contains("SerialisedObject")) {
						Log::Editor.Warn("No valid serialised data provided with command, ignoring message");
						return;
					}
					auto uuid = UUID::FromExisting(json.at("ParentUUID").get<uint64_t>(), true);
					auto serialised = json.at("SerialisedObject");

					// Get target with given UUID
					Objects::AObject* parent = nullptr;
					for (auto& obj : mProjectData.GlobalObjects) {
						if (obj->GetUUID() == uuid) {
							parent = obj.get();
							break;
						}
						parent = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
						if (parent) break;
					}
					if (!parent) {
						for (auto& scene : mProjectData.Scenes) {
							parent = Objects::AObject::GetInstanceWithUUID(uuid, scene.get());
							if (parent) break;
						}
					}

					// Ignore if no target found
					if (!parent) break;

					// Add new object
					auto newObj = Utilities::ClassSerialiser::DeserialiseObject(serialised.dump());
					newObj->mParent = parent;

					Log::Editor.Info("Successfully added object " + uuid.AsString() + " from remote message");
					break;
				}
				case RemoteProjectCommand::AddComponent:
				{
					if (!json.contains("ParentUUID")) {
						Log::Editor.Warn("No Parent UUID provided with command, ignoring message");
						return;
					}
					if (!json.contains("SerialisedComponent")) {
						Log::Editor.Warn("No valid serialised data provided with command, ignoring message");
						return;
					}
					auto uuid = UUID::FromExisting(json.at("ParentUUID").get<uint64_t>(), true);
					auto serialised = json.at("SerialisedComponent");

					// Get target with given UUID
					Objects::AObject* parent = nullptr;
					for (auto& obj : mProjectData.GlobalObjects) {
						if (obj->GetUUID() == uuid) {
							parent = obj.get();
							break;
						}
						parent = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
						if (parent) break;
					}
					if (!parent) {
						for (auto& scene : mProjectData.Scenes) {
							parent = Objects::AObject::GetInstanceWithUUID(uuid, scene.get());
							if (parent) break;
						}
					}

					// Ignore if no target found
					if (!parent) break;

					// Add new component
					auto newComp = Utilities::ClassSerialiser::DeserialiseComponent(serialised.dump());
					newComp->mParent = parent;

					Log::Editor.Info("Successfully added component " + uuid.AsString() + " from remote message");
					break;
				}
				case RemoteProjectCommand::UpdateObject:
				{
					if (!json.contains("UUID")) {
						Log::Editor.Warn("No UUID provided with command, ignoring message");
						return;
					}
					if (!json.contains("SerialisedObject")) {
						Log::Editor.Warn("No valid serialised data provided with command, ignoring message");
						return;
					}
					auto uuid = UUID::FromExisting(json.at("UUID").get<uint64_t>(), true);
					std::string serialised = json.at("SerialisedObject");

					// Get target with given UUID
					Objects::AObject* target = nullptr;
					for (auto& obj : mProjectData.GlobalObjects) {
						if (obj->GetUUID() == uuid) {
							target = obj.get();
							break;
						}
						target = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
						if (target) break;
					}
					if (!target) {
						for (auto& scene : mProjectData.Scenes) {
							target = Objects::AObject::GetInstanceWithUUID(uuid, scene.get());
							if (target) break;
						}
					}

					// Ignore if no target found
					if (!target) break;

					// Update object
					target->Deserialise(serialised);

					Log::Editor.Info("Successfully updated object " + uuid.AsString() + " from remote message");
					break;
				}
				case RemoteProjectCommand::UpdateComponent:
				{
					if (!json.contains("UUID")) {
						Log::Editor.Warn("No UUID provided with command, ignoring message");
						return;
					}
					if (!json.contains("SerialisedComponent")) {
						Log::Editor.Warn("No valid serialised data provided with command, ignoring message");
						return;
					}
					auto uuid = UUID::FromExisting(json.at("UUID").get<uint64_t>(), true);
					std::string serialised = json.at("SerialisedComponent");

					// Get target with given UUID
					Objects::AObject* targetParent = nullptr;
					Components::AComponent* target = nullptr;
					for (auto& obj : mProjectData.GlobalObjects) {
						targetParent = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
						if (targetParent) break;
					}
					if (!target) {
						for (auto& scene : mProjectData.Scenes) {
							targetParent = Objects::AObject::GetInstanceWithUUID(uuid, scene.get());
							if (targetParent) break;
						}
					}

					// Ignore if no target parent found (therefore component doesn't exist)
					if (!targetParent) break;

					for (auto& comp : *targetParent->GetComponents()) {
						if (comp->GetUUID() != uuid) continue;
						target = comp.get();
						break;
					}

					// Update object
					target->Deserialise(serialised);

					Log::Editor.Info("Successfully updated component " + uuid.AsString() + " from remote message");
					break;
				}
				case RemoteProjectCommand::RemoveInstance:
				{
					if (!json.contains("UUID")) {
						Log::Editor.Warn("No UUID provided with command, ignoring message");
						return;
					}
					auto uuid = UUID::FromExisting(json.at("UUID").get<uint64_t>(), true);

					// Get target with given UUID
					Objects::AObject* target = nullptr;
					for (size_t i = 0; i < mProjectData.GlobalObjects.size(); i++) {
						auto& obj = mProjectData.GlobalObjects[i];
						// Test if this object is the target
						if (obj->GetUUID() == uuid) {
							mProjectData.GlobalObjects.erase(std::next(mProjectData.GlobalObjects.begin(), i - 1));
							break;
						}
						// Test descendants
						target = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
						if (target) break;
					}

					if (!target) {
						for (size_t i = 0; i < mProjectData.Scenes.size(); i++) {
							auto& obj = mProjectData.Scenes[i];
							// Test if this object is the target
							if (obj->GetUUID() == uuid) {
								mProjectData.Scenes.erase(std::next(mProjectData.Scenes.begin(), i - 1));
								break;
							}
							// Test descendants
							target = Objects::AObject::GetInstanceWithUUID(uuid, obj.get());
							if (target) break;
						}
					}

					// Ignore if no target found
					if (!target) break;

					target->RemoveChild(uuid);

					break;
				}
			}
		});
	}

	Common::Ref<Objects::SceneRoot> Project::NewScene() {
		Log::Project.Info("Creating a new scene");
		const auto newScene = Common::NewShared<Objects::SceneRoot>();
		mRootObject->AddChild(newScene);
		mProjectData.Scenes.push_back(newScene);

		// Instantiate default objects/components
		///

		const auto assetManager = AssetManager::GetInstance().lock();
		if (!assetManager) throw Common::RuntimeError("Failed to create scene, no AssetManager instance");

		const auto nyenMeshUUID = AssetImporter::Import(FileHandling::GetResourcesPath() / "models/nyen/nyen plush.obj");
		const auto nyenObj = Common::NewShared<Objects::BasicObject>();
		nyenObj->mInstanceName = "Nyen";
		nyenObj->GetComponent<Components::Mesh>()->mModel = assetManager->GetAsset<Assets::Model>(nyenMeshUUID);
		nyenObj->GetComponent<Components::APhysics>()->mAngularVelocity = Math::Vector3(0, 64, 0);
		newScene->AddChild(nyenObj);

		const auto backpackMeshUUID = AssetImporter::Import(FileHandling::GetResourcesPath() / "models/survivalBackpack/backpack.obj");
		const auto backpackObj = Common::NewShared<Objects::BasicObject>();
		backpackObj->mInstanceName = "Backpack";
		backpackObj->GetComponent<Components::Mesh>()->mModel = assetManager->GetAsset<Assets::Model>(backpackMeshUUID);
		backpackObj->mTransform = Math::Transform::FromLookAt(Math::Vector3(0, 14, 10), Math::Vector3::Zero());
		newScene->AddChild(backpackObj);

		Log::Project.Info("Successfully created a new scene with UUID " + newScene->GetUUID().AsString());
		mActiveScene = newScene;
		// Autoset as initScene if none is defined
		if (mProjectData.InitSceneUUID == UUID::Null()) {
			mProjectData.InitSceneUUID = newScene->GetUUID();
			// Add baseplate for convenience
			const auto baseplateMeshUUID = AssetImporter::Import(FileHandling::GetResourcesPath() / "models/Basic/Cube.obj");
			const auto baseplate = Common::NewShared<Objects::AObject>();
			baseplate->mInstanceName = "Baseplate";
			const auto comp = baseplate->AddComponent<Components::Mesh>();
			comp->mModel = assetManager->GetAsset<Assets::Model>(baseplateMeshUUID);
			comp->mTransform.Translate(Math::Vector3(0, -8, 0));
			comp->mTransform.mScale = Math::Vector3(128, 8, 128);
			newScene->AddChild(baseplate);
		}
		return mActiveScene;
	}

	bool Project::OpenScene(const UUID& sceneUUID) {
		Common::Shared<Objects::SceneRoot> targetScene;
		for (const auto& scene : mProjectData.Scenes) {
			if (scene->GetUUID().AsInt() == sceneUUID.AsInt()) {
				targetScene = scene;
				break;
			}
		}
		if (!targetScene) {
			Log::Project.Error("Invalid Scene UUID provided (" + sceneUUID.AsString() + ")");
			return false;
		}
		Log::Project.Info("Opening scene with UUID " + sceneUUID.AsString());

		mActiveScene = targetScene;
		return true;
	}
}