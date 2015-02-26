//#ifndef ANIMATION_H_
//#define ANIMATION_H_
//#include "Application.h"
//#include "Camera.h"
//#include "FBXFile.h"
//#include "OpenGLData.h"
//
//class Animation : public Application
//{
//public:
//
//	virtual bool startup();
//	virtual void shutdown();
//	virtual bool update();
//	virtual void draw();
//
//	void GenerateGLMeshes(FBXFile* fbx);
//	void EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skeleton, float timer);
//	void UpdateBones(FBXSkeleton* skeleton);
//
//	FlyCamera camera;
//	std::vector<GLData> m_meshs;
//	FBXFile* m_file;
//
//	unsigned int m_program;
//};
//
//#endif //ANIMATION_H_