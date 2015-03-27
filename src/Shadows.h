//#ifndef SHADOWS_H_
//#define SHADOWS_H_
//#include "Application.h"
//#include "Camera.h"
//#include <vector>
//#include "tiny_obj_loader.h"
//#include "FBXFile.h"
//
//class Shadows : public Application
//{
//public:
//
//	virtual bool startup();
//	virtual void shutdown();
//	virtual bool update();
//	virtual void draw();
//
//	FlyCamera camera;
//	vec4 m_background_color;
//	float timer = 0.0f;
//	unsigned int m_program;
//
//	void createDrawShader();
//	unsigned int loadShader(unsigned int type, const char* path);
//	//void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
//	//
//	//struct OpenGLInfo
//	//{
//	//	unsigned int m_VAO;
//	//	unsigned int m_VBO;
//	//	unsigned int m_IBO;
//	//	unsigned int m_index_count;
//	//};
//	//std::vector<OpenGLInfo> m_gl_info;
//
//	bool load(const char* a_filename, FBXFile::UNIT_SCALE a_scale = FBXFile::UNITS_METER, bool a_loadTextures = true, bool a_loadAnimations = true, bool a_flipTextureY = true);
//	unsigned int getMeshCount() const;
//	FBXMeshNode* getMeshByIndex(unsigned int a_index) const;
//	unsigned int m_vertexAttributes;
//	FBXMaterial* m_material;
//	std::vector<FBXVertex> m_vertices;
//	std::vector<unsigned int> m_indices;
//	glm::mat4 m_localTransform;
//	glm::mat4 m_globalTransform;
//	void* m_userData;//	void createOpenGLBuffers(FBXFile* fbx);
//	void cleanupOpenGLBuffers(FBXFile* fbx);
//	FBXFile* m_fbx;
//
//	unsigned int m_useShadowProgram;
//	unsigned int m_shadowGenProgram;
//	unsigned int m_fbo;
//	unsigned int m_fboDepth;
//	unsigned int m_vao;
//	vec3 m_lightDirection;
//	mat4 m_lightMatrix;
//	void createShadowShader();
//};
//
//#endif //SHADOWS_H_