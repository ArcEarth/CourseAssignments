#pragma once
#include <DirectXMath.h>
#include <vector>
#include <algorithm>
#include "VertexTypes.h"

class IVertexCollection
{
public:
	virtual size_t Count() const = 0;
	size_t Stride() const
	{
		return VertexDescription().Size();
	}
	virtual const VertexTypeDescription& VertexDescription() const = 0;
	virtual const void* BeginPtr() const = 0;
};

/// <summary>
/// The generic VertexCollection class , where TVertex is the VertexType, must impleament the static field "static VertexDescription Description;".
/// </summary>
template<typename TVertex>
class VertexCollection : public std::vector<TVertex>, public IVertexCollection
{
public:
	virtual const VertexTypeDescription& VertexDescription() const
	{
		return TVertex::Description;
	}
	virtual size_t Count() const
	{
		return this->size();
	}
	virtual const void* BeginPtr() const
	{
		return &this->front();
	}
};

namespace Models{

	//interface
	class ITriangleMesh
	{
	public:
		virtual const IVertexCollection& Vertices() const = 0;
		virtual const std::vector<DirectX::XMUINT3>& Facets() const = 0;
	};


	class ObjMesh : public ITriangleMesh
	{
	public:
		ObjMesh(std::istream& objFileStream);
		~ObjMesh();

		const std::string& Name() const;
		bool HasVertexNormal() const;
		bool HasVertexColor() const;

		//std::vector<DirectX::XMFLOAT3> VertexPostions;
		//std::vector<DirectX::XMFLOAT4> VertexColors;

		virtual const std::vector<DirectX::XMUINT3>& Facets() const{
			return m_Facets;
		}

		virtual const IVertexCollection& Vertices() const
		{
			return m_Vertices;
		}
	protected:
		VertexCollection<VertexTypes::VertexPositionColor> m_Vertices;
		std::vector<DirectX::XMUINT3> m_Facets;
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
			return XMMatrixAffineTransformation(Scale, DirectX::g_XMZero, Orientation, Position);
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

	class RigidObjModel : public ObjMesh, public RigidBody
	{
	public:
		RigidObjModel(std::istream& objFileStream);
	};

}