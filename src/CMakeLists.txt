SET(sources
	"main.cpp"
)

# Add executable target and link libraries
add_executable(${CGRA_PROJECT} ${sources})

# add imgui backends
add_subdirectory(backends)