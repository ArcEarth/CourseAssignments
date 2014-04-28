#include "ObjModel.h"
#include <cassert>
#include <fstream>

using namespace Models;
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace VertexTypes;

IVertexCollection::~IVertexCollection()
{
}
ITriangleMesh::~ITriangleMesh()
{
}


void BasicVertexCollection::CreateVertexAttributeBuffers()
{
	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, Postions.size()*sizeof(Vector3), &Postions[0], GL_STATIC_DRAW);

	if (HasColor())
	{
		glGenBuffers(1, &m_ColorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
		glBufferData(GL_ARRAY_BUFFER, Colors.size()*sizeof(Color), &Colors[0], GL_STATIC_DRAW);
	}
	if (HasTexCoord())
	{
		glGenBuffers(1, &m_TexCoordBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBuffer);
		glBufferData(GL_ARRAY_BUFFER, TexCoords.size()*sizeof(Vector2), &TexCoords[0], GL_STATIC_DRAW);
	}
	if (HasNormal())
	{
		glGenBuffers(1, &m_NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, Normals.size()*sizeof(Vector2), &Normals[0], GL_STATIC_DRAW);
	}
}

BasicVertexCollection::~BasicVertexCollection()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (m_VertexBuffer)
	{
		glDeleteBuffers(1, &m_VertexBuffer);
	}
	if (m_ColorBuffer)
	{
		glDeleteBuffers(1, &m_ColorBuffer);
	}
	if (m_TexCoordBuffer)
	{
		glDeleteBuffers(1, &m_TexCoordBuffer);
	}
	if (m_NormalBuffer)
	{
		glDeleteBuffers(1, &m_NormalBuffer);
	}
}

const VertexTypeDescription& BasicVertexCollection::VertexDescription() const
{
	unsigned int flag = (unsigned) HasColor() | (unsigned) HasNormal() << 1 | (unsigned) HasTexCoord() << 2;
	switch (flag)
	{
	case 0:
		return VertexTypes::VertexBasic::Description;
	case 1:
		return VertexTypes::VertexPositionColor::Description;
	case 2:
		return VertexTypes::VertexPostionNormal::Description;
	case 3:
		return VertexTypes::VertexPositionNormalColor::Description;
	case 4:
		return VertexTypes::VertexPostionTexture::Description;
	case 6:
		return VertexTypes::VertexPostionNormalTexture::Description;
	case 5:
	case 7:
	default:
		throw std::runtime_error("Unsupported vertex format");
	}
}

RigidModel::RigidModel(const char* modelFileName, const char *textureFileName)
{
	ifstream model(modelFileName);
	if (!model.is_open())
		return;
	m_pMesh = make_shared<ObjMesh>(model);

	if (textureFileName)
	{
		m_pTexture = make_shared<GL::Texture2D>(textureFileName);
	}
}


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
	Vector2 vec2;
	Color color;
	XMUINT3 tri;

	color.w = 1.0f;
	while (!stream.eof())
	{
		stream >> tag;

		if (tag == "v")
		{
			stream >> vec3.x >> vec3.y >> vec3.z;
			m_Vertices.Postions.push_back(vec3);
		}
		else if (tag == "vc") // This is not inside the standard
		{
			stream >> color.x >> color.y >> color.z;
			m_Vertices.Colors.emplace_back(color);
		}
		else if (tag == "vt")
		{
			stream >> vec2.x >> vec2.y;
			m_Vertices.TexCoords.push_back(vec2);
		}
		else if (tag == "vn")
		{
			stream >> vec3.x >> vec3.y >> vec3.z;
			m_Vertices.Normals.push_back(vec3);
		}
		else if (tag == "vp")
		{
			stream >> vec3.x >> vec3.y >> vec3.z; //Ignore the unknow parameter data
			//VertexParameters.push_back(vec3);
		}
		else if (tag == "f")
		{
			stream >> tri.x >> tri.y >> tri.z;
			tri.x--, tri.y--, tri.z--; // The obj file format starts the indices from 1, but as usual, it should starts from 0
			m_Facets.push_back(tri);
		}
	}
	auto N = m_Vertices.Count();
	assert(m_Vertices.TexCoords.size() == 0 || N == m_Vertices.TexCoords.size());
	//assert(m_Vertices.Normals.size() == 0 || N == m_Vertices.Normals.size());

	BoundingBox::CreateFromPoints(m_BoundingBox, m_Vertices.Postions.size(), &m_Vertices.Postions[0], sizeof(DirectX::SimpleMath::Vector3));

	if (!HasVertexNormal())
		GenerateVertexNormal();

	m_Vertices.CreateVertexAttributeBuffers();
	CreateElementBuffer();
}


void ObjMesh::GenerateVertexNormal()
{
	vector<Vector3> facetNormals;
	auto N = m_Vertices.Count();
	m_Vertices.Normals.resize(N);
	XMVECTOR n, v0, v1, v2;
	for (const auto& face : Facets())
	{
		v0 = m_Vertices.Postions[face.x];
		v1 = m_Vertices.Postions[face.y];
		v2 = m_Vertices.Postions[face.z];
		v1 -= v0;
		v2 -= v0;
		n = XMVector3Cross(v1, v2);
		n = XMVector3Normalize(n);
		if (XMVectorGetY(n) < 0.0f) 
			n = XMVectorNegate(n);
		facetNormals.emplace_back(n);
		m_Vertices.Normals[face.x] += n;
		m_Vertices.Normals[face.y] += n;
		m_Vertices.Normals[face.z] += n;
	}

	for (auto& verNor : m_Vertices.Normals)
	{
		verNor.Normalize();
		//verNor = Vector3(0.0f, 1.0f, 0.0f);
	}
}

ObjMesh::~ObjMesh()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_ElementBuffer);
}
