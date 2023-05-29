#include "FrameworkCore/Core_All.h"
#include "AssetLoader.h"

#ifdef EFFECT_PREVIEW
#include <thread>
#include <chrono>
using namespace std::literals;
#endif

using namespace OpenGLFramework;

void SetProjection(float width, float height, float near, float far, 
	Core::Camera& camera, Core::Shader& shader)
{
	auto projectionMat = glm::perspective(glm::radians(camera.fov),
		width / height, near, far);
	shader.SetMat4("projection", projectionMat);
	return;
}

void AllViewsCapture(Core::MainWindow& mainWindow, Core::Camera& camera,
    float totalRotationAngle = 360, int totalTime = 30, int beginID = 0)
{
    mainWindow.Register([totalRotationAngle, totalTime,
        beginID, &mainWindow, &camera]{
        static Core::Camera savedCamera = camera;
        static int time = 0;
        static int maxTime = totalTime, timeSegment = 3;
        if(time == maxTime * timeSegment){
            mainWindow.Close();
        }
        int segment = time / maxTime;
        camera.RotateAroundCenter(totalRotationAngle / maxTime, 
                {0, camera.GetPosition().y, 0}, {0, 1, 0});
        if(time % maxTime != 0)
        {
#ifdef EFFECT_PREVIEW
            std::this_thread::sleep_for(100ms);
#else
            mainWindow.SaveImage("GenMapTestResult/" + 
                                 std::to_string(time + beginID) + ".png");
#endif
        }
        else if(segment == 1){
            camera = savedCamera;
            camera.RotateAroundCenter(30, {1, 0, 0}, {0, 1, 0});
        }
        else if(segment == 2){
            camera = savedCamera;
            camera.RotateAroundCenter(-30, {1, 0, 0}, {0, 1, 0});
        }
        time++;
    });
}

int main()
{
    [[maybe_unused]] auto& manager = Core::ContextManager::GetInstance();
    Asset::AssetLoader loader{ CONFIG_PATH, MODEL_DIR, SHADER_DIR };
    auto& mainWindow = loader.GetMainWindowInstance();

    auto& model = loader.GetModel("sucrose");
    model.transform.scale = { 0.125, 0.125, 0.125 };

    auto& shader = loader.GetShader("gen_norm");
	Core::Camera camera{ { 0, 1.25, 3.5}, {0, 1, 0}, {0, 0, -1} };

    float near = 0.1f, far = 100.0f;
    mainWindow.Register([&](){
        shader.Activate();
        shader.SetMat4("model", model.transform.GetModelMatrix());
        shader.SetMat4("view", camera.GetViewMatrix());
        const auto[width, height] = mainWindow.GetWidthAndHeight();
        SetProjection(static_cast<float>(width), static_cast<float>(height), 
                      near, far, camera, shader);
        model.Draw(shader);
    });

    std::filesystem::create_directories("GenMapTestResult");
    // AllViewsCapture(mainWindow, camera);
    AllViewsCapture(mainWindow, camera, -30, 10, 30);

    mainWindow.MainLoop({0.0, 0.0, 0.0, 0.0});
    return 0;
}