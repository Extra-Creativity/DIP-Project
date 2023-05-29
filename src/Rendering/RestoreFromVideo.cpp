#include "FrameworkCore/Core_All.h"
#include "Utility/IO/IniFile.h"
#include "stb_image_write.h"

#include <iostream>
#include <fstream>
using namespace OpenGLFramework;

std::string GetBufferPicPath()
{
    std::string bufferPath;
    std::getline(std::cin, bufferPath);
    std::cout << "Done." << std::endl;
    return bufferPath;
}

std::pair<unsigned int, unsigned int> GetWidthAndHeight()
{
    unsigned int width, height;
    std::cin >> width >> height;
    std::cout << "Done." << std::endl;
    return {width, height};
}

void TestCommunication()
{
    std::ofstream log{"log.txt"};

    std::string bufferPath = GetBufferPicPath();
    log << bufferPath << std::endl;

    const auto [width, height] = GetWidthAndHeight();
    log << width << " " << height << std::endl;

    glm::mat4 poseMat = glm::identity<glm::mat4>();
    while(true)
    {
        if(!std::cin.good()){
            break;
        }
        std::cin >> poseMat[0][0] >> poseMat[0][1] >> poseMat[0][2]
            >> poseMat[1][0] >> poseMat[1][1] >> poseMat[1][2]
            >> poseMat[2][0] >> poseMat[2][1] >> poseMat[2][2];
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                log << poseMat[i][j] << " ";
        log << std::endl;

        std::cout << "Done." << std::endl;
    }
    return;
}

int main(int argc, char* argv[])
{
    if(argc != 1)
    {
        TestCommunication();
        return 0;
    }

    IOExtension::IniFile file{ CONFIG_PATH };
    [[maybe_unused]] auto& manager = Core::ContextManager::GetInstance();
    Core::MainWindow mainWindow{ 100, 100, "test", false };

    std::string bufferPath = GetBufferPicPath();
    const auto[width, height] = GetWidthAndHeight();
    Core::Framebuffer buffer{ width, height };
    
    Core::SkyBoxTexture skybox = file.rootSection("option") == "split" ? 
        Core::SkyBoxTexture { 
            std::filesystem::path{ MODEL_DIR } / file.rootSection("skybox_split")
        } : Core::SkyBoxTexture{ 
            std::filesystem::path{ MODEL_DIR } / file.rootSection("skybox_total"),
            Core::SkyBoxTexture::TextureSegmentType::HorizontalLeft
        };
    
    Core::BasicTriRenderModel model{ 
        std::filesystem::path{ MODEL_DIR } / file.rootSection("plane") };
    
    std::filesystem::path shaderPath{ SHADER_DIR };
    Core::Shader shader{ shaderPath / file.rootSection("vertex_shader"),
                         shaderPath / file.rootSection("fragment_shader") };
    stbi_flip_vertically_on_write(true);

    std::vector<unsigned char> pixelBuffer;

    mainWindow.Register([&](){
        glm::mat4 poseMat = glm::identity<glm::mat4>();
        std::cin >> poseMat[0][0] >> poseMat[0][1] >> poseMat[0][2]
                >> poseMat[1][0] >> poseMat[1][1] >> poseMat[1][2]
                >> poseMat[2][0] >> poseMat[2][1] >> poseMat[2][2];
        if(!std::cin.good()){
            mainWindow.Close();
            return;
        }
        Core::Camera camera{ {0, 0, 0}, {0, 1, 0}, {0, 0, -1}};
        camera.Rotate(glm::inverse(glm::toQuat(poseMat)));
        glm::vec3 incident = camera.GetGaze();

        Core::Texture texture{ bufferPath };
        shader.Activate();
        
        shader.SetVec3("incident", {0, -1, 0});
        glViewport(0, 0, width, height);
        model.Draw(shader, buffer, 
        [&](int textureBeginID, Core::Shader& shader){
            glActiveTexture(GL_TEXTURE0 + textureBeginID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.GetID());
            shader.SetInt("skybox", textureBeginID);

            textureBeginID++;
            glActiveTexture(GL_TEXTURE0 + textureBeginID);
            glBindTexture(GL_TEXTURE_2D, texture.ID);
            shader.SetInt("normalTexture", textureBeginID);
        }, nullptr);

        const int channelNum = 3;
        pixelBuffer = Core::Framebuffer::SaveBufferInCPU(buffer.GetFramebuffer(),
            width, height, channelNum);
        stbi_write_jpg(bufferPath.c_str(), width, height, channelNum, 
            pixelBuffer.data(), 95);
    });

    mainWindow.Register([&](){
        // To make CPUTexture free the path so that the permission is right.
        std::cout << "Done." << std::endl;
    });

    mainWindow.MainLoop({0.0f, 0.0f, 0.0f, 1.0f});
    return 0;
}