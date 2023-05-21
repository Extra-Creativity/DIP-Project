#include "FrameworkCore/Core_All.h"
#include "Utility/IO/IniFile.h"

using namespace OpenGLFramework;

int main()
{
    IOExtension::IniFile file{ CONFIG_PATH };
    [[maybe_unused]] auto& manager = Core::ContextManager::GetInstance();

    Core::MainWindow mainWindow{1920, 1080, "test"};
    // Core::Framebuffer buffer{1920, 1080};
    // glBindRenderbuffer(GL_RENDERBUFFER, buffer.GetFramebuffer());

    
    Core::Texture texture{R"(D:\111\University\Course\CS\Digitial Image)"
                          R"( Processing\project\llff-data\data1\images\1.png)"};
    // int textureWidth = 1920, textureHeight = 1080;
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

    // model.Draw(shader, buffer);
    
    // const int width = buffer.GetWidth(), height = buffer.GetHeight(), channelNum = 3;

    // std::vector<unsigned char> pixelBuffer(width * height * channelNum);
    // unsigned char* pixelBufferRawPtr = pixelBuffer.data();

    // int initialAlignment, initalBuffer;
    // glGetIntegerv(GL_PACK_ALIGNMENT, &initialAlignment);
    // glGetIntegerv(GL_READ_BUFFER, &initalBuffer);

    // glPixelStorei(GL_PACK_ALIGNMENT, 1);
    // glReadBuffer(GL_FRONT);
    // glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE,
    //     pixelBufferRawPtr);
    // glReadBuffer(initalBuffer); // restore settings.
    // glPixelStorei(GL_PACK_ALIGNMENT, initialAlignment);

    return 0;
}