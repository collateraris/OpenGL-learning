#include "LightStates.h"

using namespace lesson_2n6;

void CLightStates::Init(EState type)
{
	currState.reset();
	switch (type)
	{
	case lesson_2n6::COMMON:
		currState = std::make_shared<CCommonLightState>();
		break;
	case lesson_2n6::DESERT:
		currState = std::make_shared<CDesertLightState>();
		break;
	case lesson_2n6::FACTORY:
		currState = std::make_shared<CFactoryLightState>();
		break;
	case lesson_2n6::HORROR:
		currState = std::make_shared<CHorrorLightState>();
		break;
	case lesson_2n6::BIOCHEMICAL_LAB:
		currState = std::make_shared<CBioLabLightState>();
		break;
	default:
		break;
	}
}

const std::vector<glm::vec3>& CLightStates::GetPointLightColors()
{
	return currState->GetPointLightColors();
}

void CLightStates::SetShaderParams(lesson_1n5::CShader& shader)
{
	currState->SetShaderParams(shader);
}

void CLightStates::SetClearColorParam()
{
	currState->SetClearColorParam();
}

const std::vector<glm::vec3>& CCommonLightState::GetPointLightColors()
{
	return pointLightPositions;
}

void CCommonLightState::SetShaderParams(lesson_1n5::CShader& lightingShader)
{
	lightingShader.setVec3f("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	lightingShader.setVec3f("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	lightingShader.setVec3f("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	lightingShader.setVec3f("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	// point light 1
	lightingShader.setVec3f("pointLights[0].position", pointLightPositions[0]);
	lightingShader.setVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	lightingShader.setVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	lightingShader.setVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	lightingShader.setFloat("pointLights[0].constant", 1.0f);
	lightingShader.setFloat("pointLights[0].linear", 0.09);
	lightingShader.setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	lightingShader.setVec3f("pointLights[1].position", pointLightPositions[1]);
	lightingShader.setVec3f("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	lightingShader.setVec3f("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	lightingShader.setVec3f("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	lightingShader.setFloat("pointLights[1].constant", 1.0f);
	lightingShader.setFloat("pointLights[1].linear", 0.09);
	lightingShader.setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	lightingShader.setVec3f("pointLights[2].position", pointLightPositions[2]);
	lightingShader.setVec3f("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	lightingShader.setVec3f("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	lightingShader.setVec3f("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	lightingShader.setFloat("pointLights[2].constant", 1.0f);
	lightingShader.setFloat("pointLights[2].linear", 0.09);
	lightingShader.setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	lightingShader.setVec3f("pointLights[3].position", pointLightPositions[3]);
	lightingShader.setVec3f("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	lightingShader.setVec3f("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	lightingShader.setVec3f("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	lightingShader.setFloat("pointLights[3].constant", 1.0f);
	lightingShader.setFloat("pointLights[3].linear", 0.09);
	lightingShader.setFloat("pointLights[3].quadratic", 0.032);
}

void CCommonLightState::SetClearColorParam()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

const std::vector<glm::vec3>& CDesertLightState::GetPointLightColors()
{
	return pointLightPositions;
}

void CDesertLightState::SetShaderParams(lesson_1n5::CShader& lightingShader)
{
	lightingShader.setVec3f("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	lightingShader.setVec3f("dirLight.ambient", glm::vec3(0.3f, 0.24f, 0.14f));
	lightingShader.setVec3f("dirLight.diffuse", glm::vec3(0.7f, 0.42f, 0.26f));
	lightingShader.setVec3f("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	// point light 1
	lightingShader.setVec3f("pointLights[0].position", pointLightPositions[0]);
	lightingShader.setVec3f("pointLights[0].ambient", glm::vec3(pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1));
	lightingShader.setVec3f("pointLights[0].diffuse", glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z));
	lightingShader.setVec3f("pointLights[0].specular", glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z));
	lightingShader.setFloat("pointLights[0].constant", 1.0f);
	lightingShader.setFloat("pointLights[0].linear", 0.09);
	lightingShader.setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	lightingShader.setVec3f("pointLights[1].position", pointLightPositions[1]);
	lightingShader.setVec3f("pointLights[1].ambient", glm::vec3(pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1));
	lightingShader.setVec3f("pointLights[1].diffuse", glm::vec3(pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z));
	lightingShader.setVec3f("pointLights[1].specular", glm::vec3(pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z));
	lightingShader.setFloat("pointLights[1].constant", 1.0f);
	lightingShader.setFloat("pointLights[1].linear", 0.09);
	lightingShader.setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	lightingShader.setVec3f("pointLights[2].position", pointLightPositions[2]);
	lightingShader.setVec3f("pointLights[2].ambient", glm::vec3(pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[3].z * 0.1));
	lightingShader.setVec3f("pointLights[2].diffuse", glm::vec3(pointLightColors[2].x, pointLightColors[2].y, pointLightColors[3].z));
	lightingShader.setVec3f("pointLights[2].specular", glm::vec3(pointLightColors[2].x, pointLightColors[2].y, pointLightColors[3].z));
	lightingShader.setFloat("pointLights[2].constant", 1.0f);
	lightingShader.setFloat("pointLights[2].linear", 0.09);
	lightingShader.setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	lightingShader.setVec3f("pointLights[3].position", pointLightPositions[3]);
	lightingShader.setVec3f("pointLights[3].ambient", glm::vec3(pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1));
	lightingShader.setVec3f("pointLights[3].diffuse", glm::vec3(pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z));
	lightingShader.setVec3f("pointLights[3].specular", glm::vec3(pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z));
	lightingShader.setFloat("pointLights[3].constant", 1.0f);
	lightingShader.setFloat("pointLights[3].linear", 0.09);
	lightingShader.setFloat("pointLights[3].quadratic", 0.032);
}

void CDesertLightState::SetClearColorParam()
{
	glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
}

const std::vector<glm::vec3>& CFactoryLightState::GetPointLightColors()
{
	return pointLightPositions;
}

void CFactoryLightState::SetShaderParams(lesson_1n5::CShader& lightingShader)
{
	lightingShader.setVec3f("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	lightingShader.setVec3f("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.1f));
	lightingShader.setVec3f("dirLight.diffuse", glm::vec3(0.2f, 0.2f, 0.7));
	lightingShader.setVec3f("dirLight.specular", glm::vec3(0.7f, 0.7f, 0.7f));

	// point light 1
	lightingShader.setVec3f("pointLights[0].position", pointLightPositions[0]);
	lightingShader.setVec3f("pointLights[0].ambient", glm::vec3(pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1));
	lightingShader.setVec3f("pointLights[0].diffuse", glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z));
	lightingShader.setVec3f("pointLights[0].specular", glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z));
	lightingShader.setFloat("pointLights[0].constant", 1.0f);
	lightingShader.setFloat("pointLights[0].linear", 0.09);
	lightingShader.setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	lightingShader.setVec3f("pointLights[1].position", pointLightPositions[1]);
	lightingShader.setVec3f("pointLights[1].ambient", glm::vec3(pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1));
	lightingShader.setVec3f("pointLights[1].diffuse", glm::vec3(pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z));
	lightingShader.setVec3f("pointLights[1].specular", glm::vec3(pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z));
	lightingShader.setFloat("pointLights[1].constant", 1.0f);
	lightingShader.setFloat("pointLights[1].linear", 0.09);
	lightingShader.setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	lightingShader.setVec3f("pointLights[2].position", pointLightPositions[2]);
	lightingShader.setVec3f("pointLights[2].ambient", glm::vec3(pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[3].z * 0.1));
	lightingShader.setVec3f("pointLights[2].diffuse", glm::vec3(pointLightColors[2].x, pointLightColors[2].y, pointLightColors[3].z));
	lightingShader.setVec3f("pointLights[2].specular", glm::vec3(pointLightColors[2].x, pointLightColors[2].y, pointLightColors[3].z));
	lightingShader.setFloat("pointLights[2].constant", 1.0f);
	lightingShader.setFloat("pointLights[2].linear", 0.09);
	lightingShader.setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	lightingShader.setVec3f("pointLights[3].position", pointLightPositions[3]);
	lightingShader.setVec3f("pointLights[3].ambient", glm::vec3(pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1));
	lightingShader.setVec3f("pointLights[3].diffuse", glm::vec3(pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z));
	lightingShader.setVec3f("pointLights[3].specular", glm::vec3(pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z));
	lightingShader.setFloat("pointLights[3].constant", 1.0f);
	lightingShader.setFloat("pointLights[3].linear", 0.09);
	lightingShader.setFloat("pointLights[3].quadratic", 0.032);
}

void CFactoryLightState::SetClearColorParam()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

const std::vector<glm::vec3>& CHorrorLightState::GetPointLightColors()
{
	return pointLightPositions;
}

void CHorrorLightState::SetShaderParams(lesson_1n5::CShader& lightingShader)
{
	lightingShader.setVec3f("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	lightingShader.setVec3f("dirLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	lightingShader.setVec3f("dirLight.diffuse", glm::vec3(0.05f, 0.05f, 0.05f));
	lightingShader.setVec3f("dirLight.specular", glm::vec3(0.2f, 0.2f, 0.2f));

	// point light 1
	lightingShader.setVec3f("pointLights[0].position", pointLightPositions[0]);
	lightingShader.setVec3f("pointLights[0].ambient", glm::vec3(pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1));
	lightingShader.setVec3f("pointLights[0].diffuse", glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z));
	lightingShader.setVec3f("pointLights[0].specular", glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z));
	lightingShader.setFloat("pointLights[0].constant", 1.0f);
	lightingShader.setFloat("pointLights[0].linear", 0.14);
	lightingShader.setFloat("pointLights[0].quadratic", 0.07);
	// point light 2
	lightingShader.setVec3f("pointLights[1].position", pointLightPositions[1]);
	lightingShader.setVec3f("pointLights[1].ambient", glm::vec3(pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1));
	lightingShader.setVec3f("pointLights[1].diffuse", glm::vec3(pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z));
	lightingShader.setVec3f("pointLights[1].specular", glm::vec3(pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z));
	lightingShader.setFloat("pointLights[1].constant", 1.0f);
	lightingShader.setFloat("pointLights[1].linear", 0.14);
	lightingShader.setFloat("pointLights[1].quadratic", 0.07);
	// point light 3
	lightingShader.setVec3f("pointLights[2].position", pointLightPositions[2]);
	lightingShader.setVec3f("pointLights[2].ambient", glm::vec3(pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[3].z * 0.1));
	lightingShader.setVec3f("pointLights[2].diffuse", glm::vec3(pointLightColors[2].x, pointLightColors[2].y, pointLightColors[3].z));
	lightingShader.setVec3f("pointLights[2].specular", glm::vec3(pointLightColors[2].x, pointLightColors[2].y, pointLightColors[3].z));
	lightingShader.setFloat("pointLights[2].constant", 1.0f);
	lightingShader.setFloat("pointLights[2].linear", 0.22);
	lightingShader.setFloat("pointLights[2].quadratic", 0.20);
	// point light 4
	lightingShader.setVec3f("pointLights[3].position", pointLightPositions[3]);
	lightingShader.setVec3f("pointLights[3].ambient", glm::vec3(pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1));
	lightingShader.setVec3f("pointLights[3].diffuse", glm::vec3(pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z));
	lightingShader.setVec3f("pointLights[3].specular", glm::vec3(pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z));
	lightingShader.setFloat("pointLights[3].constant", 1.0f);
	lightingShader.setFloat("pointLights[3].linear", 0.14);
	lightingShader.setFloat("pointLights[3].quadratic", 0.07);
}

void CHorrorLightState::SetClearColorParam()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

const std::vector<glm::vec3>& CBioLabLightState::GetPointLightColors()
{
	return pointLightPositions;
}

void CBioLabLightState::SetShaderParams(lesson_1n5::CShader& lightingShader)
{
	lightingShader.setVec3f("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	lightingShader.setVec3f("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
	lightingShader.setVec3f("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	lightingShader.setVec3f("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	// point light 1
	lightingShader.setVec3f("pointLights[0].position", pointLightPositions[0]);
	lightingShader.setVec3f("pointLights[0].ambient", glm::vec3(pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1));
	lightingShader.setVec3f("pointLights[0].diffuse", glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z));
	lightingShader.setVec3f("pointLights[0].specular", glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z));
	lightingShader.setFloat("pointLights[0].constant", 1.0f);
	lightingShader.setFloat("pointLights[0].linear", 0.07);
	lightingShader.setFloat("pointLights[0].quadratic", 0.017);
	// point light 2
	lightingShader.setVec3f("pointLights[1].position", pointLightPositions[1]);
	lightingShader.setVec3f("pointLights[1].ambient", glm::vec3(pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1));
	lightingShader.setVec3f("pointLights[1].diffuse", glm::vec3(pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z));
	lightingShader.setVec3f("pointLights[1].specular", glm::vec3(pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z));
	lightingShader.setFloat("pointLights[1].constant", 1.0f);
	lightingShader.setFloat("pointLights[1].linear", 0.07);
	lightingShader.setFloat("pointLights[1].quadratic", 0.017);
	// point light 3
	lightingShader.setVec3f("pointLights[2].position", pointLightPositions[2]);
	lightingShader.setVec3f("pointLights[2].ambient", glm::vec3(pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[3].z * 0.1));
	lightingShader.setVec3f("pointLights[2].diffuse", glm::vec3(pointLightColors[2].x, pointLightColors[2].y, pointLightColors[3].z));
	lightingShader.setVec3f("pointLights[2].specular", glm::vec3(pointLightColors[2].x, pointLightColors[2].y, pointLightColors[3].z));
	lightingShader.setFloat("pointLights[2].constant", 1.0f);
	lightingShader.setFloat("pointLights[2].linear", 0.07);
	lightingShader.setFloat("pointLights[2].quadratic", 0.017);
	// point light 4
	lightingShader.setVec3f("pointLights[3].position", pointLightPositions[3]);
	lightingShader.setVec3f("pointLights[3].ambient", glm::vec3(pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1));
	lightingShader.setVec3f("pointLights[3].diffuse", glm::vec3(pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z));
	lightingShader.setVec3f("pointLights[3].specular", glm::vec3(pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z));
	lightingShader.setFloat("pointLights[3].constant", 1.0f);
	lightingShader.setFloat("pointLights[3].linear", 0.07);
	lightingShader.setFloat("pointLights[3].quadratic", 0.017);
}

void CBioLabLightState::SetClearColorParam()
{
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
}
