#include "FrameworkCore/Core_All.h"
#include "Utility/IO/IniFile.h"

using namespace OpenGLFramework;

int main()
{
    IOExtension::IniFile file{ CONFIG_PATH };
    [[maybe_unused]] auto& manager = Core::ContextManager::GetInstance();

    Core::MainWindow mainWindow{600, 1000, "test"};
    
    Core::Texture texture{ 
        std::filesystem::path{ MODEL_DIR } / file.rootSection("test_texture")};

    Core::SkyBoxTexture skybox{ 
        std::filesystem::path{ MODEL_DIR } / file.rootSection("skybox"),
        Core::SkyBoxTexture::TextureSegmentType::HorizontalLeft };
    Core::BasicTriRenderModel model{ 
        std::filesystem::path{ MODEL_DIR } / file.rootSection("plane") };
    
    std::filesystem::path shaderPath{ SHADER_DIR };
    Core::Shader shader{ shaderPath / file.rootSection("vertex_shader"),
                         shaderPath / file.rootSection("fragment_shader") };

    mainWindow.Register([&](){
        shader.Activate();
        
        shader.SetVec3("incident", { 0.0f, -1.0f, 0.0f });
        model.Draw(shader, [&](int textureBeginID, Core::Shader& shader){
            glActiveTexture(GL_TEXTURE0 + textureBeginID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.GetID());
            shader.SetInt("skybox", textureBeginID);

            textureBeginID++;
            glActiveTexture(GL_TEXTURE0 + textureBeginID);
            glBindTexture(GL_TEXTURE_2D, texture.ID);
            shader.SetInt("normalTexture", textureBeginID);
        }, nullptr);
    });

    mainWindow.MainLoop({0.0f, 0.0f, 0.0f, 1.0f});
    return 0;
}