#include "application.hpp"

#include <algorithm>

#include "outputwindow.hpp"

#include "utilities/utils.hpp"

#include "rendering/renderer.hpp"
#include "scene/scene.hpp"
#include "camera/interactivecamera.hpp"

#include "shaderfilewatcher.hpp"
#include "anttweakbarinterface.hpp"

#ifdef _WIN32
#undef APIENTRY
#include <windows.h>
#endif

Application::Application(int argc, char** argv)
{
	// Logger init.
	Logger::g_logger.Initialize(new Logger::FilePolicy("log.txt"));

	// Window...
	LOG_INFO("Init window ...");
	m_window.reset(new OutputWindow());

	// Create "global" camera.
	auto resolution = m_window->GetResolution();
	m_camera.reset(new InteractiveCamera(m_window->GetGLFWWindow(), ei::Vec3(0.0f, 2.5f, -5.0f), ei::Vec3(0.0f),
		static_cast<float>(resolution.x) / resolution.y, 0.1f, 10000.0f, 60.0f, ei::Vec3(0, 1, 0)));

	// Scene
	LOG_INFO("\nLoad scene ...");
	m_scene.reset(new Scene());

	// TODO: Some kind of runtime/data system is needed here.
	m_scene->AddModel("../models/test0/test0.obj");
	Light spot;
	spot.type = Light::Type::SPOT;
	spot.intensity = ei::Vec3(100.0f, 90.0f, 90.0f); 
	spot.position = ei::Vec3(0.0f, 2.5f, -2.5f);
	spot.direction = ei::normalize(-spot.position);
	spot.halfAngle = 30.0f * (ei::PI / 180.0f);
	m_scene->AddLight(spot);
	spot.position = ei::Vec3(1.0f, 2.5f, -2.5f);
	spot.intensity = ei::Vec3(70.0f, 70.0f, 80.0f);
	spot.direction = ei::normalize(ei::Vec3(1.0f, 2.5f, 0.0f) - spot.position);
	m_scene->AddLight(spot);

	// Renderer.
	LOG_INFO("\nSetup renderer ...");
	m_renderer.reset(new Renderer(m_scene, m_window->GetResolution()));

	// Watch shader dir.
	ShaderFileWatcher::Instance().SetShaderWatchDirectory("shader");

	SetupTweakBarBinding();
}

Application::~Application()
{
	// Only known method to kill the console.
#ifdef _WIN32
	FreeConsole();
#endif
	Logger::g_logger.Shutdown();

	m_scene.reset();
}

void Application::SetupTweakBarBinding()
{
	m_tweakBar = std::make_unique<AntTweakBarInterface>(m_window->GetGLFWWindow());
	m_tweakBar->AddReadOnly("Frametime (ms)", std::function<std::string(void)>([&](){ return std::to_string(m_timeSinceLastUpdate.GetMilliseconds()); }));
	m_tweakBar->AddReadWrite("Light0 Dir", std::function<ei::Vec3(void)>([&](){ return m_scene->GetLights()[0].direction; }),
		std::function<void(const ei::Vec3&)>([&](const ei::Vec3& dir){ m_scene->GetLights()[0].direction = dir; }));
}

void Application::Run()
{
	// Main loop
	ezStopwatch mainLoopStopWatch;
	while (m_window->IsWindowAlive())
	{
		m_timeSinceLastUpdate = mainLoopStopWatch.GetRunningTotal();
		mainLoopStopWatch.StopAndReset();
		mainLoopStopWatch.Resume();

		Update();
		Draw();
	}
}

void Application::Update()
{
	m_window->PollWindowEvents();

	ShaderFileWatcher::Instance().Update();

	m_camera->Update(m_timeSinceLastUpdate);
	Input();

	m_window->SetTitle("time per frame " +
		std::to_string(m_timeSinceLastUpdate.GetMilliseconds()) + "ms (FPS: " + std::to_string(1.0f / m_timeSinceLastUpdate.GetSeconds()) + ")");
}

void Application::Draw()
{
	m_renderer->Draw(*m_camera);
	m_tweakBar->Draw();
	m_window->Present();
}

void Application::Input()
{
}