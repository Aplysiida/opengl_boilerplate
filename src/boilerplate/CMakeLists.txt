#Source files
set(sources
	"camera.hpp"
	"camera.cpp"
	"mesh.hpp"
	"mesh.cpp"
	"scene.hpp"
	"shader.hpp"
	"shader.cpp"
)

# Add these sources to the project target
target_relative_sources(${CGRA_PROJECT} ${sources})