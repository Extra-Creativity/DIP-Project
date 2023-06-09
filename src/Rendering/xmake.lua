option("preview")
    set_default(false)
    set_showmenu(true)
    add_defines("EFFECT_PREVIEW")

target("GenerateNormalMapTest")
    add_options("preview")
    config_path = path.join(os.curdir(), "config", "GenNormConfig.ini"):gsub("\\", "/")
    shader_dir = path.join(os.curdir(), "shaders"):gsub("\\", "/")
    model_dir = path.join(os.curdir(), "data"):gsub("\\", "/")

    add_defines(
        "CONFIG_PATH=\"" .. config_path .. "\"",
        "SHADER_DIR=\"" .. shader_dir .. "\"", 
        "MODEL_DIR=\"" .. model_dir .. "\"")

    add_includedirs("../../Render/src")
    add_deps("OpenGLFramework")
    add_headerfiles("AssetLoader.h")
    add_files("AssetLoader.cpp", "GenerateNormalMapTest.cpp")

target("GenerateStereoData")
    add_options("preview")
    config_path = path.join(os.curdir(), "config", "GenStereoConfig.ini"):gsub("\\", "/")
    shader_dir = path.join(os.curdir(), "shaders"):gsub("\\", "/")
    model_dir = path.join(os.curdir(), "data"):gsub("\\", "/")

    add_defines(
        "CONFIG_PATH=\"" .. config_path .. "\"",
        "SHADER_DIR=\"" .. shader_dir .. "\"", 
        "MODEL_DIR=\"" .. model_dir .. "\"")

    add_includedirs("../../Render/src")
    add_deps("OpenGLFramework")
    add_headerfiles("AssetLoader.h")
    add_files("AssetLoader.cpp", "GenerateStereoData.cpp")

target("RestoreTest")
    config_path = path.join(os.curdir(), "config", "Restore.ini"):gsub("\\", "/")
    shader_dir = path.join(os.curdir(), "shaders"):gsub("\\", "/")
    model_dir = path.join(os.curdir(), "data"):gsub("\\", "/")

    add_defines(
        "CONFIG_PATH=\"" .. config_path .. "\"",
        "SHADER_DIR=\"" .. shader_dir .. "\"", 
        "MODEL_DIR=\"" .. model_dir .. "\"")

    add_includedirs("../../Render/src")
    add_deps("OpenGLFramework")
    add_files("RestoreTest.cpp")

target("RestoreFromVideo")
    config_path = path.join(os.curdir(), "config", "Restore.ini"):gsub("\\", "/")
    shader_dir = path.join(os.curdir(), "shaders"):gsub("\\", "/")
    model_dir = path.join(os.curdir(), "data"):gsub("\\", "/")

    add_defines(
        "CONFIG_PATH=\"" .. config_path .. "\"",
        "SHADER_DIR=\"" .. shader_dir .. "\"", 
        "MODEL_DIR=\"" .. model_dir .. "\"")

    add_includedirs("../../Render/src")
    add_deps("OpenGLFramework")
    add_files("RestoreFromVideo.cpp")