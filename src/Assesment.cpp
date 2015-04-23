#include "Assesment.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Utility.h"
#include "stb_image.h"

void On_mouse_button(GLFWwindow* window, int button, int pressed, int alt_keys)
{
	TwEventMouseButtonGLFW(button, pressed);
}
void On_mouse_position(GLFWwindow* window, double x, double y)
{
	TwEventMousePosGLFW((int)x, (int)y);
}
void On_mouse_scroll(GLFWwindow* window, double x, double y)
{
	TwEventMouseWheelGLFW((int)y);
}
void On_key(GLFWwindow* window, int key, int scancode, int pressed, int mod_keys)
{
	TwEventKeyGLFW(key, pressed);
}
void On_char(GLFWwindow* window, unsigned int c)
{
	TwEventCharGLFW(c, GLFW_PRESS);
}
void On_window_resize(GLFWwindow* window, int width, int height)
{
	TwWindowSize(width, height);
	glViewport(0, 0, width, height);
}

bool Assesment::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	glfwSetMouseButtonCallback(m_window, On_mouse_button);
	glfwSetCursorPosCallback(m_window, On_mouse_position);
	glfwSetScrollCallback(m_window, On_mouse_scroll);
	glfwSetKeyCallback(m_window, On_key);
	glfwSetCharCallback(m_window, On_char);
	glfwSetWindowSizeCallback(m_window, On_window_resize);

	TwInit(TW_OPENGL, NULL);
	TwWindowSize(1280, 720);
	m_bar = TwNewBar("AssignmentBar");
	Time = 0;
	srand(time_t(0));
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();
	dims = 128;
	size = 20;
	octaves = 5;
	persistance = 0.2f;
	Reload = false;
	buildGrid(vec2(size, size), glm::ivec2(dims, dims));
	buildPerlinTexture(glm::ivec2(dims, dims), octaves, persistance);
	m_Camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_Camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_Camera.sensitivity = 3;
	m_lightDirection = vec3(0,1,0);
	//const char* vsSource = "#version 410\n \
	//					   layout(location=0) in vec4 position; \
	//					   layout(location=1) in vec2 texcoord; \
	//					   out vec2 frag_texcoord; \
	//					   out vec4 normals;\
	//					   uniform mat4 view_proj; \
	//					   uniform mat4 worldProj;\
	//					   uniform sampler2D perlin_texture; \
	//					   void main() { frag_texcoord = texcoord; \
	//					   int offsetPos = 50;\
	//					   fragTexCoord = texcoord;\
	//					   vec4 pos = position;\
	//					   	pos.y += texture(perlinTexture, texcoord).r * 5;\
	//					   	gl_Position = view_proj * pos;\
	//					   \
	//					   	float texturesize = textureSize(perlin_texture, 0);\
	//					   	float textureoffset = 1 / texturesize;\
	//					   	float sampleSize = 1.0f;\
	//						pos.y -= texture(perlinTexture, texcoord).r * 5;\
	//					   	vec4 posUp = pos;\
	//					   	posUp.x += 0;\
	//					   	posUp.y = texture(perlin_texture, texcoord + vec2(0, textureoffset).r) * offsetPos;\
	//					   	posUp.z += sampleSize;\
	//					   \
	//					   	vec4 posUpRight = pos;\
	//					   	posUpRight.x += sampleSize;\
	//					   	posUpRight.y = texture(perlin_texture, texcoord + vec2(textureoffset, textureoffset).r) * offsetPos;\
	//					   	posUpRight.z += sampleSize;\
	//					   \
	//					   	vec4 posRight = pos;\
	//					   	posRight.x += sampleSize;\
	//					   	posRight.y = texture(perlin_texture, texcoord + vec2(textureoffset, 0).r) * offsetPos;\
	//					   \
	//					   	vec4 posDownRight = pos;\
	//					   	posDownRight.x += sampleSize;\
	//					   	posDownRight.y = texture(perlin_texture, texcoord + vec2(textureoffset, -textureoffset).r) * offsetPos;\
	//					   	posDownRight.z -= sampleSize;\
	//					   \
	//					   	vec4 posDown = pos;\
	//					   	posDown.x += 0;\
	//					   	posDown.y = texture(perlin_texture, texcoord + vec2(0, -textureoffset).r) * offsetPos;\
	//					   	posDown.z -= sampleSize;\
	//					   \
	//					   	vec4 posDownLeft = pos;\
	//					   	posDownLeft.x -= sampleSize;\
	//					   	posDownLeft.y = texture(perlin_texture, texcoord + vec2(-textureoffset, -textureoffset).r) * offsetPos;\
	//					   	posDownLeft.z -= sampleSize;\
	//					   \
	//					   	vec4 posLeft = pos;\
	//					   	posLeft.x -= sampleSize;\
	//					   	posLeft.y = texture(perlin_texture, texcoord + vec2(-textureoffset, 0).r) * offsetPos;\
	//					   	posLeft.z += 0;\
	//					   \
	//					   	vec4 posUpLeft = pos;\
	//					   	posUpLeft.x -= sampleSize;\
	//					   	posUpLeft.y = texture(perlin_texture, texcoord + vec2(-textureoffset, textureoffset).r) * offsetPos;\
	//					   	posUpLeft.z += sampleSize;\
	//					   \
	//						vec4 finalNormal = vec4(0, 0, 0, 0);\
	//					   \
	//					   \
	//					   \
	//					   	finalNormal.xyz += cross(posUp.xyz - pos.xyz, posUpRight.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posUpRight.xyz - pos.xyz, posRight.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posRight.xyz - pos.xyz, posDownRight.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posDownRight.xyz - pos.xyz, posDown.xyz - pos.xyz);\
	//					   \
	//					   	finalNormal.xyz += cross(posDown.xyz - pos.xyz, posDownLeft.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posDownLeft.xyz - pos.xyz, posLeft.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posLeft.xyz - pos.xyz, posUpLeft.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posUpLeft.xyz - pos.xyz, posUp.xyz - pos.xyz);\
	//					   \
	//					   \
	//					   	normals = normalize(finalNormal); }";
	//					  /* "#version 410\n \
	//					   layout(location = 0) in vec4 position;\
	//					   layout(location = 1) in vec2 texCoord;\
	//					   \
	//					   out vec2 fragTexCoord;\
	//					   out vec4 normals;\
	//					   \
	//					   uniform mat4 viewProj;\
	//					   uniform mat4 worldProj;\
	//					   uniform sampler2D perlinTexture;\
	//					   void main()\
	//					   {\
	//					   	int offsetPos = 50;\
	//					   	fragTexCoord = texcoord;\
	//					   \
	//					   	vec4 pos = position;\
	//					   	pos.y += texture(perlinTexture, texcoord).r * 5;\
	//					   	gl_Position = view_proj * pos;\
	//					   \
	//					   	float texturesize = textureSize(perlin_texture, 0);\
	//					   	float textureoffset = 1 / texturesize;\
	//					   	float sampleSize = 1.0f;\
	//					   \
	//					   	vec4 posUp = pos;\
	//					   	posUp.x += 0;\
	//					   	posUp.y = texture(perlin_texture, texcoord + vec2(0, textureoffset).r) * offsetPos;\
	//					   	posUp.z += sampleSize;\
	//					   \
	//					   	vec4 posUpRight = pos;\
	//					   	posUpRight.x += sampleSize;\
	//					   	posUpRight.y = texture(perlinTexture, texCoord + vec2(textureoffset, textureoffset).r) * offsetPos;\
	//					   	posUpRight.z += sampleSize;\
	//					   \
	//					   	vec4 posRight = pos;\
	//					   	posRight.x += sampleSize;\
	//					   	posRight.y = texture(perlinTexture, texCoord + vec2(textureoffset, 0).r) * offsetPos;\
	//					   \
	//					   	vec4 posDownRight = pos;\
	//					   	posDownRight.x += sampleSize;\
	//					   	posDownRight.y = texture(perlinTexture, texCoord + vec2(textureoffset, -textureoffset).r) * offsetPos;\
	//					   	posDownRight.z -= sampleSize;\
	//					   \
	//					   	vec4 posDown = pos;\
	//					   	posDown.x += 0;\
	//					   	posDown.y = texture(perlinTexture, texCoord + vec2(0, -textureoffset).r) * offsetPos;\
	//					   	posDown.z -= sampleSize;\
	//					   \
	//					   	vec4 posDownLeft = pos;\
	//					   	posDownLeft.x -= sampleSize;\
	//					   	posDownLeft.y = texture(perlinTexture, texCoord + vec2(-textureoffset, -textureoffset).r) * offsetPos;\
	//					   	posDownLeft.z -= sampleSize;\
	//					   \
	//					   	vec4 posLeft = pos;\
	//					   	posLeft.x -= sampleSize;\
	//					   	posLeft.y = texture(perlinTexture, texCoord + vec2(-textureoffset, 0).r) * offsetPos;\
	//					   	posLeft.z += 0;\
	//					   \
	//					   	vec4 posUpLeft = pos;\
	//					   	posUpLeft.x -= sampleSize;\
	//					   	posUpLeft.y = texture(perlinTexture, texCoord + vec2(-textureoffset, textureoffset).r) * offsetPos;\
	//					   	posUpLeft.z += sampleSize;\
	//					   \
	//						vec4 finalNormal = vec4(0, 0, 0, 0);\
	//					   \
	//					   \
	//					   \
	//					   	finalNormal.xyz += cross(posUp.xyz - pos.xyz, posUpRight.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posUpRight.xyz - pos.xyz, posRight.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posRight.xyz - pos.xyz, posDownRight.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posDownRight.xyz - pos.xyz, posDown.xyz - pos.xyz);\
	//					   \
	//					   	finalNormal.xyz += cross(posDown.xyz - pos.xyz, posDownLeft.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posDownLeft.xyz - pos.xyz, posLeft.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posLeft.xyz - pos.xyz, posUpLeft.xyz - pos.xyz);\
	//					   	finalNormal.xyz += cross(posUpLeft.xyz - pos.xyz, posUp.xyz - pos.xyz);\
	//					   \
	//					   \
	//					   	normals = normalize(finalNormal);";*/

	//const char* fsSource = "#version 410\n \
	//					   in vec2 frag_texcoord; \
	//					   in vec4 normals;\
	//					   out vec4 out_color; \
	//					   uniform sampler2D perlin_texture; \
	//					   uniform vec3 m_lightDirection;\
	//					   void main() { \
	//					   vec4 tempColour = texture(perlin_texture, frag_texcoord).rrrr; \
	//					   float edge = clamp(tempColour, 0, 1); \
	//					   tempColour.a = 1; \
	//					   if(tempColour.r < 1 && tempColour.r > 0.5) \
	//					   {tempColour = mix(vec4(1,0,0,1), vec4(0,1,0,1), 1 - edge);} \
	//					   if(tempColour.r < 0.5 && tempColour.r > 0.3) \
	//					   {tempColour = mix(vec4(0,1,0,1), vec4(0,0,1,1), 1 - edge);} \
	//					   if(tempColour.r < 0.3 && tempColour.r > 0) \
	//					   {tempColour = mix(vec4(0,0,1,1), vec4(1,1,0,1), 1 - edge);} \
	//					   vec3 ambient = tempColour.xyz * vec3(0.1, 0.1, 0.1);\
	//					   \
	//					   	vec3 N = normalize(normals.xyz);\
	//					   \
	//					   	vec3 L = normalize(m_lightDirection);\
	//					   	float d = max(0, dot(N, -L));\
	//					   	vec3 FinalDiffuse = vec3(d) * vec3(1, 1, 1) * tempColour.xyz;\
	//					   \
	//					   	out_color = vec4(ambient + FinalDiffuse, 1);}";
	//					   /*"#version 410 \n \
	//					   \
	//					   in vec2 fragTexCoord;\
	//					   in vec4 normals;\
	//					   \
	//					   out vec4 fragColour;\
	//					   uniform sampler2D perlinTexture;\
	//					   \
	//					   uniform sampler2D RockTexture;\
	//					   uniform sampler2D GrassTexture;\
	//					   uniform sampler2D WaterTexture;\
	//					   uniform vec3 m_lightDirection;\
	//					   \
	//					   void main()\
	//					   {\
	//					   	vec4 tempColour = texture(perlinTexture, fragTexCoord);\
	//					   	vec4 tempColour1 = texture(perlinTexture, fragTexCoord);\
	//					   	vec4 rockColour = texture(RockTexture, fragTexCoord);\
	//					   	vec4 grassColour = texture(GrassTexture, fragTexCoord);\
	//					   	vec4 waterColour = texture(WaterTexture, fragTexCoord);\
	//					   	float edge = clamp(tempColour, 0, 1);\
	//					   \
	//					   \
	//					   	tempColour1 = rockColour;\
	//					   	if (tempColour.r <= 1 && tempColour.r > 0.5)\
	//					   	{\
	//					   \
	//					   		tempColour1 = rockColour;\
	//					   \
	//					   	}\
	//					   	if (tempColour.r < 0.5 && tempColour.r > 0.3)\
	//					   	{\
	//					   		tempColour1 = mix(rockColour, grassColour, 1 - edge);\
	//					   	}\
	//					   	if (tempColour.r < 0.3 && tempColour.r > 0)\
	//					   	{\
	//					   		tempColour1 = mix(grassColour, waterColour, 1 - edge);\
	//					   	}\
	//					   	vec3 ambient = tempColour1.xyz * vec3(0.1, 0.1, 0.1);\
	//					   \
	//					   	vec3 N = normalize(normals.xyz);\
	//					   \
	//					   	vec3 L = normalize(m_lightDirection);\
	//					   	float d = max(0, dot(N, -L));\
	//					   	vec3 FinalDiffuse = vec3(d) * vec3(1, 1, 1) * tempColour1.xyz;\
	//					   \
	//					   	fragColour = vec4(ambient + FinalDiffuse, 1);\
	//					   \
	//					   }";*/

	//unsigned int pgvertexShader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(pgvertexShader, 1, (const char**)&vsSource, 0);
	//glCompileShader(pgvertexShader);
	//unsigned int pgfragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(pgfragmentShader, 1, (const char**)&fsSource, 0);
	//glCompileShader(pgfragmentShader);
	//m_programID = glCreateProgram();
	//glAttachShader(m_programID, pgvertexShader);
	//glAttachShader(m_programID, pgfragmentShader);
	//glLinkProgram(m_programID);
	//glDeleteShader(pgvertexShader);
	//glDeleteShader(pgfragmentShader);

	const char * vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec2 TexCoord; \
						   out vec2 vTexCoord; \
						   out vec4 normals;\
						   uniform mat4 ProjectionView; \
						   uniform sampler2D perlin; \
						   void main() { \
						   vTexCoord = TexCoord; \
						   int offsetPos = 50;\
						   vec4 pos = Position; \
						   pos.y += texture(perlin, TexCoord).r * 5;\
						   gl_Position= ProjectionView * pos;\
						   float texturesize = textureSize(perlin, 0);\
						   float textureoffset = 1 / texturesize;\
						   float sampleSize = 1.0f;\
						   \
						   	vec4 posUp = pos;\
						   	posUp.x += 0;\
						   	posUp.y = texture(perlin, TexCoord + vec2(0, textureoffset).r) * offsetPos;\
						   	posUp.z += sampleSize;\
						   \
						   	vec4 posUpRight = pos;\
						   	posUpRight.x += sampleSize;\
						   	posUpRight.y = texture(perlin, TexCoord + vec2(textureoffset, textureoffset).r) * offsetPos;\
						   	posUpRight.z += sampleSize;\
						   \
						   	vec4 posRight = pos;\
						   	posRight.x += sampleSize;\
						   	posRight.y = texture(perlin, TexCoord + vec2(textureoffset, 0).r) * offsetPos;\
						   \
						   	vec4 posDownRight = pos;\
						   	posDownRight.x += sampleSize;\
						   	posDownRight.y = texture(perlin, TexCoord + vec2(textureoffset, -textureoffset).r) * offsetPos;\
						   	posDownRight.z -= sampleSize;\
						   \
						   	vec4 posDown = pos;\
						   	posDown.x += 0;\
						   	posDown.y = texture(perlin, TexCoord + vec2(0, -textureoffset).r) * offsetPos;\
						   	posDown.z -= sampleSize;\
						   \
						   	vec4 posDownLeft = pos;\
						   	posDownLeft.x -= sampleSize;\
						   	posDownLeft.y = texture(perlin, TexCoord + vec2(-textureoffset, -textureoffset).r) * offsetPos;\
						   	posDownLeft.z -= sampleSize;\
						   \
						   	vec4 posLeft = pos;\
						   	posLeft.x -= sampleSize;\
						   	posLeft.y = texture(perlin, TexCoord + vec2(-textureoffset, 0).r) * offsetPos;\
						   	posLeft.z += 0;\
						   \
						   	vec4 posUpLeft = pos;\
						   	posUpLeft.x -= sampleSize;\
						   	posUpLeft.y = texture(perlin, TexCoord + vec2(-textureoffset, textureoffset).r) * offsetPos;\
						   	posUpLeft.z += sampleSize;\
						   \
							vec4 finalNormal = vec4(0, 0, 0, 0);\
						   \
						   \
						   \
						   	finalNormal.xyz += cross(posUp.xyz - pos.xyz, posUpRight.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posUpRight.xyz - pos.xyz, posRight.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posRight.xyz - pos.xyz, posDownRight.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posDownRight.xyz - pos.xyz, posDown.xyz - pos.xyz);\
						   \
						   	finalNormal.xyz += cross(posDown.xyz - pos.xyz, posDownLeft.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posDownLeft.xyz - pos.xyz, posLeft.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posLeft.xyz - pos.xyz, posUpLeft.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posUpLeft.xyz - pos.xyz, posUp.xyz - pos.xyz);\
						   \
						   \
						   	normals = normalize(finalNormal);}";
	const char *fsSource = "#version 410\n \
						   in vec2 vTexCoord; \
						   out vec4 FragColor; \
						   in vec4 normals;\
						   uniform vec3 m_lightDirection; \
						   uniform sampler2D diffuse; \
						   void main() { \
						   FragColor = texture(diffuse,vTexCoord);\
						   vec4 tempColour = texture(diffuse, vTexCoord);\
						   float d = max(0, \
						   dot( normalize(normals.xyz), \
						   m_lightDirection ) ); \
						   FragColor = vec4(vec4(d,d,d,1) + tempColour);}";
	unsigned int tvertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(tvertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(tvertexShader);
	unsigned int tfragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(tfragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(tfragmentShader);
	m_tprogramID = glCreateProgram();
	glAttachShader(m_tprogramID, tvertexShader);
	glAttachShader(m_tprogramID, tfragmentShader);
	glLinkProgram(m_tprogramID);
	glDeleteShader(tvertexShader);
	glDeleteShader(tfragmentShader);

	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data1 = stbi_load("./data/textures/grass1.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data1);

	unsigned char* data2 = stbi_load("./data/models/soulspear/soulspear_.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_texture2);
	glBindTexture(GL_TEXTURE_2D, m_texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data2);

	unsigned char* data3 = stbi_load("./data/models/tanks/Battle_tank.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_texture3);
	glBindTexture(GL_TEXTURE_2D, m_texture3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data3);

	m_emitter = new GPUParticleEmitter();
	m_emitter->initalise(1000, 5.0f, 20.0f, 10, 40, 0.1f, 0.1f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));
	m_fbx = new FBXFile();
	m_fbx->initialiseOpenGLTextures();
	m_fbx->load("./data/models/soulspear.fbx");
	createOpenGLBuffers(m_fbx);

	m_fbx2 = new FBXFile();
	m_fbx2->initialiseOpenGLTextures();
	m_fbx2->load("./data/models/tanks/battle_tank.fbx");
	createOpenGLBuffers(m_fbx2);

	// directional lighting
	vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec4 Normal; \
						   layout(location=2) in vec2 texcoord;\
						   out vec4 vNormal; \
						   out vec2 frag_texcoord; \
						   uniform mat4 ProjectionView; \
						   void main() { vNormal = Normal; \
						   frag_texcoord = texcoord; \
						   gl_Position = ProjectionView * Position;}";
	fsSource = "#version 410\n \
						   in vec4 vNormal; \
						   out vec4 FragColor; \
						   in vec2 frag_texcoord; \
						   uniform sampler2D diffuse; \
						   uniform vec3 m_lightDirection; \
						   void main() { \
						   vec4 tempColour = texture(diffuse, frag_texcoord);\
						   float d = max(0, \
						   dot( normalize(vNormal.xyz), \
						   m_lightDirection ) ); \
						   FragColor = vec4(vec4(d,d,d,1) + tempColour);}";
	unsigned int fbxvertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(fbxvertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(fbxvertexShader);
	unsigned int fbxfragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fbxfragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fbxfragmentShader);
	m_fbxprogramID = glCreateProgram();
	glAttachShader(m_fbxprogramID, fbxvertexShader);
	glAttachShader(m_fbxprogramID, fbxfragmentShader);
	glLinkProgram(m_fbxprogramID);
	glDeleteShader(fbxvertexShader);
	glDeleteShader(fbxfragmentShader);

	m_StartSize = 0.1f;
	m_EndSize = 0.1f;
	m_StartColour = vec4(1, 0, 0, 1);
	m_EndColour = vec4(1,1,0,1);
	TwAddVarRW(m_bar, "lightDirection", TW_TYPE_DIR3F, &m_lightDirection, "group=Light");
	TwAddVarRW(m_bar, "Reload", TW_TYPE_BOOL8, &Reload, "group=Particle");
	TwAddVarRW(m_bar, "Perlin", TW_TYPE_FLOAT, &dims, "group=Plain max=200");
	TwAddVarRW(m_bar, "Size", TW_TYPE_FLOAT, &size, "group=Plain max=200");
	TwAddVarRW(m_bar, "Octaves", TW_TYPE_INT32, &octaves, "group=Plain max=40");
	TwAddVarRW(m_bar, "Persistance", TW_TYPE_FLOAT, &persistance, "group=Plain max=1");
	TwAddVarRW(m_bar, "StartColour", TW_TYPE_COLOR4F, &m_StartColour, "group=Particle");
	TwAddVarRW(m_bar, "EndColour", TW_TYPE_COLOR4F, &m_EndColour, "group=Particle");
	TwAddVarRW(m_bar, "startsize", TW_TYPE_FLOAT, &m_StartSize, "group=Particle");
	TwAddVarRW(m_bar, "endsize", TW_TYPE_FLOAT, &m_EndSize, "group=Particle");

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
	Time += dt;

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
	if (Reload == true)
	{
		UpdateParticle();
		UpdatePlain();
		Reload = false;
	}
	return true;
}
void Assesment::shutdown()
{
	Gizmos::destroy();
	cleanupOpenGLBuffers(m_fbx);
	cleanupOpenGLBuffers(m_fbx2);
	Application::shutdown();
	glDeleteProgram(m_programID);
	glDeleteProgram(m_tprogramID);
	glDeleteProgram(m_fbxprogramID);
}
void Assesment::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(m_planeMesh.m_VAO);

	glUseProgram(m_tprogramID);
	// bind the camera
	int loc = glGetUniformLocation(m_tprogramID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&m_Camera.view_proj);
	int LightDir_Uniform = glGetUniformLocation(m_tprogramID, "m_lightDirection");
	glUniform3fv(LightDir_Uniform, 1, (float*)&m_lightDirection);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_perlinTexture);

	// set texture slot
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	// tell the shader where it is
	loc = glGetUniformLocation(m_tprogramID, "diffuse");
	glUniform1i(loc, 0);
	loc = glGetUniformLocation(m_tprogramID, "perlin");
	glUniform1i(loc, 1);
	// draw
	glBindVertexArray(m_planeMesh.m_VAO);
	glDrawElements(GL_TRIANGLES, m_planeMesh.m_indexCount, GL_UNSIGNED_INT, nullptr);
	Gizmos::draw(m_Camera.view_proj);

	m_emitter->draw(Time, m_Camera.world, m_Camera.view_proj);

	glUseProgram(m_fbxprogramID);
	// bind the camera
	int fbxloc = glGetUniformLocation(m_fbxprogramID, "ProjectionView");
	glUniformMatrix4fv(fbxloc, 1, GL_FALSE,
		&(m_Camera.view_proj[0][0]));
	LightDir_Uniform = glGetUniformLocation(m_fbxprogramID, "m_lightDirection");
	glUniform3fv(LightDir_Uniform, 1, (float*)&m_lightDirection);
	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		FBXMaterial* meshMaterial = mesh->m_material;
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture2);

		mat4 worldTransform = m_fbx->getMeshByIndex(0)->m_globalTransform;
		int world_uniform = glGetUniformLocation(m_fbxprogramID, "world");
		glUniformMatrix4fv(world_uniform, 1, GL_FALSE, (float*)&worldTransform);
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	for (unsigned int i = 0; i < m_fbx2->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx2->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture3);

		mat4 worldTransform = m_fbx->getMeshByIndex(0)->m_globalTransform;
		int world_uniform = glGetUniformLocation(m_fbxprogramID, "world");
		glUniformMatrix4fv(world_uniform, 1, GL_FALSE, (float*)&worldTransform);
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	Gizmos::draw(m_Camera.view_proj);
	TwDraw();

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
	offSet.x = (float)rand();
	offSet.y = (float)rand();
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	stbi_image_free(data);*/
}

void Assesment::createOpenGLBuffers(FBXFile* fbx)
{
	// create the GL VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		// storage for the opengl data in 3 unsigned int
		unsigned int* glData = new unsigned int[3];
		// continued next page…
		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);
		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);
		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
			sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex),
			((char*)0) + FBXVertex::NormalOffset);

		glEnableVertexAttribArray(2); //texcoord
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 
			sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		mesh->m_userData = glData;
	}
	//unsigned int meshCount = fbx->getMeshCount();
	//FBXMeshNode* m_meshes = fbx->getMeshByIndex(meshIndex);
	//for (unsigned int meshIndex = 0; meshIndex < meshCount; meshIndex++)
	//{
	//	FBXMeshNode* currMesh = fbx->getMeshByIndex(meshIndex);
	//	m_meshes[meshIndex].m_index_count = currMesh->m_indices.size();
	//	glGenBuffers(1, &m_meshes[meshIndex].m_VBO);
	//	glGenBuffers(1, &m_meshes[meshIndex].m_IBO);
	//	glGenVertexArrays(1, &m_meshes[meshIndex].m_VAO);

	//	glBindVertexArray(m_meshes[meshIndex].m_VAO);
	//	glBindBuffer(GL_ARRAY_BUFFER, m_meshes[meshIndex].m_VBO);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)*currMesh->m_vertices.size(), currMesh->m_vertices.data(), GL_STATIC_DRAW);

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshes[meshIndex].m_IBO);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*currMesh->m_indices.size(), currMesh->m_indices.data(), GL_STATIC_DRAW);

	//	glEnableVertexAttribArray(0); //pos
	//	glEnableVertexAttribArray(1); //texcoord
	//	glEnableVertexAttribArray(2); //Bone indices
	//	glEnableVertexAttribArray(3); //Bone weights
	//	glEnableVertexAttribArray(4);//normal

	//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
	//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
	//	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
	//	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
	//	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);

	//	glBindVertexArray(0);
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Assesment::cleanupOpenGLBuffers(FBXFile* fbx) {
	// clean up the vertex data attached to each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);
		delete[] glData;
	}
}

void Assesment::UpdatePlain()
{
	buildGrid(vec2(size, size), glm::ivec2(dims, dims));
	buildPerlinTexture(glm::ivec2(dims, dims), octaves, persistance);
	glDeleteProgram(m_programID);
	glDeleteProgram(m_tprogramID);
	glDeleteProgram(m_fbxprogramID);
		const char * vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec2 TexCoord; \
						   out vec2 vTexCoord; \
						   out vec4 normals;\
						   uniform mat4 ProjectionView; \
						   uniform sampler2D perlin; \
						   void main() { \
						   vTexCoord = TexCoord; \
						   int offsetPos = 50;\
						   vec4 pos = Position; \
						   pos.y += texture(perlin, TexCoord).r * 5;\
						   gl_Position= ProjectionView * pos;\
						   float texturesize = textureSize(perlin, 0);\
						   float textureoffset = 1 / texturesize;\
						   float sampleSize = 1.0f;\
						   \
						   	vec4 posUp = pos;\
						   	posUp.x += 0;\
						   	posUp.y = texture(perlin, TexCoord + vec2(0, textureoffset).r) * offsetPos;\
						   	posUp.z += sampleSize;\
						   \
						   	vec4 posUpRight = pos;\
						   	posUpRight.x += sampleSize;\
						   	posUpRight.y = texture(perlin, TexCoord + vec2(textureoffset, textureoffset).r) * offsetPos;\
						   	posUpRight.z += sampleSize;\
						   \
						   	vec4 posRight = pos;\
						   	posRight.x += sampleSize;\
						   	posRight.y = texture(perlin, TexCoord + vec2(textureoffset, 0).r) * offsetPos;\
						   \
						   	vec4 posDownRight = pos;\
						   	posDownRight.x += sampleSize;\
						   	posDownRight.y = texture(perlin, TexCoord + vec2(textureoffset, -textureoffset).r) * offsetPos;\
						   	posDownRight.z -= sampleSize;\
						   \
						   	vec4 posDown = pos;\
						   	posDown.x += 0;\
						   	posDown.y = texture(perlin, TexCoord + vec2(0, -textureoffset).r) * offsetPos;\
						   	posDown.z -= sampleSize;\
						   \
						   	vec4 posDownLeft = pos;\
						   	posDownLeft.x -= sampleSize;\
						   	posDownLeft.y = texture(perlin, TexCoord + vec2(-textureoffset, -textureoffset).r) * offsetPos;\
						   	posDownLeft.z -= sampleSize;\
						   \
						   	vec4 posLeft = pos;\
						   	posLeft.x -= sampleSize;\
						   	posLeft.y = texture(perlin, TexCoord + vec2(-textureoffset, 0).r) * offsetPos;\
						   	posLeft.z += 0;\
						   \
						   	vec4 posUpLeft = pos;\
						   	posUpLeft.x -= sampleSize;\
						   	posUpLeft.y = texture(perlin, TexCoord + vec2(-textureoffset, textureoffset).r) * offsetPos;\
						   	posUpLeft.z += sampleSize;\
						   \
							vec4 finalNormal = vec4(0, 0, 0, 0);\
						   \
						   \
						   \
						   	finalNormal.xyz += cross(posUp.xyz - pos.xyz, posUpRight.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posUpRight.xyz - pos.xyz, posRight.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posRight.xyz - pos.xyz, posDownRight.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posDownRight.xyz - pos.xyz, posDown.xyz - pos.xyz);\
						   \
						   	finalNormal.xyz += cross(posDown.xyz - pos.xyz, posDownLeft.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posDownLeft.xyz - pos.xyz, posLeft.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posLeft.xyz - pos.xyz, posUpLeft.xyz - pos.xyz);\
						   	finalNormal.xyz += cross(posUpLeft.xyz - pos.xyz, posUp.xyz - pos.xyz);\
						   \
						   \
						   	normals = normalize(finalNormal);}";
	const char *fsSource = "#version 410\n \
						   in vec2 vTexCoord; \
						   out vec4 FragColor; \
						   in vec4 normals;\
						   uniform vec3 m_lightDirection; \
						   uniform sampler2D diffuse; \
						   void main() { \
						   FragColor = texture(diffuse,vTexCoord);\
						   vec4 tempColour = texture(diffuse, vTexCoord);\
						   float d = max(0, \
						   dot( normalize(normals.xyz), \
						   m_lightDirection ) ); \
						   FragColor = vec4(vec4(d,d,d,1) + tempColour);}";
	unsigned int tvertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(tvertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(tvertexShader);
	unsigned int tfragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(tfragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(tfragmentShader);
	m_tprogramID = glCreateProgram();
	glAttachShader(m_tprogramID, tvertexShader);
	glAttachShader(m_tprogramID, tfragmentShader);
	glLinkProgram(m_tprogramID);
	glDeleteShader(tvertexShader);
	glDeleteShader(tfragmentShader);

	// directional lighting
	vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec4 Normal; \
						   layout(location=2) in vec2 texcoord;\
						   out vec4 vNormal; \
						   out vec2 frag_texcoord; \
						   uniform mat4 ProjectionView; \
						   void main() { vNormal = Normal; \
						   frag_texcoord = texcoord; \
						   gl_Position = ProjectionView * Position;}";
	fsSource = "#version 410\n \
						   in vec4 vNormal; \
						   out vec4 FragColor; \
						   in vec2 frag_texcoord; \
						   uniform sampler2D diffuse; \
						   uniform vec3 m_lightDirection; \
						   void main() { \
						   vec4 tempColour = texture(diffuse, frag_texcoord);\
						   vec3 temp = m_lightDirection; \
						   float d = max(0, \
						   dot( normalize(vNormal.xyz), \
						   m_lightDirection ) ); \
						   FragColor = vec4(vec4(d,d,d,1) + tempColour);}";
	unsigned int fbxvertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(fbxvertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(fbxvertexShader);
	unsigned int fbxfragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fbxfragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fbxfragmentShader);
	m_fbxprogramID = glCreateProgram();
	glAttachShader(m_fbxprogramID, fbxvertexShader);
	glAttachShader(m_fbxprogramID, fbxfragmentShader);
	glLinkProgram(m_fbxprogramID);
	glDeleteShader(fbxvertexShader);
	glDeleteShader(fbxfragmentShader);

}

void Assesment::UpdateParticle()
{
	m_emitter->UpdateSelf(m_StartSize, m_EndSize, m_StartColour, m_EndColour);
	//m_emitter->initalise(1000, 5.0f, 20.0f, 10, 40, m_StartSize, m_EndSize, m_StartColour, m_EndColour);
}