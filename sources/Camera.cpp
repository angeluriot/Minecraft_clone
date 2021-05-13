#include "Camera.h"

// Cr�e une cam�ra � partir d'une autre

Camera::Camera(const Camera& other)
{
	*this = other;
}

// Cr�e une cam�ra � partir de ces propri�t�s

Camera::Camera(const glm::vec3& position, float sensitivity, float fov)
{
	this->position = position;
	this->sensitivity = sensitivity;
	this->fov = fov;

	direction = glm::vec3(0.f, 0.f, -1.f);

	projection = glm::perspective(fov, static_cast<float>(GameWindow::size.x) / static_cast<float>(GameWindow::size.y), 0.1f, 100.f);
	view = glm::lookAt(this->position, this->position + direction, glm::vec3(0.f, 1.f, 0.f));

	yaw = -90.f;
	pitch = 0.f;
}

// Op�rateur �gal

Camera& Camera::operator=(const Camera& other)
{
	position = other.position;
	sensitivity = other.sensitivity;
	fov = other.fov;
	view = other.view;
	projection = other.projection;
	direction = other.direction;
	yaw = other.yaw;
	pitch = other.pitch;

	return *this;
}

// Changement de la direction de la cam�ra � partir des mouvements de la souris

void Camera::look(const glm::ivec2& mouse_pos)
{
	yaw += (mouse_pos.x - GameWindow::center.x) * sensitivity;
	pitch -= (mouse_pos.y - GameWindow::center.y) * sensitivity;

	if (pitch > 89.f)
		pitch = 89.f;

	if (pitch < -89.f)
		pitch = -89.f;

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);

	view = glm::lookAt(position, position + direction, glm::vec3(0.f, 1.f, 0.f));
}

// Donne la matrice de la cam�ra

glm::mat4 Camera::get_matrix() const
{
	// glm::perspective bug et ne prend pas en compte les param�tres "near" et "far" donc je le fais artificiellement ici
	return glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 0.1)) * projection * view;
}

// Change la position de la cam�ra

void Camera::set_position(const glm::vec3& position)
{
	this->position = position;
	view = glm::lookAt(this->position, this->position + direction, glm::vec3(0.f, 1.f, 0.f));
}

// Donne la position de la cam�ra

glm::vec3 Camera::get_position() const
{
	return position;
}

// Donne l'angle horizontal de la cam�ra

float Camera::get_yaw() const
{
	return yaw;
}

// Donne l'angle vertical de la cam�ra

float Camera::get_pitch() const
{
	return pitch;
}

// Donne la direction de la cam�ra

glm::vec3 Camera::get_direction() const
{
	return direction;
}

// Inverse la cam�ra en fonction de la hauteur d'un miroir hozitontal

void Camera::invert(float mirror_level)
{
	position.y = position.y - 2.f * (position.y - mirror_level);
	pitch = -pitch;

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);

	view = glm::lookAt(position, position + direction, glm::vec3(0.f, 1.f, 0.f));
}

// Change la r�solution de l'image donn�e par la cam�ra

void Camera::change_resolution(float width, float height)
{
	projection = glm::perspective(fov, width / height, 0.1f, 100.f);
}
