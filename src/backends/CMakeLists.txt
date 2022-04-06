#Source files
set(sources
    "imgui_impl_glfw.cpp"
    "imgui_impl_glfw.h"

    "imgui_impl_opengl3.cpp"
    "imgui_impl_opengl3.h"

    "imgui_impl_opengl3_loader.h"
)

# Add these sources to the project target
target_relative_sources(${CGRA_PROJECT} ${sources})