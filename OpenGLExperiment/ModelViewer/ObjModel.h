#pragma once
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <gl\glew.h>
#include "Texture.h"
#include "VertexTypes.h"

//class IVertexCollection
//{
//public:
//	virtual size_t Count() const = 0;
//	size_t Stride() const
//	{
//		return VertexDescription().Size();
//	}
//	virtual const VertexTypeDescription& VertexDescription() const = 0;
//	virtual const void* BeginPtr() const = 0;
//};

class IVertexCollection
{
public:
	virtual ~IVertexCollection() = 0;
	virtual size_t Count() const = 0;
	virtual const VertexTypeDescription& VertexDescription() const = 0;
	virtual const void* GetAttributeData(const std::string& attributeName) const = 0;
};


class BasicVertexCollection : public IVertexCollection
{
public:
	virtual size_t Count() const
	{
		return Postions.size();
	}

	virtual ~BasicVertexCollection();

	virtual const VertexTypeDescription& VertexDescription() const;

	virtual const void* GetAttributeData(const std::string& attributeName) const
	{
		if (Count() == 0) return nullptr;
		if (attributeName == "Postion") return &Postions[0];
		if (attributeName == "TextureCoordinate" && HasTexCoord()) return &TexCoords[0];
		if (attributeName == "Color" && HasColor()) return &Colors[0];
		if (attributeName == "Normal" && HasNormal()) return &Normals[0];
		return nullptr;
	}

	void CreateVertexAttributeBuffers();

	bool HasNormal() const { return !Normals.empty(); }
	bool HasColor() const{ return !Colors.empty(); }
	bool HasTexCoord() const{ return !TexCoords.empty(); }

	GLuint GetVertexBuffer() const{
		return m_VertexBuffer;
	}
	GLuint GetTexCoordBuffer() const{
		return m_TexCoordBuffer;
	}
	GLuint GetNormalBuffer() const{
		return m_NormalBuffer;
	}
	GLuint GetColorBuffer() const{
		return m_ColorBuffer;
	}

	std::vector<DirectX::SimpleMath::Vector3>	Postions;
	std::vector<DirectX::SimpleMath::Color>		Colors;
	std::vector<DirectX::SimpleMath::Vector2>	TexCoords;
	std::vector<DirectX::SimpleMath::Vector3>	Normals;

private:
	GLuint	m_VertexBuffer, m_TexCoordBuffer, m_ColorBuffer, m_NormalBuffer;
};

/// <summary>
/// The generic VertexCollection class , where TVertex is the VertexType, must impleament the static field "static VertexDescription Description;".
/// </summary>
//class VertexCollection : public IVertexCollection
//{
//public:
//	VertexCollection(const VertexTypeDescription& description);
//
//	virtual const VertexTypeDescription& VertexDescription() const
//	{
//		return TVertex::Description;
//	}
//	virtual size_t Count() const
//	{
//		return this->size();
//	}
//	virtual const void* BeginPtr() const
//	{
//		return &this->front();
//	}
//private:
//	VertexTypeDescription m_VertexDesciption;
//};

namespace Models{

	//interface
	class ITriangleMesh
	{
	public:
		virtual ~ITriangleMesh() = 0;
		virtual const DirectX::BoundingBox& MeshBoundingBox() const = 0;
		virtual const IVertexCollection& Vertices() const = 0;
		virtual const std::vector<DirectX::XMUINT3>& Facets() const = 0;
		virtual GLuint GetElementBuffer() const = 0;
	};

	class ObjMesh : public ITriangleMesh
	{
	public:
		ObjMesh(std::istream& objFileStream);
		~ObjMesh();

		bool HasVertexNormal() const
		{
			return m_Vertices.HasNormal();
		}

		bool HasVertexColor() const
		{
			return m_Vertices.HasColor();
		}

		bool HasVertexTextureCoordinate() const
		{
			return m_Vertices.HasTexCoord();
		}

		void GenerateVertexNormal();

		virtual const IVertexCollection& Vertices() const
		{
			return m_Vertices;
		}
		virtual const std::vector<DirectX::XMUINT3>& Facets() const
		{
			return m_Facets;
		};
		virtual const DirectX::BoundingBox& MeshBoundingBox() const
		{
			return m_BoundingBox;
		}
		virtual GLuint GetElementBuffer() const
		{
			return m_ElementBuffer;
		}

		void CreateElementBuffer()
		{
			glGenBuffers(1, &m_ElementBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Facets.size() * sizeof(DirectX::XMUINT3), &m_Facets[0].x, GL_STATIC_DRAW);
		}
	protected:
		DirectX::BoundingBox						m_BoundingBox;
		BasicVertexCollection						m_Vertices;
		std::vector<DirectX::XMUINT3>				m_Facets;
		GLuint										m_ElementBuffer;
	};

	struct RigidBody
	{
	public:
		RigidBody();
		DirectX::SimpleMath::Vector3	Position;
		DirectX::SimpleMath::Vector3	Scale;
		DirectX::SimpleMath::Quaternion Orientation;

		void SetIdentity()
		{
			Position = (DirectX::XMVECTOR) DirectX::g_XMZero;
			Orientation = DirectX::XMQuaternionIdentity();
			Scale = (DirectX::XMVECTOR) DirectX::g_XMOne;
		}

		DirectX::XMMATRIX GetModelMatrix() const
		{
			return DirectX::XMMatrixAffineTransformation(Scale, DirectX::g_XMZero, Orientation, Position);
		}
		inline void Translate(DirectX::XMVECTOR vTranslation)
		{
			Position += vTranslation;
		}
		inline void Rotate(DirectX::XMVECTOR qRotation)
		{
			Orientation *= qRotation;
		}
	};

	class Camera : public RigidBody
	{
	public:
		Camera()
		{
			using namespace DirectX;
			FOV = 75.0f;		// Typical value = 75 for human
			Aspect = 1.0f;
			Near = 0.1f;		// Typical value = 0.1f
			Far = 100.0f;		// Typical value = 100.0f
			Orientation = XMQuaternionIdentity();
			//Orientation = XMQuaternionRotationRollPitchYaw(0, XM_PI, 0);	// LookTo negetive Z direction
		}
		DirectX::XMMATRIX GetViewMatrix() const
		{
			using namespace DirectX::SimpleMath;
			auto foward = GetForward();
			auto up = XMVector3Rotate(Vector3(0, -1.0f, 0), Orientation);
			return DirectX::XMMatrixLookToRH(Position, foward, up);
		}

		DirectX::XMVECTOR GetForward() const
		{
			using namespace DirectX::SimpleMath;
			return XMVector3Rotate(Vector3(0, 0, -1.0f), Orientation);
		}

		DirectX::XMMATRIX GetProjectMatrix() const
		{
			return DirectX::XMMatrixPerspectiveFovRH(FOV, Aspect, Near, Far);
		}

		void Move(DirectX::SimpleMath::Vector3 displacement)
		{
			Position += DirectX::XMVector3Rotate(displacement, Orientation);
		}

		void LookAt(DirectX::SimpleMath::Vector3 focusPosition);
		void LookTo(DirectX::SimpleMath::Vector3 direction)
		{

		}

		float FOV;		// Typical value = 75 for human
		float Aspect;	// Width / Height
		float Near;		// Typical value = 0.1f
		float Far;		// Typical value = 100.0f
	};

	class RigidModel : public RigidBody
	{
	public:
		RigidModel(const char* modelFileName, const char *textureFileName = nullptr);
		RigidModel(const std::shared_ptr<GL::Texture2D> &pTexture, const std::shared_ptr<ITriangleMesh> &pMesh)
			: m_pMesh(pMesh), m_pTexture(pTexture)
		{}

		const ITriangleMesh* Mesh() const
		{
			return m_pMesh.get();
		}
		bool HasColorTexture() const
		{
			return m_pTexture != nullptr;
		}
		const GL::Texture2D* ColorTexture() const
		{
			return m_pTexture.get();
		}

		DirectX::BoundingOrientedBox GetOrientedBoundingBox() const
		{
			using namespace DirectX::SimpleMath;
			const auto& box = m_pMesh->MeshBoundingBox();
			return DirectX::BoundingOrientedBox(Position + *(Vector3*)(&box.Center), box.Extents, Orientation);
		}

	private:
		std::shared_ptr<GL::Texture2D> m_pTexture;
		std::shared_ptr<ITriangleMesh> m_pMesh;
	};

	class RigidObjModel : public ObjMesh, public RigidBody
	{
	public:
		RigidObjModel(std::istream& objFileStream);
	};

}