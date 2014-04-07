#include "stdafx.h"
#include "ObjModel.h"

using namespace Models;
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace VertexTypes;

RigidBody::RigidBody()
{
	SetIdentity();
}

RigidObjModel::RigidObjModel(std::istream& stream)
	: ObjMesh(stream)
{
}

ObjMesh::ObjMesh(std::istream& stream)
{
	string tag;
	Vector3 vec3;
	Color color;
	XMUINT3 tri;

	std::vector<DirectX::SimpleMath::Vector3> VertexPostions;
	std::vector<DirectX::SimpleMath::Color> VertexColors;

	color.w = 1.0f;
	while (!stream.eof())
	{
		stream >> tag;

		if (tag == "v")
		{
			stream >> vec3.x >> vec3.y >> vec3.z;
			VertexPostions.push_back(vec3);
		}
		else if (tag == "vc")
		{
			stream >> color.x >> color.y >> color.z;
			VertexColors.emplace_back(color);
		}
		else if (tag == "f")
		{
			stream >> tri.x >> tri.y >> tri.z;
			tri.x--, tri.y--, tri.z--; // The obj file format starts the indices from 1, but as usual, it should starts from 0
			m_Facets.push_back(tri);
		}
	}
	auto N = VertexPostions.size();
	while (VertexColors.size() < N)
	{
		VertexColors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
	}
	for (size_t i = 0; i < N; i++)
	{
		m_Vertices.emplace_back(VertexPostions[i], VertexColors[i]);
	}

}


ObjMesh::~ObjMesh()
{
}
