#include "FrameworkCore/Core_All.h"
#include "AssetLoader.h"

using namespace OpenGLFramework;

void SetProjection(float width, float height, float near, float far, 
	Core::Camera& camera, Core::Shader& shader)
{
	auto projectionMat = glm::perspective(glm::radians(camera.fov),
		width / height, near, far);
	shader.SetMat4("projection", projectionMat);
	return;
}

int main()
{
    [[maybe_unused]] auto& manager = Core::ContextManager::GetInstance();
    Asset::AssetLoader loader{ CONFIG_PATH, MODEL_DIR, SHADER_DIR };
    auto& mainWindow = loader.GetMainWindowInstance();

    auto& model = loader.GetModel("sucrose");
    model.transform.scale = { 0.1, 0.1, 0.1 };

    auto& shader = loader.GetShader("gen_norm");
	Core::Camera camera{ { 0, 1, 3.5}, {0, 1, 0}, {0, 0, -1} };

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
    mainWindow.MainLoop({0.0, 0.0, 0.0, 1.0});
    return 0;
}