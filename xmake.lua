set_project("DIP-Project")
set_version("1.1")
set_xmakever("2.6.1")

add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
set_languages("cxx20")
add_cxxflags("-Wall")


includes("Render", "src/Rendering")

add_requires("glfw", "glad", "glm", "assimp", "stb")

add_packages("glfw", "glad", "glm", "assimp", "stb")