#pragma once

//glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

/*
* Virtual base class for camera, contains all variables and functions that all cameras need
*/
class Camera {
protected:
	const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f); //TODO: maybe change this from constant at some point?
	float yaw = 0, pitch = 0;

	glm::vec3 cameraUp = WORLD_UP;	//up direction of camera
	glm::vec3 cameraRight;
	glm::vec3 cameraPos = glm::vec3(0, 0, 0);

	void calcAngles(glm::vec2 newPos, glm::vec2 oldPos);
public:
	enum direction { Forward, Backward, Right, Left };	//for camera direction
	float sensitivity = 0.1f;	//how fast does camera move around by mouse movements
	float fov = 1.0f;

	//virtual functions to implement
	virtual void processMouseCamera(glm::vec2 newPos, glm::vec2 oldPos) =0;
	virtual const glm::mat4& getViewMat() =0;
	virtual void resetPos() =0;
};

/*
* Orbits around target position
*/
class OrbitalCamera : public Camera {
private:
public:
	float distance = 1.0f;

	void processMouseCamera(glm::vec2 newPos, glm::vec2 oldPos);
	const glm::mat4& getViewMat();

	void resetPos() { distance = 1.0f; fov = 1.0f; }
};

/*
* User can move camera around
*/
class MoveAroundCamera : public Camera {
private:
	glm::vec3 cameraFront;

	void updateCamera(glm::vec3 dir);	//update camera based on new front direction
public:
	float cameraSpeed = 1.0f;	//how fast does the camera move around
	//process mouse movement here
	void processMouseCamera(glm::vec2 newPos, glm::vec2 oldPos);
	void moveCamera(MoveAroundCamera::direction d);
	const glm::mat4& getViewMat();
	void resetPos() { cameraPos = glm::vec3(0, 0, 0); fov = 1.0f; }
};