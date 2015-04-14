#include "Assesment.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Utility.h"
#include "stb_image.h"
bool Assesment::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	srand(time(0));
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();
	dims = 128;
	size = 20;
	octaves = 5;
	persistance = 0.2f;
	buildGrid(vec2(size, size), glm::ivec2(dims, dims));
	buildPerlinTexture(glm::ivec2(dims, dims), octaves, persistance);
	m_Camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_Camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_Camera.sensitivity = 3;
	const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 position; \
						   layout(location=1) in vec2 texcoord; \
						   out vec2 frag_texcoord; \
						   uniform mat4 view_proj; \
						   uniform sampler2D perlin_texture; \
						   void main() { frag_texcoord = texcoord; \
						   vec4 pos = position; \
						   pos.y += texture(perlin_texture, texcoord).r * 5; \
						   gl_Position = view_proj * pos; }";

	const char* fsSource = "#version 410\n \
						   in vec2 frag_texcoord; \
						   out vec4 out_color; \
						   uniform sampler2D perlin_texture; \
						   void main() { \
						   vec4 tempColour = texture(perlin_texture, frag_texcoord).rrrr; \
						   float edge = clamp(tempColour, 0, 1); \
						   tempColour.a = 1; \
						   if(tempColour.r < 1 && tempColour.r > 0.5) \
						   {tempColour = mix(vec4(1,0,0,1), vec4(0,1,0,1), 1 - edge);} \
						   if(tempColour.r < 0.5 && tempColour.r > 0.3) \
						   {tempColour = mix(vec4(0,1,0,1), vec4(0,0,1,1), 1 - edge);} \
						   if(tempColour.r < 0.3 && tempColour.r > 0) \
						   {tempColour = mix(vec4(0,0,1,1), vec4(1,1,0,1), 1 - edge);} \
						   out_color = tempColour;}";

	/*const char* vsSource = "#version 410\n \
						   	layout(location=0) in vec4 position; \
							layout(location=1) in vec2 texcoord; \
							uniform mat4 view_proj; \
							out vec2 frag_texcoord; \
							uniform sampler2D perlin_texture;\
							void main() { \
							vec4 pos = position;\
							pos.y += texture(perlin_texture, texcoord).r * 5;\
							frag_texcoord = texcoord; \
							gl_Position= view_proj * pos;\
							}";
	const char* fsSource = "#version 410\n \
						   	in vec2 frag_texcoord; \
							out vec4 out_color; \
							uniform sampler2D perlin_texture; \
							void main() { \
							out_color = texture(perlin_texture,frag_texcoord).rrrr;\
							out_color.a = 1;\
							}";*/
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON_MODE);
	return true;
}

bool Assesment::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 blue(0, 0, 1, 1);
	vec4 red(1, 0, 0, 1);
	vec4 yellow(1, 1, 0, 1);
	vec4 green(0, 1, 0, 1);

	for (int i = 0; i <= 20; i++)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? white : black);
	}
	m_Camera.update(dt);

	return true;
}
void Assesment::shutdown()
{
	Gizmos::destroy();
	Application::shutdown();
	glDeleteProgram(m_programID);
}
void Assesment::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_programID);
	int viewProjUnif = glGetUniformLocation(m_programID, "view_proj");

	glUniformMatrix4fv(viewProjUnif, 1, GL_FALSE, (float*)&m_Camera.view_proj);
	int texUniform = glGetUniformLocation(m_programID, "perlin_texture");
	glUniform1i(texUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlinTexture);
	glBindVertexArray(m_planeMesh.m_VAO);
	glDrawElements(GL_TRIANGLES, m_planeMesh.m_indexCount, GL_UNSIGNED_INT, 0);

	Gizmos::draw(m_Camera.view_proj);
	glfwSwapBuffers(this->m_window);
	glfwPollEvents();
}

void Assesment::buildGrid(vec2 realDims, glm::ivec2 dims)
{
	//compute how many verticies we need
	unsigned int vertexCount = (dims.x + 1) * (dims.y + 1);
	//allocate vetex data
	VertexTexCoord* vertexData = new VertexTexCoord[vertexCount];

	//compute how many indicies we need
	unsigned int indexCount = dims.x * dims.y * 6;
	//allocate index data
	unsigned int* indexData = new unsigned int[indexCount];


	float currY = 0;
	//two nested for loops
	for (unsigned int y = 0; y < dims.y + 1; ++y)
	{
		float currX = 0;
		for (unsigned int x = 0; x < dims.x + 1; x++)
		{
			//inside create points
			vertexData[y*(dims.x + 1) + x].position = vec4(currX - (realDims.x / 2), 0, currY - (realDims.y / 2), 1);
			vertexData[y*(dims.x + 1) + x].texCoord = vec2((float)x / dims.x, (float)y / dims.y);
			currX += realDims.x / (float)dims.x;
		}
		currY += realDims.y / (float)dims.y;
	}
	//generate indexData
	int curreIndex = 0;
	for (int y = 0; y < dims.y; y++)
	{
		for (int x = 0; x < dims.x; x++)
		{
			//create our 6 indicies here
			indexData[curreIndex++] = y*(dims.x + 1) + x;
			indexData[curreIndex++] = (y + 1)*(dims.x + 1) + x;
			indexData[curreIndex++] = (y + 1)*(dims.x + 1) + x + 1;

			indexData[curreIndex++] = (y + 1)*(dims.x + 1) + x + 1;
			indexData[curreIndex++] = y*(dims.x + 1) + x + 1;
			indexData[curreIndex++] = y*(dims.x + 1) + x;
		}
	}
	//create vertexArrayObject, buffers, etc
	m_planeMesh.m_indexCount = indexCount;
	glGenVertexArrays(1, &m_planeMesh.m_VAO);
	glGenBuffers(1, &m_planeMesh.m_VBO);
	glGenBuffers(1, &m_planeMesh.m_IBO);
	//bind and fill buffers
	glBindVertexArray(m_planeMesh.m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_planeMesh.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_planeMesh.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord)*vertexCount, vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* indexCount, indexData, GL_STATIC_DRAW);
	//tell openGL about vertex structure
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), (void*)sizeof(vec4));

	//unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] vertexData;
	delete[] indexData;

}

void Assesment::buildPerlinTexture(glm::ivec2 dims, int octaves, float persistance)
{

	//set scale
	float scale = (1.0f / dims.x) * 3.0f;
	//allocate memory for perlin data
	m_perlinData = new float[dims.x * dims.y];
	vec2 offSet;
	offSet.x = rand();
	offSet.y = rand();
	//loop through the pixles
	for (int y = 0; y < dims.y; y++)
	{
		for (int x = 0; x < dims.x; x++)
		{
			//call glm::perlinFunction
			float amplitude = 1;
			float freq = 1;
			m_perlinData[y*dims.x + x] = 0;
			for (int o = 0; o < octaves; o++)
			{
				float perlinSample = glm::perlin(vec2(x + offSet.x, y + offSet.y)*scale * freq) * 0.5 + 0.5;
				perlinSample *= amplitude;
				m_perlinData[y*dims.x + x] += perlinSample;
				amplitude *= persistance;
				freq *= 2;
			}
		}
	}

	//generate openGL texture handles
	glGenTextures(1, &m_perlinTexture);
	glBindTexture(GL_TEXTURE_2D, m_perlinTexture);

	//pass perlin data to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dims.x, dims.y, 0, GL_RED, GL_FLOAT, m_perlinData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/*int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load("./data/textures/crate.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	
	glGenTextures(1, &m_perlinTexture);
	glBindTexture(GL_TEXTURE_2D, m_perlinTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	stbi_image_free(data);*/
}