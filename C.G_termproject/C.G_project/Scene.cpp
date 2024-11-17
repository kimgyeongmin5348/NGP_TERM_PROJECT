#include "stdafx.h"
#include "Scene.h"

Scene::Scene(Player* pPlayer)
{
	player = pPlayer;
}

Scene::~Scene()
{
}

void Scene::BuildObject()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 깊이 검사 (클리핑)
	glUseProgram(s_program);

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// 미니맵
	glViewport(1050, 550, 150, 150);
	glm::vec3 cameraPos = glm::vec3(player->x_trans_aoc, 5.0f, player->z_trans);
	glm::vec3 cameraDirection = glm::vec3(player->x_trans_aoc, 0.0f, player->z_trans);
	glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 10.0f);

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));

	unsigned int viewLocation = glGetUniformLocation(s_program, "view");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	float lightInitialX = 0;
	float lightInitialY = 10;
	float lightInitialZ = 20.f;
	float lightRotationAngle = glm::radians(0.0f);
	glm::vec4 lightPosition(lightInitialX, lightInitialY, lightInitialZ, 1.0f);
	lightPosition = glm::rotate(glm::mat4(1.0f), lightRotationAngle, glm::vec3(1.0f, 0.0f, 0.0f)) * lightPosition;
	glUseProgram(s_program);
	unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, lightPosition.x, lightPosition.y, lightPosition.z);
	unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, 0.7f, 0.7f, 0.7f);

	// 플레이어, Ground, 빌딩_Mat
	player = new Player();
	ground = new Ground();
	
	for (int i = 0; i < 100; ++i) {
		for (int j = 0; j < 10; ++j) {
			build = new Building();
		}
	}

	glutSwapBuffers();
	glutPostRedisplay();
}
