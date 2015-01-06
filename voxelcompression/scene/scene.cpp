#include "scene.hpp"
#include "model.hpp"
#include "camera/camera.hpp"

#include <glhelper/shaderobject.hpp>


Scene::Scene()
{
	Model::CreateVAO();
	m_simpleShader.reset(new gl::ShaderObject("simple draw"));
	m_simpleShader->AddShaderFromFile(gl::ShaderObject::ShaderType::VERTEX, "shader/simple.vert");
	m_simpleShader->AddShaderFromFile(gl::ShaderObject::ShaderType::FRAGMENT, "shader/simple.frag");
	m_simpleShader->CreateProgram();

	m_perFrameUniformBuffer.Init(*m_simpleShader, "PerFrame");
	m_perFrameUniformBuffer.BindBuffer(0);
}

Scene::~Scene()
{
	Model::DestroyVAO();
}

void Scene::AddModel(const std::string& filename)
{
	LOG_INFO("Loading " << filename << " ...");
	std::shared_ptr<Model> model = Model::FromFile(filename);
	if(model)
		models.push_back(model);
}

void Scene::Draw(Camera& camera)
{
	m_perFrameUniformBuffer.GetBuffer()->Map();
	m_perFrameUniformBuffer["ViewProjection"].Set(static_cast<const gl::Mat4&>(camera.ComputeViewProjection()));
	m_perFrameUniformBuffer.GetBuffer()->Unmap();

	m_simpleShader->Activate();
	Model::BindVAO();

	for (const std::shared_ptr<Model>& model : models)
	{
		model->BindBuffers();
		for (const Model::Mesh& mesh : model->GetMeshes())
		{
			if (mesh.diffuseTexture)
				mesh.diffuseTexture->Bind(0);
			GL_CALL(glDrawElements, GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, reinterpret_cast<const void*>(sizeof(std::uint32_t) * mesh.startIndex));
		}
	}
}