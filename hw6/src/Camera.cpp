#include "Camera.h"

Camera::Camera(){
	camPos = glm::vec3(0.0f, 0.0f, 20.0f);
	camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	camSpeed = 0.05f;
	pfov = 45.0f;
	pratio = 1.0f;
	pnear = 0.1f;
	pfar = 20.0f;
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 20.0f);;
	lookAt();
}


Camera::~Camera()
{
}


void Camera::moveForward(GLfloat const speed) {
	camPos += camFront * camSpeed;
	lookAt();
}

void Camera::moveBack(GLfloat const speed) {
	camPos -= camFront * camSpeed;
	lookAt();
}

void Camera::moveRight(GLfloat const speed) {
	camPos += camRight * camSpeed;
	lookAt();
}

void Camera::moveLeft(GLfloat const speed) {
	camPos -= camRight * camSpeed;
	lookAt();
}

void Camera::rotate(GLfloat const pitch, GLfloat const yaw) {
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camFront = glm::normalize(front);
	lookAt();
}

void Camera::perspective(GLfloat const pfov, GLfloat const pratio, GLfloat const pnear, GLfloat const pfar) {
	projection = glm::perspective(glm::radians(pfov), pratio, pnear, pfar);
}


void Camera::lookAt() {
	camRight = glm::normalize(glm::cross(camFront, worldUp));
	camUp = glm::normalize(glm::cross(camRight, camFront));
	view = glm::lookAt(camPos, camPos + camFront, camUp);
}

glm::mat4 Camera::getView() {
	return view;
}

glm::vec3 Camera::getPos() {
	return camPos;
}