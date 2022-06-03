cmake_minimum_required(VERSION 3.19)
project(linkingtest LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX "Debug")
set(CMAKE_RELEASE_POSTFIX "Release")

add_executable(linkingtest src/main.cpp src/NativeWindowHack.mm)

#  -------- Dependencies ------------

add_compile_definitions(
        BX_CONFIG_DEBUG=1
        BGFX_CONFIG_RENDERER_METAL)

# add_compile_options("-ObjC++")

# spdlog
target_include_directories(linkingtest PRIVATE thirdparty/spdlog/include)
target_link_directories(linkingtest PRIVATE thirdparty/spdlog/build/)
target_link_libraries(linkingtest PRIVATE spdlog)

# glfw
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(linkingtest PRIVATE glfw)

# glm
find_package(glm)
target_link_libraries(linkingtest PRIVATE glm)

#imgui

    # add glfw dep to imgui
    set(IMGUI_SOURCE thirdparty/imgui/backends/imgui_impl_glfw.cpp thirdparty/imgui/imgui.cpp thirdparty/imgui/imgui_draw.cpp thirdparty/imgui/imgui_demo.cpp thirdparty/imgui/imgui_tables.cpp thirdparty/imgui/imgui_widgets.cpp)
    add_library(imgui STATIC ${IMGUI_SOURCE})

    target_link_libraries(imgui PRIVATE 
        glfw
        glm)    # for conversion from IMVec2 to glm::vec2

target_link_libraries(linkingtest PRIVATE imgui)
include_directories(thirdparty/imgui)

target_link_libraries(linkingtest PRIVATE "-framework Metal -framework MetalKit -framework Cocoa -framework IOKit -framework CoreVideo -framework QuartzCore")

#bgfx
target_include_directories(linkingtest
        PRIVATE thirdparty/bgfx/include
            thirdparty/bx/include
            thirdparty/bimg/include)

target_link_directories(linkingtest PRIVATE thirdparty/bgfx/.build/osx-x64/bin/)

target_link_libraries(linkingtest PRIVATE
        bgfxDebug
        bxDebug
        bimgDebug

        # from bgfx.lua
        "-framework Cocoa"
        "-framework IOKit"
        "-framework QuartzCore"
        "-weak_framework Metal"
        "-weak_framework MetalKit"

#        "-framework CoreVideo"  # for the metal layer hack
        )

# This part is just the relevant bits of the Makefile of bgfx.

# set(GENIE thirdparty/bx/tools/bin/darwin/genie)

# add_custom_command(OUTPUT thirdparty/bgfx/.build/projects/gmake-osx-x64/Makefile
#         COMMAND $(GENIE) --with-shared-lib --gcc=osx-x64 gmake
#         # all options: $(GENIE) --with-tools --with-combined-examples --with-shared-lib --gcc=osx-x64 gmake
#         WORKING_DIRECTORY thirdparty/bgfx/)

# add_custom_command(OUTPUT bgfxDebug
#         MAIN_DEPENDENCY thirdparty/bgfx/.build/projects/gmake-osx-x64/Makefile
#         COMMAND make -C .build/projects/gmake-osx-x64 config=debug
#         WORKING_DIRECTORY thirdparty/bgfx/)

# new target: test

add_library(BIGGEngine SHARED
        src/ContextI.cpp
        src/ContextImplGLFW.cpp
        src/Renderer.cpp
        src/NativeWindowHack.mm
        src/Script.cpp
        src/Script.hpp)

target_include_directories(BIGGEngine PRIVATE
        thirdparty/spdlog/include 
        thirdparty/bgfx/include
        thirdparty/bx/include
        thirdparty/bimg/include
        thirdparty/glfw/include
        thirdparty/glm
        thirdparty/lua-5.4.4/include/
        thirdparty/entt/src/)

target_link_libraries(BIGGEngine PRIVATE
        bxDebug
        bgfxDebug
        bimgDebug
        glfw
        glm
        spdlog
        lua
        "-framework Cocoa"
        "-framework IOKit"
        "-framework QuartzCore"
        "-weak_framework Metal"
        "-weak_framework MetalKit"
        "-framework CoreVideo")  # for the metal layer hack

target_link_directories(BIGGEngine PRIVATE
        thirdparty/bgfx/.build/osx-x64/bin/
        thirdparty/spdlog/build/
        thirdparty/lua-5.4.4/bin/)

target_precompile_headers(BIGGEngine PRIVATE src/Core.hpp)

add_executable(test test/test.cpp)
target_link_libraries(test PRIVATE
        BIGGEngine
        imgui)

target_include_directories(test PRIVATE 
        thirdparty/spdlog/include 
        thirdparty/bgfx/include
        thirdparty/bx/include
        thirdparty/bimg/include
        thirdparty/glfw/include
        thirdparty/glm
        thirdparty/imgui
        thirdparty/lua-5.4.4/include/
        thirdparty/entt/src/)

target_link_directories(test PRIVATE 
        thirdparty/bgfx/.build/osx-x64/bin/
        thirdparty/spdlog/build/
        thirdparty/lua-5.4.4/bin/)

target_precompile_headers(test REUSE_FROM BIGGEngine)
      

add_custom_target(cleanlogs rm log/*
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})