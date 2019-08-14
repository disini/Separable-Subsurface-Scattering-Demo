#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace sss
{
	class ArcBallCamera
	{
	public:
		explicit ArcBallCamera(const glm::vec3 &center, float distance);
		glm::mat4 update(const glm::vec2 &mouseDelta, float scrollDelta);

	private:
		glm::vec3 m_center;
		float m_distance;
		float m_theta;
		float m_phi;
	};
}