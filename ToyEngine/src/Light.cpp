#include "Light.h"
#include <iostream>

Light::Light()
	: m_Type(LightType::NONE), m_Color(glm::vec4(1)), m_Position(glm::vec3(0)), m_DebugName("Light") {}

Light::Light(const glm::vec3 pos, const std::string& name, const glm::vec4 color)
	: m_Position(pos), m_DebugName(name), m_Color(color)
{
}

Light::~Light()
{
}

PointLight::PointLight(const glm::vec3& position, const std::string& name, const Attenuation& method)
	: Light(position, name), m_Radius(1.0f), m_RadiusSquared(m_Radius * m_Radius), m_Attenuation(method)
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

