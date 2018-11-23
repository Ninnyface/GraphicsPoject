#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	currentScene = 0;
	camera = new Camera();
	heightMap = new HeightMap(TEXTUREDIR "terrain.raw", 0);
	quad = Mesh::GenerateQuad();
	skyBox = Mesh::GenerateQuad();
	beachBall = new OBJMesh(MESHDIR"texturedSphere.obj");
	sun = new OBJMesh(MESHDIR"texturedSphere.obj");
	planet = new OBJMesh(MESHDIR"texturedSphere.obj");
	planet2 = new OBJMesh(MESHDIR"texturedSphere.obj");
	vertices = new Vector3[RAW_HEIGHT*RAW_WIDTH];
	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	
	moveAlien = false;
	moveCounter = 0;

	hellNode = new MD5Node(*hellData);

	hellNode->SetTransform(Matrix4::Scale(Vector3(10, 10, 10)));
	hellData->AddAnim(MESHDIR"attack2.md5anim");
	hellData->AddAnim(MESHDIR"walk7.md5anim");
	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f,
		5000.0f, RAW_WIDTH * HEIGHTMAP_X));
	


	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f,
		(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		Vector4(0.9f, 0.9f, 1.0f, 1),
		(RAW_WIDTH * HEIGHTMAP_X));

	objShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR "PerPixelFragment.glsl");
	magmaShader = new Shader(SHADERDIR "PerPixelVertex.glsl",
		SHADERDIR "magmaFragment.glsl");
	reflectShader = new Shader(SHADERDIR "PerPixelVertex.glsl",
		SHADERDIR "reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR "skyboxVertex.glsl",
		SHADERDIR "skyboxFragment.glsl");
	lightShader = new Shader(SHADERDIR "BumpVertex.glsl",
		SHADERDIR "BumpFragment.glsl");
	planetShader = new Shader(SHADERDIR "TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");
	sceneShader = new Shader(SHADERDIR "shadowscenevert.glsl",
		SHADERDIR "shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR "shadowVert.glsl",
		SHADERDIR "shadowFrag.glsl");

	if (!sceneShader->LinkProgram() || !shadowShader->LinkProgram() || 
		!planetShader->LinkProgram() || !objShader->LinkProgram() || 
		!magmaShader->LinkProgram() || !reflectShader->LinkProgram() || 
		!lightShader->LinkProgram() || !skyboxShader->LinkProgram()) {
		return;

	}

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	beachBall->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "ball.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	planet->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	planet->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	planet2->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	planet2->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "waterbump.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	sun->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "sun.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "magma.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR "lavarock.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	magmaMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "hell_ft.tga", TEXTUREDIR "hell_bk.tga",
		TEXTUREDIR "hell_up.tga", TEXTUREDIR "hell_dn.tga",
		TEXTUREDIR "hell_rt.tga", TEXTUREDIR "hell_lf.tga",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	waterMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "lagoon_ft.tga", TEXTUREDIR "lagoon_bk.tga",
		TEXTUREDIR "lagoon_up.tga", TEXTUREDIR "lagoon_dn.tga",
		TEXTUREDIR "lagoon_rt.tga", TEXTUREDIR "lagoon_lf.tga",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	spaceMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "drakeq_ft.tga", TEXTUREDIR "drakeq_bk.tga",
		TEXTUREDIR "drakeq_up.tga", TEXTUREDIR "drakeq_dn.tga",
		TEXTUREDIR "drakeq_rt.tga", TEXTUREDIR "drakeq_lf.tga",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	if (!magmaMap || !waterMap || !quad->GetTexture() || !heightMap->GetTexture() ||
		!heightMap->GetBumpMap()) {
		return;
	}

	SetTextureRepeating(beachBall->GetTexture(), true);
	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	init = true;
	rotate = 0.0f;
	

	projMatrix = Matrix4::Perspective(1.0f, 150000.0f,
		(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

}

Renderer ::~Renderer(void) {
	delete camera;
	delete hellData;
	delete hellNode;
	delete heightMap;
	delete quad;
	delete skyBox;
	delete beachBall;
	delete planet;
	delete planet2;
	delete sun;
	delete reflectShader;
	delete skyboxShader;
	delete magmaShader;
	delete lightShader;
	delete objShader;
	delete shadowShader;
	delete sceneShader;
	delete planetShader;
	delete light;
	currentShader = 0;
}

void Renderer::UpdateScene(float msec) {

	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	hellNode->Update(msec);
	rotate += msec / 2000.0f;
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox();

	//magma
	if (currentScene == 0) {
		//DrawMagmaMap();
		//DrawMagma();
		DrawShadowScene();
		DrawCombinedScene();
	}
	//if sea
	else if (currentScene == 1) {
		//DrawSeaMap();
		heightMap->UpdateMap();
		//DrawEntities();
		DrawShadowScene();
		DrawCombinedScene();
	}
	//if space
	else {
		//DrawSpaceMap();
		//DrawEntities();
		SetCurrentShader(planetShader);
		modelMatrix = Matrix4::Translation(Vector3(vertices[33025]))
			* Matrix4::Rotation(rotate * 4, Vector3(1, 0, 0))
			* Matrix4::Translation(Vector3(0, 50000, 0))
			* Matrix4::Rotation(rotate * 4, Vector3(-1, 0, 0))
			* Matrix4::Rotation(rotate * 10, Vector3(0, 1, 0))
			* Matrix4::Scale(Vector3(500, 500, 500));
		sunPos = modelMatrix.GetPositionVector();
		light->SetPosition(sunPos);
		UpdateShaderMatrices();
		sun->Draw();
		DrawShadowScene();
		DrawCombinedScene();
	}
	
	

	SwapBuffers();
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), 
		vertices[37130]);
	textureMatrix = biasMatrix * (projMatrix*viewMatrix);
	UpdateShaderMatrices();
	if (currentScene == 0) {
		DrawMagmaMap();
		DrawMagma();
		DrawEntities();
	}
	else if (currentScene == 1) {
		DrawSeaMap();
		DrawEntities();
	}
	else {
		DrawSpaceMap();
		DrawEntities();
	}
	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),
		"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(),
		"shadowTex"), 2);
	glUniform3fv(glGetUniformLocation(currentShader -> GetProgram(),
		"cameraPos"), 1, (float *)& camera -> GetPosition());
	
	SetShaderLight(*light);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	
	viewMatrix = camera -> BuildViewMatrix();
	UpdateShaderMatrices();
	
	if (currentScene == 0) {
		DrawMagmaMap();
		DrawMagma();
		DrawEntities();
	}
	else if (currentScene == 1) {
		DrawSeaMap();
		DrawEntities();
	}
	else {
		DrawSpaceMap();
		DrawEntities();
	}

	glUseProgram(0);
}

void Renderer::DrawEntities() {
	
	if (currentScene == 0) {	
		/*SetShaderLight(*light);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
			"cameraPos"), 1, (float *)& camera->GetPosition());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"diffuseTex"), 0);	*/

		modelMatrix = Matrix4::Translation(Vector3(vertices[33025]))
			* Matrix4::Rotation(rotate * 30, Vector3(0, 1, 0))
			* Matrix4::Translation(Vector3(0, 1000, (RAW_HEIGHT * HEIGHTMAP_Z)));
		light->SetPosition(modelMatrix.GetPositionVector());

		if (moveAlien) {
			moveCounter += 3;
			modelMatrix = Matrix4::Translation(Vector3(vertices[(moveCounter / 277) * 8]))* Matrix4::Rotation(90, Vector3(0, 1, 0));
		}
		else {
			//modelMatrix = Matrix4::Translation(Vector3(vertices[33025]));


			modelMatrix = Matrix4::Translation(Vector3(vertices[37130])) * Matrix4::Rotation(40, Vector3(0, 1, 0));
		}
		//UpdateShaderMatrices();
		Matrix4 tempMatrix = textureMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "textureMatrix"), 1, false, *& tempMatrix.values);

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "modelMatrix"), 1, false, *& modelMatrix.values);

		hellNode->Draw(*this);
	}
	else if (currentScene == 1) {
		modelMatrix = Matrix4::Translation(Vector3(vertices[33025]))
			* Matrix4::Rotation(rotate * 30, Vector3(0, 1, 0))
			* Matrix4::Translation(Vector3(4000, 200, 0));
		light->SetPosition(modelMatrix.GetPositionVector());
		
		for (int i = 22; i <= 42; i += 10) {
			modelMatrix = Matrix4::Translation(Vector3(vertices[i*1000].x, vertices[i*1000].y + 50, vertices[i*1000].z)) * Matrix4::Rotation(10*i + (rotate * 10), Vector3(1, 1, 0)) * Matrix4::Scale(Vector3(10, 10, 10));
			
			Matrix4 tempMatrix = textureMatrix * modelMatrix;
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
				, "textureMatrix"), 1, false, *& tempMatrix.values);

			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
				, "modelMatrix"), 1, false, *& modelMatrix.values);

			//UpdateShaderMatrices();
			beachBall->Draw();
		}
	}
	else {
		

		modelMatrix = Matrix4::Translation(Vector3(sunPos))
			* Matrix4::Rotation(rotate*40, Vector3(1,0,0))
			* Matrix4::Translation(Vector3(0, -25000, 0))
			* Matrix4::Rotation(rotate * 40, Vector3(-1, 0, 0))
			* Matrix4::Rotation(rotate * 20, Vector3(0, 1, 0))
			* Matrix4::Scale(Vector3(100, 100, 100));
		
		Matrix4 tempMatrix = textureMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "textureMatrix"), 1, false, *& tempMatrix.values);

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "modelMatrix"), 1, false, *& modelMatrix.values);

		//UpdateShaderMatrices();
		
		planet->Draw();

		modelMatrix = Matrix4::Translation(Vector3(sunPos))	
			* Matrix4::Rotation(rotate * 30, Vector3(1, 0, -1))
			* Matrix4::Translation(Vector3(0, -35000, 0))
			* Matrix4::Scale(Vector3(100, 100, 100));

		tempMatrix = textureMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "textureMatrix"), 1, false, *& tempMatrix.values);

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "modelMatrix"), 1, false, *& modelMatrix.values);

		planet2->Draw();

	}
	glUseProgram(0);
}

void Renderer::MoveAlien() {
	moveAlien = !moveAlien;
	if (moveAlien) {
		hellNode->PlayAnim(MESHDIR"walk7.md5anim");
	}
	else {
		hellNode->PlayAnim(MESHDIR"idle2.md5anim");
	}
	moveCounter = 0;
}

void Renderer::DrawSkybox() {

	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	if (currentScene == 0) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, magmaMap);
	}
	else if (currentScene == 1) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, waterMap);
	}
	else {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, spaceMap);
	}
	
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"cubeTex"), 2);

	skyBox->Draw();

	glUseProgram(0);

	glDepthMask(GL_TRUE);

}

void Renderer::DrawSpaceMap() {
	
	modelMatrix.ToIdentity();

	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

	heightMap->Draw();
	

	
}

void Renderer::DrawSeaMap() {
	/*SetCurrentShader(lightShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);
	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"cubeTex"), 2);

	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, waterMap);
	
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);*/

	modelMatrix.ToIdentity();
	
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

	heightMap->Draw();

}

void Renderer::DrawMagmaMap() {
	/*SetCurrentShader(lightShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);*/
	modelMatrix.ToIdentity();
	//textureMatrix.ToIdentity();
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

	heightMap->Draw();
}

void Renderer::DrawMagma() {
	
	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 200 * HEIGHTMAP_Y / 3.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);
	
	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	
	//textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
	//	Matrix4::Rotation(rotate, Vector3(0.0f, 0.0f, 1.0f));

	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& modelMatrix.values);

	quad->Draw();

}

void Renderer::ChangeScene(int x) {
	currentScene += x;
	if (currentScene < 0) {
		currentScene = 2;
	}
	else if (currentScene > 2) {
		currentScene = 0;
	}
	if (currentScene == 0) {

		delete heightMap;
		heightMap = new HeightMap(TEXTUREDIR "terrain.raw", 0);

		delete light;
		light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 1000.0f,
			(RAW_HEIGHT * HEIGHTMAP_Z)*1.5f),
			Vector4(0.9f, 0.9f, 1.0f, 1),
			(RAW_WIDTH * HEIGHTMAP_X)*4);

		vertices = heightMap->getVertices();

		heightMap->SetTexture(SOIL_load_OGL_texture(
			TEXTUREDIR "lavarock.JPG", SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

		heightMap->SetBumpMap(SOIL_load_OGL_texture(
			TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
		if (!heightMap->GetTexture() ||
			!heightMap->GetBumpMap()) {
			return;

		}

		SetTextureRepeating(heightMap->GetTexture(), true);
		SetTextureRepeating(heightMap->GetBumpMap(), true);
	}
	else if (currentScene == 1) {

		delete heightMap;
		heightMap = new HeightMap(TEXTUREDIR "terrain.raw", 1);

		delete light;
		light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 2000.0f,
			(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
			Vector4(0.9f, 0.9f, 1.0f, 1),
			(RAW_WIDTH * HEIGHTMAP_X)*1.5f);


		vertices = heightMap->getVertices();

		heightMap->SetTexture(SOIL_load_OGL_texture(
			TEXTUREDIR "water.jpg", SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

		heightMap->SetBumpMap(SOIL_load_OGL_texture(
			TEXTUREDIR "waterbump.JPG", SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

		if (!heightMap->GetTexture() || !quad->GetTexture() ||
			!heightMap->GetBumpMap()) {
			return;

		}
	
		SetTextureRepeating(heightMap->GetTexture(), true);
		SetTextureRepeating(heightMap->GetBumpMap(), true);
	}
	else {

		delete heightMap;
		heightMap = new HeightMap(TEXTUREDIR "terrain.raw", 2);
		
		delete light;
		light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 5000.0f,
			(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)+5000),
			Vector4(0.9f, 0.9f, 1.0f, 1),
			100000);


		vertices = heightMap->getVertices();

		heightMap->SetTexture(SOIL_load_OGL_texture(
			TEXTUREDIR "moon.jpg", SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

		heightMap->SetBumpMap(SOIL_load_OGL_texture(
			TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		
		if (!heightMap->GetTexture() ||
			!heightMap->GetBumpMap()) {
			return;

		}
		
		SetTextureRepeating(heightMap->GetTexture(), true);
		SetTextureRepeating(heightMap->GetBumpMap(), true);
	}
}