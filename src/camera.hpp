#pragma once

//glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//TODO: add virtual camera class and orbital camera
class Camera {
protected:
	const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f); //TODO: maybe change this from constant at some point?
	float yaw = 0, pitch = 0;
	float fov = 1.0f;

	glm::vec3 cameraFront;
	glm::vec3 cameraUp = WORLD_UP;
	glm::vec3 cameraRight;
	glm::vec3 cameraPos = glm::vec3(0, 0, 0);
public:
	enum direction { Forward, Backward, Right, Left };	//for camera direction
	float sensitivity = 0.1f;	//how fast does camera move around by mouse movements

	virtual void processMouseCamera(float xOffset, float yOffset) {}

	const glm::mat4& getViewMat() {
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	void resetPos() { cameraPos = glm::vec3(0, 0, 0); }
};

/*class OrbitalCamera : public Camera {
private:
public:
};*/


class MoveAroundCamera : public Camera {
private:
	void updateCamera(glm::vec3 dir) {
		cameraFront = glm::normalize(dir);	
		cameraRight = glm::normalize(glm::cross(WORLD_UP, cameraFront));
		cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));
	}
public:
	float cameraSpeed = 1.0f;	//how fast does the camera move around

	//process mouse movement here
	void processMouseCamera(float xOffset, float yOffset) {
		//add sensitivity
		xOffset *= sensitivity;
		yOffset *= sensitivity;
		//add offset to yaw and pitch
		yaw += glm::radians(xOffset);
		pitch += glm::radians(yOffset);
		//clamp pitch and yaw to -pi/2 to pi/2 
		//yaw = glm::clamp(yaw, -glm::pi<float>() / 2, glm::pi<float>() / 2);
		//pitch = glm::clamp(pitch, -glm::pi<float>() / 2, glm::pi<float>() / 2);
		glm::vec3 direction;	//new direction that camera is pointing to
		direction.x = glm::cos(yaw) * glm::cos(pitch);
		direction.y = glm::sin(pitch);
		direction.z = glm::sin(yaw) * glm::cos(pitch);
		
		updateCamera(direction);
	}

	void processScrollCamera() {
		;
	}

	void moveCamera(MoveAroundCamera::direction d) {
		float speed = cameraSpeed/3;	//scale speed down to allow more control
		switch (d) {
		case Backward:
			cameraPos += speed * -cameraFront;
			break;
		case Left:
			cameraPos += -speed * glm::normalize(glm::cross(cameraFront, cameraUp));
			break;
		case Right:
			cameraPos += speed * glm::normalize(glm::cross(cameraFront, cameraUp));
		default:	//forward
			cameraPos += speed * cameraFront;
			break;
		}
	}
};