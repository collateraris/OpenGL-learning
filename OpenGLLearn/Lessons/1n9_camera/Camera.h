#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace lesson_1n9
{
	class CCamera
	{
	public:

		static CCamera& Get();

		void KeyProcessing(int key, int action);

		void MouseProcessing(float xpos, float ypos);

		void ScrollProcessing(float xoffset, float yoffset);

		void Movement(float deltaTime);

		glm::mat4 GetView();

		float GetFov();

		const glm::vec3& GetCameraPosition();

		void SetCameraPosition(const glm::vec3& pos);

		const glm::vec3& GetCameraFront();

	private:

		const bool bPRESS = true;
		const bool bRELEASE = false;

		bool keys[1024] = { bRELEASE };

		CCamera() {};

		CCamera(const CCamera& copy) = delete;
		CCamera operator=(const CCamera& copy) = delete;

		glm::vec3 mCameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		float fov = 45.0f;
	};
}