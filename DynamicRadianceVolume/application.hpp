#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Time/Stopwatch.h"

class OutputWindow;
class Scene;
class InteractiveCamera;
class Renderer;
class AntTweakBarInterface;
class FrustumOutlines;

class Application
{
public:
	Application(int argc, char** argv);

	~Application();

	void Run();

private:
	void SetupTweakBarBinding();

	void Update();

	void Draw();

	void Input();

	std::string OpenFileDialog();
	std::string SaveFileDialog(const std::string& defaultName, const std::string& fileEnding);

	void AddEntity(const std::string& filename);

	void ChangeEntityCount(unsigned int entityCount);
	void ChangeLightCount(unsigned int lightCount);

	std::unique_ptr<OutputWindow> m_window;
	std::shared_ptr<Scene> m_scene;
	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<InteractiveCamera> m_camera;
	std::unique_ptr<AntTweakBarInterface> m_tweakBar;

	std::unique_ptr<FrustumOutlines> m_frustumOutlineRenderer;
	bool m_detachViewFromCameraUpdate;

	ezTime m_timeSinceLastUpdate;

	bool m_showTweakBar;

	std::string m_tweakBarStatisticGroupSetting;	
	std::vector<std::string> m_tweakBarStatisticEntries;
};
