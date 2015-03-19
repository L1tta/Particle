#include "RenderTarget.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Vertex.h"


bool RenderTarget::startup()
{
	m_background_color.r = 0.3f;
	m_background_color.g = 0.3f;
	m_background_color.b = 0.3f;
	m_background_color.a = 1.0f;

	m_positions[0] = vec3(-10, 5, 10);
	m_positions[1] = vec3(10, 5, 10);

	m_rotations[0] = glm::quat(vec3(0, -1, 0));
	m_rotations[1] = glm::quat(vec3(0, 1, 0));

	if (Application::startup() == false)
	{
		return false;
	}

	glClearColor(m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	camera.setLookAt(vec3(10, 10, 10), vec3(0,0,0), vec3(0, 1, 0));
	camera.sensitivity = 3;
	reflection_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	reflection_camera.setLookAt(vec3(0, 5, -5), vec3(0,5,0), vec3(0, 1, 0));
	reflection_camera.sensitivity = 3;

	// setup framebuffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		m_fboTexture, 0);
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
		1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, m_fboDepth);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// yo future jack just for referance dont mess with the last 3 numbers but the first 3 a positions

	float vertexData[] = {
		5, 0, -5, 1, 0, 0,
		-5, 0, -5, 1, 1, 0,
		-5, 10, -5, 1, 1, 1,
		5, 10, -5, 1, 0, 1,
	};
	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4,
		vertexData, GL_STATIC_DRAW);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6,
		indexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(float)* 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(float)* 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec2 TexCoord; \
						   out vec2 vTexCoord; \
						   uniform mat4 ProjectionView; \
						   void main() { \
						   vTexCoord = TexCoord; \
						   gl_Position = ProjectionView * Position;\
						    }";
	const char* fsSource = "#version 410\n \
						   in vec2 vTexCoord; \
						   out vec4 FragColor; \
						   uniform sampler2D diffuse; \
						   void main() { \
						   FragColor = texture(diffuse,vTexCoord);\
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

	return true;
}

void RenderTarget::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}


bool RenderTarget::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	Gizmos::clear();
	timer += dt;

	float sin_wave = sinf(timer) * 0.5f + 0.5f;

	vec3 final_pos = glm::mix(m_positions[0], m_positions[1], sin_wave);
	glm::quat final_rot = glm::slerp(m_rotations[0], m_rotations[1], sin_wave);

	glm::mat4 transform = glm::translate(final_pos) * glm::toMat4(final_rot);

	Gizmos::addTransform(transform);
	Gizmos::addAABBFilled(final_pos, vec3(1), vec4(0, 0, 1, 1), &transform);

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

void RenderTarget::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 512, 512);
	glClearColor(0.75f, 0.75f, 0.75f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// draw our meshes, or gizmos, to the render target
	//mat4 world = camera.world;
	//vec4 plane = vec4(0,0,1,-5);
	//vec3 reflected = glm::reflect(-world[2].xyz(),plane.xyz());
	//reflected = glm::normalize(reflected);
	//float dist = (glm::dot((plane.xyz()* plane.w) - world[3].xyz(), plane.xyz())) / (glm::dot(plane.xyz(), world[2].xyz()));
	//vec3 inter = world[3].xyz() - world[2].xyz() * dist;
	//world[3].xyz = inter - reflected * dist;
	//world[2].xyz = -reflected;
	//vec3 up = vec3(0,1,0);
	//world[0].xyz = glm::normalize(glm::cross(world[2].xyz(), up));
	//world[1].xyz = glm::normalize(glm::cross(world[0].xyz(), world[2].xyz()));
	//mat4 view = glm::inverse(world);
	//mat4 proj_view = camera.proj * view;
	Gizmos::draw(reflection_camera.view_proj);	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_program);
	int loc = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&camera.view_proj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glUniform1i(glGetUniformLocation(m_program, "diffuse"), 0);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);	// draw our meshes, or gizmos, to the render target
	Gizmos::draw(camera.view_proj);
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}