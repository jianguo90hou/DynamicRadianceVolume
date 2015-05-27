#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ei/vector.hpp>
#include <ei/3dtypes.hpp>

#include <glhelper/gl.hpp>
#include <glhelper/texture2d.hpp>

namespace gl
{
	class Buffer;
	class VertexArrayObject;
}

class Model
{
public:
	/// Loads a model from a given filename.
	/// \return nullptr if the path is invalid or the file could not be loaded.
	static std::shared_ptr<Model> FromFile(const std::string& filename);

	~Model();

	const std::string& GetOriginFilename() { return m_originFilename; }

	struct Mesh
	{
		Mesh() : startIndex(0), numIndices(0) {}
		
		unsigned int startIndex;
		unsigned int numIndices;

		// All textures are always present. If only a single value is available, a 1x1 texture is generated at loading time.
		std::shared_ptr<gl::Texture2D> diffuse;
		std::shared_ptr<gl::Texture2D> normalmap;	// Tangent space normals RGB -> XZY*2.0 - 1.0
		std::shared_ptr<gl::Texture2D> roughnessMetalic; // Combined texture of roughness (R) and metallic values (G)
	};

	struct Vertex
	{
		ei::Vec3 position;
		ei::Vec3 normal;
		ei::Vec4 tangent; // The 4th component determines the handedness of the bitangent
		ei::Vec2 texcoord;
	};

	unsigned int GetNumTriangles() const { return m_numTriangles; }
	unsigned int GetNumVertices() const { return m_numVertices; }
	const std::vector<Mesh>& GetMeshes() const { return m_meshes; }

	const ei::Box& GetBoundingBox() const { return m_boundingBox; }

	static void CreateVAO();
	static void DestroyVAO();
	static void BindVAO();

	/// Binds vertex and index buffer.
	void BindBuffers();

private:
	Model(const std::string& originFilename);

	static std::unique_ptr<gl::VertexArrayObject> m_vertexArrayObject;

	const std::string m_originFilename;

	std::vector<Mesh> m_meshes;

	unsigned int m_numTriangles;
	unsigned int m_numVertices;

	std::unique_ptr<gl::Buffer> m_vertexBuffer;
	std::unique_ptr<gl::Buffer> m_indexBuffer;

	ei::Box m_boundingBox;
};
