#include "renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();
	this->camera = new Camera();
	MatrixShader = new Shader("MatrixVertex.glsl", "colourFragment.glsl");

	if (!MatrixShader->LoadSuccess()) {
		return;
	}
	init = true;

	SwitchToPerspective();
}

Renderer::~Renderer(void) {
	delete triangle;
	delete MatrixShader;
	delete camera;
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT);
	BindShader(MatrixShader);
	
	glUniformMatrix4fv(glGetUniformLocation(MatrixShader -> GetProgram(), "projMatrix"), 1, false, projMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(MatrixShader -> GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);
	for (int i = 0; i < 3; ++i) {
		Vector3 tempPos = position;
		tempPos.z += (i * 200.0f);
		tempPos.x -= (i * 100.0f);
		tempPos.y -= (i * 100.0f);
		
		modelMatrix = Matrix4::Translation(tempPos) *
		Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
		Matrix4::Scale(Vector3(scale, scale, scale));

		glUniformMatrix4fv(glGetUniformLocation(MatrixShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
		triangle->Draw();
	}
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::SwitchToPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
	(float)width / (float)height, 45.0f);
}

void Renderer::SwitchToOrthographic() { 
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f, width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}