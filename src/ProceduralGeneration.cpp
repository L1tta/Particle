#include "ProceduralGeneration.h"

#include "GLM_Header.h"
#include "GL_Header.h"
#include "Gizmos.h"
#include "stb_image.h"

unsigned int ProceduralGeneration::loadShader(unsigned int type, const char* path) {
	FILE* file = fopen(path, "rb");
	if (file == nullptr)
		return 0;
	// read the shader source
	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* source = new char[length + 1];
	memset(source, 0, length + 1);
	fread(source, sizeof(char), length, file);
	fclose(file);
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);
	delete[] source;
	return shader;
}

void ProceduralGeneration::createDrawShader()
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER,
		"lighting_vertex.glsl");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER,
		"lighting_fragment.glsl");
	m_program = glCreateProgram();
	glAttachShader(m_program, vs);
	glAttachShader(m_program, fs);
	glLinkProgram(m_program);
	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void ProceduralGeneration::buildGrid(vec2 real_dims, glm::ivec2 dims)
{
	//unsigned int vertex_count = (dims.x + 1)*(dims.y + 1);
	//VertexTexCoord* vertex_data = new VertexTexCoord[vertex_count];
	//unsigned int index_count = dims.x * dims.y * 6;
	//unsigned int *index_data = new unsigned int[index_count];
	//float curr_y = 0;
	//for (int y = 0; y < dims.y + 1; ++y)
	//{
	//	float curr_x = 0;
	//	for (int x = 0; x < dims.x + 1; ++x)
	//	{
	//		vertex_data[y * (dims.x + 1) + x].position = vec4(curr_x,0,curr_y,1);
	//		vertex_data[y * (dims.x + 1) + x].texCoord = vec2((float)x / (float)dims.x, (float)y / (float)dims.y);
	//		curr_x += real_dims.x / (float)dims.y;
	//	}
	//	curr_x += real_dims.x / (float)dims.x;
	//}
	//int curr_index = 0;
	//for (int y = 0; y < dims.y; ++y)
	//{
	//	for (int x = 0; x < dims.x; ++x)
	//	{
	//		index_data[curr_index++] = y * (dims.x + 1) + x;
	//		index_data[curr_index++] = (y + 1) * (dims.x + 1) + x;
	//		index_data[curr_index++] = (y + 1) * (dims.x + 1) + (x + 1);
	//
	//		index_data[curr_index++] = (y + 1) * (dims.x + 1) + (x + 1);
	//		index_data[curr_index++] = y * (dims.x + 1) + (x + 1);
	//		index_data[curr_index++] = y * (dims.x + 1) + x;
	//	}
	//}
	//m_plane_mesh.m_indexCount = index_count;
	//glGenVertexArrays(1, &m_plane_mesh.m_VAO);
	//glGenBuffers(1, &m_plane_mesh.m_VBO);
	//glGenBuffers(1, &m_plane_mesh.m_IBO);
	//
	//glBindVertexArray(m_plane_mesh.m_VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, m_plane_mesh.m_VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane_mesh.m_IBO);
	//
	//glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord) * vertex_count, vertex_data, GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * index_count, index_data, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), 0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord),(void*)sizeof(vec4));
	//
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool ProceduralGeneration::startup()
{
	m_background_color.r = 0.75f;
	m_background_color.g = 0.75f;
	m_background_color.b = 0.75f;
	m_background_color.a = 1.0f;

	if (Application::startup() == false)
	{
		return false;
	}

	glClearColor(m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.sensitivity = 3;

	const char* vsSource = "#version 410\n \
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
							}";
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//buildGrid(vec2(100,100),glm::ivec2(64,64));

	int dims = 64;
	//glm::ivec2 realDims = glm::ivec2(100,100);
	m_perlin_data = new float[dims * dims];
	float scale = (1.0f / dims) * 3;
	int octaves = 6;
	for (int x = 0; x < 64; ++x)
	{
		for (int y = 0; y < 64; ++y)
		{
			float amplitude = 1.f;
			float persistence = 0.3f;
			m_perlin_data[y * dims + x] = 0;
			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlin_sample =
					glm::perlin(vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				m_perlin_data[y * dims + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}
	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 64, 64, 0, GL_RED, GL_FLOAT, m_perlin_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	float vertex_data[] = {
		-5, 0, -5, 1, 0, 0,
		-5, 0, 5, 1, 0, 1,
		5, 0, 5, 1, 1, 1,
		5, 0, -5, 1, 1, 0,
	};
	unsigned int index_data[] = {
		0, 1, 2,
		0, 2, 3,
	};
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4, vertex_data, GL_STATIC_DRAW);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, index_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glClearColor(0.3f, 0.3f, 0.3f, 0.3f);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void ProceduralGeneration::shutdown()
{
	Gizmos::destroy();
	glDeleteProgram(m_program);
	Application::shutdown();
}


bool ProceduralGeneration::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	Gizmos::clear();
	timer += dt;

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

void ProceduralGeneration::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(m_vao);
	
	glUseProgram(m_program);
	
	// bind the camera
	int loc = glGetUniformLocation(m_program, "view_proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&camera.view_proj);
	//// set texture slot
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
	// tell the shader where it is
	loc = glGetUniformLocation(m_program, "perlin_texture");
	glUniform1i(loc, 0);
	// draw
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}