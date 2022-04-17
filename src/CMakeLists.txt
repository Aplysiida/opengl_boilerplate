SET(sources
	"main.cpp"
	# might move this into new subdirectory
	"camera.hpp"
	"camera.cpp"
	"mesh.hpp"
	"mesh.cpp"
	"scene.hpp"
	"shader.hpp"
	"shader.cpp"
)

# Add executable target and link libraries
add_executable(${CGRA_PROJECT} ${sources})

# add imgui backends
add_subdirectory(backends)