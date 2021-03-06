#include "scene/Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

Transform::Transform()
	: location()
	, rotation()
	, scale(1.0f)
	, matrix(1.0f)
	, isDirty(true)
{
}

Transform::~Transform()
{
}

void Transform::SetLocation(const glm::vec3 & inLocation)
{
	this->location = inLocation;
	MarkDirty();
}

void Transform::AddLocation(const glm::vec3 & inLocation)
{
	location += inLocation;
	MarkDirty();
}

void Transform::SetRotation(const glm::vec3 & inRotation)
{
	this->rotation = inRotation;
	MarkDirty();
}

void Transform::AddRotation(const glm::vec3 & inRotation)
{
	rotation += inRotation;
	MarkDirty();
}

void Transform::SetScale(const glm::vec3 & inScale)
{
	this->scale = inScale;
	MarkDirty();
}

void Transform::AddScale(const glm::vec3 & inScale)
{
	scale += inScale;
	MarkDirty();
}

void Transform::MarkDirty()
{
	isDirty = true;
}

glm::mat4 Transform::GetMatrix() const
{
	if (isDirty)
	{
		return CalculateMatrix();
	}
	return matrix;
}

glm::mat4& Transform::GetMatrix()
{
	if (isDirty)
	{
		matrix = CalculateMatrix();
		isDirty = false;
	}
	return matrix;
}

glm::mat4 Transform::CalculateRotationMatrix() const
{
	glm::mat4 mat(1.0f);
	mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0, 1.0f));
	mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0, 0.0f));
	mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0, 0.0f));

	return mat;
}

glm::mat4 Transform::CalculateViewMatrix() const
{
	glm::vec3 direction = GetForwardVector();
	return glm::lookAt(location, location + direction, GetUpVector());
}

glm::mat4 Transform::CalculateMatrix() const
{
	glm::mat4 mat(1.0f);
	mat = glm::translate(mat, location);
	mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0, 1.0f));
	mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0, 0.0f));
	mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0, 0.0f));
	mat = glm::scale(mat, scale);

	return mat;
}

glm::vec3 Transform::GetForwardVector() const
{
	glm::vec4 forward = { 0.0f, 0.0f, 1.0f, 0.0f };
	forward = CalculateRotationMatrix() * forward;
	return glm::vec3(forward);
}

glm::vec3 Transform::GetUpVector() const
{
	glm::vec4 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	up = CalculateRotationMatrix() * up;
	return glm::vec3(up);
}

glm::vec3 Transform::GetLeftVector() const
{
	glm::vec4 right = { 1.0f, 0.0f, 0.0f, 0.0f };
	right = CalculateRotationMatrix() * right;
	return glm::vec3(right);
}


