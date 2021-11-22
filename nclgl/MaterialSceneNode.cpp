#include "MaterialSceneNode.h"
#include "MeshMaterial.h"

MaterialSceneNode::MaterialSceneNode(Shader *shader) : SceneNode(nullptr,	Vector4(1,1,1,1), shader)
{
	mesh = Mesh::LoadFromMeshFile("Role_T.msh");
	material = new MeshMaterial("Role_T.mat");
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry =
			material->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}
}

MaterialSceneNode::~MaterialSceneNode()
{
	delete material;
}

void MaterialSceneNode::Draw(OGLRenderer& r)
{
	PreDraw(r);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		mesh->DrawSubMesh(i);
	}
}
