SET(sources
	"main.cpp"
	# might move this into new subdirectory
	"mesh.hpp"
	"mesh.cpp"
	"shader.hpp"
	"shader.cpp"
)

# Add executable target and link libraries
add_executable(${CGRA_PROJECT} ${sources})

# add imgui backends
add_subdirectory(backends)