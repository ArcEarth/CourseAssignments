#include "VertexTypes.h"

const VertexTypeDescription VertexTypes::VertexPositionColor::Description = {
	VertexAttribute{ "Position", 3U, ElementTypeEnum::Float },
	VertexAttribute{ "Color", 4U, ElementTypeEnum::Float }
};

const VertexTypeDescription VertexTypes::VertexBasic::Description = {
	VertexAttribute{ "Position", 3U, ElementTypeEnum::Float },
};

const VertexTypeDescription VertexTypes::VertexPostionTexture::Description = {
	VertexAttribute{ "Position", 3U, ElementTypeEnum::Float },
	VertexAttribute{ "TextureCoordinate", 2U, ElementTypeEnum::Float }
};

const VertexTypeDescription VertexTypes::VertexPostionNormal::Description = {
	VertexAttribute{ "Position", 3U, ElementTypeEnum::Float },
	VertexAttribute{ "Normal", 3U, ElementTypeEnum::Float },
};

const VertexTypeDescription VertexTypes::VertexPostionNormalTexture::Description = {
	VertexAttribute{ "Position", 3U, ElementTypeEnum::Float },
	VertexAttribute{ "Normal", 3U, ElementTypeEnum::Float },
	VertexAttribute{ "TextureCoordinate", 2U, ElementTypeEnum::Float }
};

const VertexTypeDescription VertexTypes::VertexPositionNormalColor::Description = {
	VertexAttribute{ "Position", 3U, ElementTypeEnum::Float },
	VertexAttribute{ "Normal", 3U, ElementTypeEnum::Float },
	VertexAttribute{ "Color", 4U, ElementTypeEnum::Float }
};