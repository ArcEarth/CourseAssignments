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
	const VertexAttribute& operator[](const std::string name) const
	{
		return *std::find_if(Attributes.cbegin(), Attributes.cend(),
			[&name](const VertexAttribute& attr)
		{
			return attr.Name == name;
		});
	}
};

namespace VertexTypes {
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
}