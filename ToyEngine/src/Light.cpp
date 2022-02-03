#include "PCH.h"

#include "Light.h"
#include <iostream>

Light::Light()
	: m_Type(LightType::NONE), m_Color(glm::vec4(1)), m_Position(glm::vec3(0)), m_DebugName("Light") {}

Light::Light(const glm::vec3 pos, const glm::vec4 color, const std::string& name, const LightType& type)
	: m_Position(pos), m_Color(color), m_DebugName(name), m_Type(type)
{
}

Light::Light(const glm::vec4 color, const std::string& name, const LightType& type)
	: m_Color(color), m_DebugName(name), m_Type(type)
{
}

Light::~Light()
{
}

PointLight::PointLight(const glm::vec3& position,
	const glm::vec4& color,
	const std::string& name,
	const Attenuation& method,
	const LightType& type)
	: Light(position, color, name, type), m_Radius(1.0f), m_RadiusSquared(m_Radius* m_Radius), m_Attenuation(method)
{
	s_Count = 0;
	std::cout << "Created " << m_DebugName << std::endl;
}

void PointLight::Translate(const glm::vec3& tvec)
{
	m_Position = glm::translate(glm::identity<glm::mat4>(), tvec) * glm::vec4(m_Position, 1.0f);
}

void PointLight::UpdateRadius(float radius)
{
	m_Radius		= std::max(0.0f, radius);
	m_RadiusSquared = m_Radius * m_Radius;
}

DirectionalLight::DirectionalLight(const glm::vec3& dir, const glm::vec4& color, const std::string& name, const LightType& type)
	: m_Direction(dir), Light(color, name, type), m_Model(glm::identity<glm::mat4>())
{
	s_Count = 0;
	std::cout << "Created " << m_DebugName << std::endl;
}

void DirectionalLight::Rotate(float angle, const glm::vec3& axis)
{
	m_Model = glm::rotate(m_Model, angle, axis);
}

glm::vec3& DirectionalLight::dir()
{
	return m_Direction;
}
