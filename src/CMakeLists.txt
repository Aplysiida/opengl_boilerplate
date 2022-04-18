SET(sources
	"main.cpp"
	"meshfairing.hpp"
	"meshfairing.cpp"
)

# Add boilerplate code
add_subdirectory(boilerplate)

# Add executable target and link libraries
add_executable(${CGRA_PROJECT} ${sources})

# add imgui backends
add_subdirectory(backends)