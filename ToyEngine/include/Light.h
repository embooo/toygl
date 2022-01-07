#pragma once

#include <string>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class LightType   { NONE, POINT_LIGHT };
enum class Attenuation { NONE, INV_SQUARE, NO_SINGULARITY };

// Light attenuation methods
// Inverse square : 1/distance^2
// NO_SINGULARITY : fixes the issue of inverse square attenuation tending to infinity when distance = 0
// http://www.cemyuksel.com/research/pointlightattenuation/pointlightattenuation.pdf

class Light
{
public:
	static unsigned int Count;

	Light();
	Light(const glm::vec3 position, const glm::vec4 color, const std::string& name);

	virtual glm::vec3& pos() { return m_Position; }
	virtual const glm::vec3& pos() const { return m_Position; }

	virtual glm::vec4& color() { return m_Color; }
	virtual const glm::vec4& color() const { return m_Color; }

	LightType& type() { return m_Type;  }

	virtual ~Light();

protected:
	LightType   m_Type;
	std::string m_DebugName;
	glm::vec4   m_Color;
	glm::vec3   m_Position;
};

class PointLight : public Light
{
public:
	PointLight(const glm::vec3& position = glm::vec3(0), 
			   const glm::vec4& color = glm::vec4(0,0,0,1),
			   const std::string& name = "PointLight_" + std::to_string(s_Count), 
			   const Attenuation& method = Attenuation::INV_SQUARE, 
			   const LightType& type = LightType::POINT_LIGHT);

	float& radius() { return m_Radius; }
	const float& radius() const { return m_Radius; }

	void Translate(const glm::vec3& tvec);
	void UpdateRadius(float radius);
private:
	float m_Radius;
	float m_RadiusSquared;
	Attenuation m_Attenuation;
	inline static unsigned int s_Count;
};
