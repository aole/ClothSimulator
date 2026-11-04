cmake_minimum_required(VERSION 3.21)
project(ClothSimulator LANGUAGES CXX)

find_package(wxWidgets CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(ClothSimulator WIN32
    ClothSimulator/ClothMesh.cpp
    ClothSimulator/Controller.cpp
    ClothSimulator/cs2DPanel.cpp
    ClothSimulator/OpenGLContext.cpp
    ClothSimulator/csApplication.cpp
    ClothSimulator/csGL3DGrid.cpp
    ClothSimulator/csGL3DObject.cpp
    ClothSimulator/csGLPanel.cpp
    ClothSimulator/csMainFrame.cpp
    ClothSimulator/Model.cpp
    ClothSimulator/World.cpp
)
target_compile_features(ClothSimulator PRIVATE cxx_std_17)
target_compile_definitions(ClothSimulator PRIVATE
    _CRT_SECURE_NO_WARNINGS GLM_ENABLE_EXPERIMENTAL)
target_include_directories(ClothSimulator PRIVATE ClothSimulator)
target_link_libraries(ClothSimulator PRIVATE
    wx::core wx::base wx::gl GLEW::GLEW glm::glm OpenGL::GL)
set_property(TARGET ClothSimulator PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
    "$<TARGET_FILE_DIR:ClothSimulator>")

# The application reads these files relative to its working directory.
add_custom_command(TARGET ClothSimulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/LitFS.glsl"
        "${CMAKE_CURRENT_SOURCE_DIR}/LitVS.glsl"
        "${CMAKE_CURRENT_SOURCE_DIR}/UnLitFS.glsl"
        "${CMAKE_CURRENT_SOURCE_DIR}/UnLitVS.glsl"
        "${CMAKE_CURRENT_SOURCE_DIR}/pref.txt"
        "${CMAKE_CURRENT_SOURCE_DIR}/SewPattern.jpg"
        "${CMAKE_CURRENT_SOURCE_DIR}/test.txt"
        "$<TARGET_FILE_DIR:ClothSimulator>"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Objects"
        "$<TARGET_FILE_DIR:ClothSimulator>/Objects"
    VERBATIM
)
