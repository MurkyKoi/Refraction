#pragma once

#include <deque>
#include <filesystem>

#include <Core/Common.h>
#include <Classes/Objects/AObject.h>
#include <Math/Rect.h>
#include <Interface/Project.h>

namespace Refraction::Editor {
	struct TempEditorState {
		Common::Shared<Objects::AObject> SelectedObject = nullptr;
		Common::Shared<Engine::Project> ProjectInstance = nullptr;
		bool SimulatingGame = false;
		bool ViewportHovered = false;

		bool PanelViewportVisible = true;
		bool PanelPropertiesVisible = true;
		bool PanelExplorerVisible = true;
		bool PanelStatisticsVisible = true;
		bool PanelLiveCollabVisible = false;
		bool PanelLogVisible = true;

		bool DialogExportVisible = false;
		bool DialogCreateProjectVisible = false;
		bool DialogAddObjectVisible = false;
		bool DialogAddComponentVisible = false;
	};
	struct PersistentEditorState {
		std::filesystem::path ExecutableDir = "";
		std::filesystem::path ResourcesDir = "";
		Math::Rect WindowRect = Math::Rect(256, 256, 1280, 720);
		std::deque<std::filesystem::path> RecentProjects = {};
	};

	class EditorState {
	public:
		static TempEditorState Temp;
		static PersistentEditorState Persistent;

		static bool Serialise();
		static bool Deserialise();
		static void AddToRecentProjects(const std::filesystem::path &projectFilePath);
	};
}
