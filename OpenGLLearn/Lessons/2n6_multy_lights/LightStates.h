#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>

#include "../1n5_shaders/Shader.h"

namespace lesson_2n6
{
	enum EState
	{
		COMMON,
		DESERT,
		FACTORY,
		HORROR,
		BIOCHEMICAL_LAB,
	};

	class CLightStates;

	class ILightState
	{
	protected:

		friend class CLightStates;

		virtual const std::vector<glm::vec3>& GetPointLightColors() = 0;

		virtual void SetShaderParams(lesson_1n5::CShader& shader) = 0;

		virtual void  SetClearColorParam() = 0;
	};

	class CCommonLightState : public ILightState
	{
	public:
		CCommonLightState() = default;
	protected:

		virtual const std::vector<glm::vec3>& GetPointLightColors() override;

		virtual void SetShaderParams(lesson_1n5::CShader&) override;

		virtual void SetClearColorParam() override;

	private:
		const std::vector<glm::vec3> pointLightPositions = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
		};
	};

	class CDesertLightState : public ILightState
	{
	public:
		CDesertLightState() = default;
	protected:

		virtual const std::vector<glm::vec3>& GetPointLightColors() override;

		virtual void SetShaderParams(lesson_1n5::CShader&) override;

		virtual void SetClearColorParam() override;

	private:
		const std::vector<glm::vec3> pointLightPositions = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
		};

		const std::vector<glm::vec3> pointLightColors = {
			glm::vec3(1.0f, 0.6f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0, 0.0),
			glm::vec3(0.2f, 0.2f, 1.0f)
		};
	};

	class CHorrorLightState : public ILightState
	{
	public:
		CHorrorLightState() = default;
	protected:

		virtual const std::vector<glm::vec3>& GetPointLightColors() override;

		virtual void SetShaderParams(lesson_1n5::CShader&) override;

		virtual void SetClearColorParam() override;

	private:
		const std::vector<glm::vec3> pointLightPositions = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
		};

		const std::vector<glm::vec3> pointLightColors = {
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(0.3f, 0.1f, 0.1f)
		};
	};

	class CBioLabLightState : public ILightState
	{
	public:
		CBioLabLightState() = default;
	protected:

		virtual const std::vector<glm::vec3>& GetPointLightColors() override;

		virtual void SetShaderParams(lesson_1n5::CShader&) override;

		virtual void SetClearColorParam() override;

	private:
		const std::vector<glm::vec3> pointLightPositions = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
		};

		const std::vector<glm::vec3> pointLightColors = {
			glm::vec3(0.4f, 0.7f, 0.1f),
			glm::vec3(0.4f, 0.7f, 0.1f),
			glm::vec3(0.4f, 0.7f, 0.1f),
			glm::vec3(0.4f, 0.7f, 0.1f)
		};
	};

	class CFactoryLightState : public ILightState
	{
	public:
		CFactoryLightState() = default;
	protected:

		virtual const std::vector<glm::vec3>& GetPointLightColors() override;

		virtual void SetShaderParams(lesson_1n5::CShader&) override;

		virtual void SetClearColorParam() override;

	private:
		const std::vector<glm::vec3> pointLightPositions = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
		};

		const std::vector<glm::vec3> pointLightColors = {
				glm::vec3(0.2f, 0.2f, 0.6f),
				glm::vec3(0.3f, 0.3f, 0.7f),
				glm::vec3(0.0f, 0.0f, 0.3f),
				glm::vec3(0.4f, 0.4f, 0.4f)
		};
	};

	class CLightStates
	{
	public:
		CLightStates() = default;

		void Init(EState type);

		const std::vector<glm::vec3>& GetPointLightColors();

		void SetShaderParams(lesson_1n5::CShader&);

		void SetClearColorParam();

	private:

		std::shared_ptr<ILightState> currState;
	};
}