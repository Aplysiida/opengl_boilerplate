#pragma once

//glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//TODO: add virtual camera class and orbital camera

class MoveAroundCamera {
private:
	const glm::vec3 WORLD_UP = glm::vec3(0.0f,1.0f,0.0f); //TODO: maybe change this from constant at some point?
	float yaw = 0, pitch = 0; //add these var to virtual camera class

	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;
	glm::vec3 cameraPos = glm::vec3(0, 0, 0); //will allow movement later

	void updateCamera(glm::vec3 dir) {
		cameraFront = glm::normalize(dir);
		cameraRight = glm::normalize(glm::cross(WORLD_UP, cameraFront));
		cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));
	}
public:
	float sensitivity;	//how fast does camera move around by mouse movements

	MoveAroundCamera() {}

	//process mouse movement here
	void processMouseCamera(float xOffset, float yOffset) {
		//add offset to yaw and pitch
		yaw += glm::radians(xOffset);
		pitch += glm::radians(yOffset);
		//clamp pitch and yaw to -pi/2 to pi/2 
		yaw = glm::clamp(yaw, -glm::pi<float>() / 2, glm::pi<float>() / 2);
		pitch = glm::clamp(pitch, -glm::pi<float>() / 2, glm::pi<float>() / 2);
		glm::vec3 direction;	//new direction that camera is pointing to
		direction.x = glm::cos(yaw);
		direction.y = glm::sin(pitch);
		direction.z = glm::sin(yaw);
		
		updateCamera(direction);
	}

	void processScrollCamera() {
		;
	}

	const glm::mat4& getViewMat() {
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraRight);
	}
};