#include "CameraAndProjections.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Vertex.h"

bool CameraAndProjections::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	// create shaders
	const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \n \
						   layout(location=1) in vec4 Colour; \n \
						   out vec4 vColour; \n \
						   uniform mat4 ProjectionView; \n \
						   void main() { vColour = Colour; gl_Position = ProjectionView * Position;}";

	const char* fsSource = "#version 410\n \
					   in vec4 vColour; \n \
					   out vec4 FragColor; \n \
					   void main() { FragColor = vColour; }";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);
	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	generateGrid(10, 10);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	camera.sensitivity = 3;

	return true;
}

void CameraAndProjections::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

void CameraAndProjections::generateGrid(unsigned int rows, unsigned int cols)
{
	//Sets up Vertexes
	Vertex* vertexArray = new Vertex[(rows + 1) * (cols + 1)];
	for (unsigned int row = 0; row < rows + 1; ++row)
	{
		for (unsigned int col = 0; col < cols + 1; ++col)
		{
			vec4 position = vec4((float)col, 0, (float)row, 1);
			vertexArray[col + row * (cols + 1)].position = position;

			vec4 color = vec4(sinf((row / (float)(rows + 1))), sinf((col / (float)(cols + 1))), 1.0f, 1.0f);
			vertexArray[col + row * (cols + 1)].colour = color;
			//std::cout << "you" << std::endl;
		}
	}

	m_indexCount = rows * cols * 6;

	//Sets up Indicies
	unsigned int* indexArray = new unsigned int[m_indexCount];
	int indexLocation = 0;
	for (unsigned int row = 0; row < rows; ++row)
	{
		for (unsigned int col = 0; col < cols; ++col)
		{
			//Triangle 1
			indexArray[indexLocation + 0] = col + row * (cols + 1);
			indexArray[indexLocation + 1] = col + (row + 1) * (cols + 1);
			indexArray[indexLocation + 2] = (col + 1) + row * (cols + 1);

			//Triangle 2
			indexArray[indexLocation + 3] = (col + 1) + row * (cols + 1);
			indexArray[indexLocation + 4] = col + (row + 1) * (cols + 1);
			indexArray[indexLocation + 5] = (col + 1) + (row + 1) * (cols + 1);

			indexLocation += 6;
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (cols + 1)*(rows + 1)*sizeof(Vertex), vertexArray, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //color

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertexArray;
	delete[] indexArray;
}

bool CameraAndProjections::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
			i == 10 ? white : black);
	}

	camera.update(dt);

	return true;
}

void CameraAndProjections::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(m_VAO);

	glUseProgram(m_programID);

	unsigned int projViewHandle = glGetUniformLocation(m_programID, "ProjectionView");
	if (projViewHandle >= 0)
	{
		glUniformMatrix4fv(projViewHandle, 1, GL_FALSE, (float*)&camera.view_proj);
	}

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	Gizmos::draw(camera.proj, camera.view);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}