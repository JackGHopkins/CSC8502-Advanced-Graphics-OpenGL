#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	automaticCamera = false;

	quad = Mesh::GenerateQuad();
	mesh = Mesh::LoadFromMeshFile("Role_T.msh");
	cube = Mesh::LoadFromMeshFile("Cube.msh");
	material = new MeshMaterial("Role_T.mat");
	heightMap = new HeightMap(TEXTUREDIR"noise.png");

	texture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	normalMap = SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	waterTex = SOIL_load_OGL_texture(
		TEXTUREDIR"cartoonWater.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthTex = SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthBump = SOIL_load_OGL_texture(
		TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"skybox_west.jpg", TEXTUREDIR"skybox_east.jpg",
		TEXTUREDIR"skybox_up.jpg", TEXTUREDIR"skybox_down.jpg",
		TEXTUREDIR"skybox_south.jpg", TEXTUREDIR"skybox_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!earthTex || !earthBump || !cubeMap || !waterTex) {
		return;
	}

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	SetTextureRepeating(waterTex, true);

	shader = new Shader("bumpvertex.glsl", "bumpfragment.glsl");
	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	texShader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");

	if (!shader->LoadSuccess() || !texture || !normalMap) {
		return;
	}

	if (!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess() || !texShader->LoadSuccess()) {
		return;
	}

	SetTextureRepeating(texture, true);
	SetTextureRepeating(normalMap, true);

	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}

	this->root = new SceneNode();
	root->AddChild(new SceneNode(mesh));

	Vector3 heightMapSize = heightMap->GetHeightmapSize();
	camera = new Camera(0, 0, Vector3(0,0,0));
	light = new Light(heightMapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), heightMapSize.x * 0.5f);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;;
	init = true;
}
Renderer::~Renderer(void) {
	delete root;
	delete light;
	delete heightMap;
	delete quad;
	delete camera;
	delete shader;
	delete lightShader;
	delete reflectShader;
	delete skyboxShader;
	glDeleteTextures(1, &texture);
}

void Renderer::UpdateScene(float dt) {
	if (!automaticCamera) {
		camera->UpdateCamera(dt);
	}
	else {
		camera->UpdateAutomaticCamera(dt);
	}
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(dt);
}


void Renderer::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(shader);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	UpdateShaderMatrices();
	SetShaderLight(*light);


	DrawSkybox();
	DrawHeightMap();
	DrawWater();
	DrawNodes();
	DrawCube();
}

 void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();	
}


 void Renderer::BuildNodeLists(SceneNode* from) {
	 if (frameFrustum.InsideFrustum(*from)) {
		 Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		 from->SetCameraDistance(Vector3::Dot(dir, dir));
		 if (from->GetColour().w < 1.0f)
			 transparentNodeList.push_back(from);
		 else
			 nodeList.push_back(from);
	 }
	 for (vector < SceneNode* >::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		 BuildNodeLists((*i));
	 }
 }

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance); 	//Sorts transparents into REVERSE order.
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance); //Sorts based on distance to camera
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList)
		DrawNodes(i);

	for (const auto& i : transparentNodeList)
		DrawNodes(i);
}

void Renderer::DrawNodes(SceneNode* n) {
	if (n->GetMesh()) {
		//Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		//glUniformMatrix4fv(
		//glGetUniformLocation(shader->GetProgram(),"modelMatrix"), 1, false, model.values);
		//
		//glUniform4fv(glGetUniformLocation(shader->GetProgram(),"nodeColour"), 1, (float*)& n->GetColour());
		//
		//texture = n-> GetTexture();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture);
	
		//glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"), texture);

		BindShader(lightShader);
		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);

		Vector3 hSize = Vector3(1000, 1000, 1000);

		modelMatrix =
			Matrix4::Translation(hSize * 0.5f) *
			Matrix4::Scale(hSize * 0.5f) *
			Matrix4::Rotation(90, Vector3(1, 0, 0));

		textureMatrix =
			Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
			Matrix4::Scale(Vector3(10, 10, 10)) *
			Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

		UpdateShaderMatrices();
		SetShaderLight(*light); //No lighting in this shader !
	

		UpdateShaderMatrices();

		vector<Matrix4> frameMatrices;

		int j = glGetUniformLocation(shader->GetProgram(), "joints");
		glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

		for (int i = 0; i < mesh->GetSubMeshCount(); i++) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, matTextures[i]);
			mesh->DrawSubMesh(i);
		}
		//n->Draw(*this);
	}
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	BindShader(skyboxShader);
	UpdateShaderMatrices();
	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightMap() {
	BindShader(lightShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();
	heightMap->Draw();
}

void Renderer::DrawWater() {
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = heightMap->GetHeightmapSize();

	modelMatrix =
		Matrix4::Translation(hSize * 0.5f) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	SetShaderLight(*light); //No lighting in this shader !
	quad->Draw();
}
void Renderer::DrawCube() {
	BindShader(lightShader);

	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = Vector3(5000, 5000, 5000);

	modelMatrix =
		Matrix4::Translation(hSize * 0.5f) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	SetShaderLight(*light); //No lighting in this shader !
	cube->Draw();
}

void Renderer::ToggleAutomaticCamera() {
	automaticCamera = !automaticCamera;
	if (automaticCamera) {
		camera->SetPitch(camera->cameraAngles[camera->index].pitch);
		camera->SetYaw(camera->cameraAngles[camera->index].yaw);
	}
}

void Renderer::PrintCoordinates() {
	std::cout << "X: " << camera->GetPosition().x << std::endl;
	std::cout << "Y: " << camera->GetPosition().y << std::endl;
	std::cout << "Z: " << camera->GetPosition().z << std::endl;
	std::cout << "Pitch: " << camera->GetPitch() << std::endl;
	std::cout << "Yaw: " << camera->GetYaw() << std::endl << std::endl;
}


