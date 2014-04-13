#include "VertexTypes.h"

const VertexTypeDescription VertexTypes::VertexPositionColor::Description = {
	VertexAttribute{ "Position", 3U, ElementTypeEnum::Float },
	VertexAttribute{ "Color", 4U, ElementTypeEnum::Float }
};