#ifndef PROCEDURALGENERATION_H_
#define PROCEDURALGENERATION_H_
#include "Application.h"
#include "Camera.h"
#include <vector>
#include "tiny_obj_loader.h"
#include "FBXFile.h"
#include "Vertex2.h"

class ProceduralGeneration : public Application
{
public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();
	OpenGLData m_plane_mesh;
	void buildGrid(vec2 real_dims, glm::ivec2 dims);
	void buildPerlinTexture();
	float* m_perlin_data;

	FlyCamera camera;
	vec4 m_background_color;
	float timer = 0.0f;
	unsigned int m_program;

	void createDrawShader();
	unsigned int loadShader(unsigned int type, const char* path);
	unsigned int m_perlin_texture;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
};

#endif //PROCEDURALGENERATION_H_