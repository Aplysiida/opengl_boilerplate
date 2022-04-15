#include <iostream>
//glm
#include <glm/gtc/matrix_transform.hpp>
//project
#include "camera.hpp"

//-- base camera functions --
void Camera::calcAngles(glm::vec2 newPos, glm::vec2 oldPos) {
	//calculate mouse movement offset
	float xOffset = newPos.x - oldPos.x;
	float yOffset = newPos.y - oldPos.y;
	//add sensitivity
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	//add offset to yaw and pitch
	yaw += glm::radians(xOffset);
	pitch += glm::radians(yOffset);
}

//-- orbital camera functions --
const glm::mat4& OrbitalCamera::getViewMat() {
	float d = distance * 100;	//scale distance up

	float x = cameraPos.x + distance * glm::cos(yaw) * glm::cos(pitch);
	float y = cameraPos.y + distance * glm::sin(pitch);
	float z = cameraPos.z + distance * glm::sin(yaw) * glm::cos(pitch);

	glm::mat4 view = glm::lookAt(glm::vec3(x,y,z), glm::vec3(0,0,0), cameraUp);

	return view;
}

void OrbitalCamera::processMouseCamera(glm::vec2 newPos, glm::vec2 oldPos) {
	calcAngles(newPos, oldPos);
}

//-- movearound camera functions --

const glm::mat4& MoveAroundCamera::getViewMat() {
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void MoveAroundCamera::updateCamera(glm::vec3 dir) {
	cameraFront = glm::normalize(dir);
	cameraRight = glm::normalize(glm::cross(WORLD_UP, cameraFront));
	cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));
}

void MoveAroundCamera::processMouseCamera(glm::vec2 newPos, glm::vec2 oldPos) {
	calcAngles(newPos, oldPos);

	glm::vec3 direction;	//new direction that camera is pointing to
	direction.x = glm::cos(yaw) * glm::cos(pitch);
	direction.y = glm::sin(pitch);
	direction.z = glm::sin(yaw) * glm::cos(pitch);

	updateCamera(direction);
}

void MoveAroundCamera::moveCamera(MoveAroundCamera::direction d) {
	float speed = cameraSpeed / 3;	//scale speed down to allow more control
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