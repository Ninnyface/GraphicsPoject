# pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/OBJMesh.h"

#define SHADOWSIZE 2048

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void ChangeScene(int);
	void MoveAlien();

protected:
	void DrawMagmaMap();
	void DrawSeaMap();
	void DrawSpaceMap();
	void DrawWater();
	void DrawMagma();
	void DrawSkybox();
	void DrawEntities();

	Shader * lightShader;
	Shader * reflectShader;
	Shader * skyboxShader;
	Shader * magmaShader;
	Shader * objShader;
	Shader * planetShader;
	Shader * sceneShader;
	Shader * shadowShader;

	HeightMap * heightMap;
	Mesh * quad;
	Mesh * skyBox;

	Light * light;
	Camera * camera;

	MD5FileData* hellData;
	MD5Node*	 hellNode;

	OBJMesh* beachBall;
	OBJMesh* sun;
	OBJMesh* planet;
	OBJMesh* planet2;

	Vector3* vertices;

	GLuint shadowFBO;
	GLuint shadowTex;
	GLuint magmaMap;
	GLuint waterMap;
	GLuint spaceMap;

	float rotate;
	int currentScene;
	bool moveAlien;	int moveCounter;};