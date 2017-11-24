#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define S_NUM (40)

#ifndef PI
#define PI (3.1415926f)

#endif // !PI


class Sphere
{
public:
	unsigned int sphereVBO, sphereVAO, sphereEBO;
	float field_vertices[S_NUM * S_NUM * 3];
	unsigned int indices[(S_NUM - 1) * (S_NUM- 1) * 6];

	// generate the sphere at 0,0,0 in sphere space, radius = 1
	Sphere()
	{
		//sphereModel = glm::mat4(1.0f);
		float delta = 2 * PI / S_NUM;
		int dx = 0;
		for (float theta = 0; theta < 2 * PI; theta += delta)
		{
			for (float phi = 0; phi < 2 * PI; phi += delta)
			{
				float x = sin(theta) * cos(phi);
				float y = sin(theta) * sin(phi);
				float z = cos(theta);

				field_vertices[dx * 3] = x;
				field_vertices[dx * 3 + 1] = y;
				field_vertices[dx * 3 + 2] = z;
				dx++;
			}
		}

		for (int i = 0; i < S_NUM - 1; i++)
		{
			for (int j = 0; j < S_NUM - 1; j++)
			{
				unsigned int a = (j + 0) * S_NUM + (i + 0); // bottom left
				unsigned int b = (j + 0) * S_NUM + (i + 1); // bottom right
				unsigned int c = (j + 1) * S_NUM + (i + 1); // top right
				unsigned int d = (j + 1) * S_NUM + (i + 0); // top left
				int dx = j * (S_NUM- 1) + i;
				indices[dx * 6 + 0] = a;
				indices[dx * 6 + 1] = b;
				indices[dx * 6 + 2] = c;

				indices[dx * 6 + 3] = a;
				indices[dx * 6 + 4] = c;
				indices[dx * 6 + 5] = d;
			}
		}

		glGenVertexArrays(1, &sphereVAO);
		glGenBuffers(1, &sphereVBO);
		glGenBuffers(1, &sphereEBO);

		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(field_vertices), field_vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

};
