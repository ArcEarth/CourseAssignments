#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <algorithm>
#include "SimpleMath.h"

enum class ElementTypeEnum
{
	Uint8,
	Int8,
	Uint16,
	Int16,
	Uint32,
	Int32,
	Float,
	Double,
};

enum class VertexAttributeSemantic
{
	Position,
	Color,
	Normal,
};

struct VertexAttribute
{
	std::string Name;
	uint32_t Dimension;
	ElementTypeEnum ElementType;
	//VertexAttributeSemantic Semantic;
};

class VertexTypeDescription
{
public:
	VertexTypeDescription(std::initializer_list<VertexAttribute> &&attr_list)
		: Attributes(std::move(attr_list))
	{
	}

	VertexTypeDescription& operator=(std::initializer_list<VertexAttribute> &&attr_list)
	{
		Attributes.assign(attr_list);
	}

	size_t Size() const
	{
		Attributes.size();
	}
	std::vector<VertexAttribute> Attributes;

	bool HasAttribute(const std::string &name) const
	{
		return std::find_if(Attributes.cbegin(), Attributes.cend(),
			[&name](const VertexAttribute& attr)
		{
			return attr.Name == name;
		}) != Attributes.cend();
	}

	size_t GetAttributeIndex(const std::string &name) const
	{
		return std::find_if(Attributes.cbegin(), Attributes.cend(),
			[&name](const VertexAttribute& attr)
		{
			return attr.Name == name;
		}) - Attributes.cbegin();
	}

	const VertexAttribute& operator[](const std::string &name) const
	{
		return *std::find_if(Attributes.cbegin(), Attributes.cend(),
			[&name](const VertexAttribute& attr)
		{
			return attr.Name == name;
		});
	}
};

namespace VertexTypes {
	struct VertexBasic
	{
		VertexBasic()
		{}
		VertexBasic(const DirectX::SimpleMath::Vector3 &position)
			: Position(position)
		{}
		DirectX::SimpleMath::Vector3 Position;
		static const VertexTypeDescription Description;
	};

	struct VertexPositionColor
	{
		VertexPositionColor()
		{}
		VertexPositionColor(const DirectX::SimpleMath::Vector3 &position, DirectX::SimpleMath::Color color)
			: Position(position), Color(color)
		{}
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Color Color;
		static const VertexTypeDescription Description;
	};

	struct VertexPositionNormalColor
	{
		VertexPositionNormalColor()
		{}
		VertexPositionNormalColor(const DirectX::SimpleMath::Vector3 &position, const DirectX::SimpleMath::Vector3 &normal, DirectX::SimpleMath::Color color)
			: Position(position), Normal(normal), Color(color)
		{}
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector3 Normal;
		DirectX::SimpleMath::Color Color;
		static const VertexTypeDescription Description;
	};

	struct VertexPostionTexture
	{
		VertexPostionTexture()
		{}
		VertexPostionTexture(const DirectX::SimpleMath::Vector3 &position, DirectX::SimpleMath::Vector2 texture_coor)
			: Position(position), TextureCoordinate(texture_coor)
		{}
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector2 TextureCoordinate;
		static const VertexTypeDescription Description;
	};

	struct VertexPostionNormalTexture
	{
		VertexPostionNormalTexture()
		{}
		VertexPostionNormalTexture(const DirectX::SimpleMath::Vector3 &position, const DirectX::SimpleMath::Vector3 &normal, DirectX::SimpleMath::Vector2 texture_coor)
			: Position(position), Normal(normal), TextureCoordinate(texture_coor)
		{}
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector3 Normal;
		DirectX::SimpleMath::Vector2 TextureCoordinate;
		static const VertexTypeDescription Description;
	};

	struct VertexPostionNormal
	{
		VertexPostionNormal()
		{}
		VertexPostionNormal(const DirectX::SimpleMath::Vector3 &position, const DirectX::SimpleMath::Vector3 &normal)
			: Position(position), Normal(normal)
		{}
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector3 Normal;
		static const VertexTypeDescription Description;
	};
}