#include "FrameworkCore/Core_All.h"
#include "Utility/Generator/Generator.h"
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

OpenGLFramework::Coroutine::Generator<int> GetViews(
    Core::Camera& camera, float totalRotationAngle, int totalTime)
{
    Core::Camera savedCamera = camera;
    int timeSegment = 3;
    co_yield 0;
    camera = savedCamera;

    for(int time = 0, maxTime = timeSegment * totalTime; time < maxTime; time++)
    {
        camera.RotateAroundCenter(totalRotationAngle / totalTime,
                                {0, camera.GetPosition().y, 0}, {0, 1, 0});
        if (time == totalTime)
        {
            camera = savedCamera;
            camera.RotateAroundCenter(30, {1, 0, 0}, {0, 1, 0});
            continue;
        }
        else if (time == 2 * totalTime)
        {
            camera = savedCamera;
            camera.RotateAroundCenter(-30, {1, 0, 0}, {0, 1, 0});
            continue;
        }
        co_yield 0;
    }
    co_return;
}

OpenGLFramework::Coroutine::Generator<int> SetNextCameraPos(
    Core::Camera& camera, float totalRotationAngle, int totalTime)
{
    auto gen1 = GetViews(camera, totalRotationAngle, totalTime);
    auto gen2 = GetViews(camera, -totalRotationAngle, totalTime);

    // To save camera inside.
    auto iter1 = gen1.begin(), iter2 = gen2.begin();
    co_yield 0;
    while(true)
    {
        iter1++;
        if(iter1 == gen1.end())
            break;
        co_yield 0;
    }
    while(true)
    {
        iter2++;
        if(iter2 == gen2.end())
            break;
        co_yield 0;
    }
    co_return;
}

OpenGLFramework::Coroutine::Generator<glm::vec3>
    GetIncident(Core::Camera& camera)
{
    glm::vec3 initIncident = -camera.GetGaze();
    glm::vec3 incidents[] = {
        initIncident, initIncident * glm::quat{{0, 45, 0}},
        initIncident * glm::quat{{0, -45, 0}},
        initIncident * glm::quat{{45, 0, 0}},
        initIncident * glm::quat{{-45, 0, 0}}
    };
    for(auto& incident: incidents)
        co_yield incident;
    co_return;
}

void PrintLightDir(std::ofstream& fout)
{
    Core::Camera camera{ { 0, 0, 0}, {0, 1, 0}, {0, 0, -1} };
    for(auto vec: GetIncident(camera))
    {
        fout << vec.x << " " << vec.y << " " << vec.z << "\n";
    }
    return;
}

void PrintViewMat(std::ofstream& fout, Core::Camera& camera)
{
    // Only output rotation matrix.
    auto mat = glm::mat3{camera.GetViewMatrix()};
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            fout << mat[i][j] << " ";
        }
    }
    return;
}

int main()
{
    std::filesystem::create_directories("GenStereoResult");
    [[maybe_unused]] auto& manager = Core::ContextManager::GetInstance();
    Asset::AssetLoader loader{ CONFIG_PATH, MODEL_DIR, SHADER_DIR };
    auto& mainWindow = loader.GetMainWindowInstance();

    auto& model = loader.GetModel("sucrose");
    model.transform.scale = { 0.125, 0.125, 0.125 };

    auto& shader = loader.GetShader("gen_stereo");
	Core::Camera camera{ { 0, 1.25, 3.5}, {0, 1, 0}, {0, 0, -1} };

    float near = 0.1f, far = 100.0f;

    auto generator = SetNextCameraPos(camera, 30, 10);
    auto iter = generator.begin();
    mainWindow.Register([&](){        
        if(iter == generator.end())
        {
            mainWindow.Close();
            return;
        }

        static int i = 1;
        static int j = 0;
        std::string dir = "GenStereoResult/" + std::to_string(i);
        std::filesystem::create_directories(dir);
#ifdef EFFECT_PREVIEW
        std::this_thread::sleep_for(500ms);
#else
        if(j != 0)
            mainWindow.SaveImage(dir + "/" + std::to_string(j) + ".jpg");
#endif

        static auto incidentGen = GetIncident(camera);
        static auto incidentIter = incidentGen.begin();
        if(incidentIter == incidentGen.end())
        {
            std::ofstream fout{ dir + "/light_dir.txt" };
            PrintLightDir(fout);
            fout.close();
            fout.open(dir + "/camera_dir.txt");
            PrintViewMat(fout, camera);

            iter++;
            incidentGen = GetIncident(camera);
            incidentIter = incidentGen.begin();
            i++;
            j = 0;
        }
        shader.Activate();
        shader.SetVec3("incidentDir", *incidentIter);

        j++;
        incidentIter++;
    });

    mainWindow.Register([&](){
        shader.SetMat4("model", model.transform.GetModelMatrix());
        shader.SetMat4("view", camera.GetViewMatrix());
        const auto[width, height] = mainWindow.GetWidthAndHeight();
        SetProjection(static_cast<float>(width), static_cast<float>(height), 
                      near, far, camera, shader);
        model.Draw(shader);
    });

    mainWindow.MainLoop({0.0, 0.0, 0.0, 0.0});
    return 0;
}