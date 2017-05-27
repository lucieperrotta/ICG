get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# these files are copied only once
file(GLOB TEXTURES
    sky/fog_sky.tga
    textures/sand_texture.tga
    textures/grass_texture.tga
    textures/rock_texture.tga
    textures/snow_texture.tga
    )
copy_files_once(${TEXTURES})

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
    sky/*.glsl
    cube/*.glsl
    water/*.glsl
    #quad/*.glsl
    noise/*.glsl
    displaytexture/*.glsl
    grid/*.glsl
        )
deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
