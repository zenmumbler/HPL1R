/*
 * 2024 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_SAMPLER_H
#define HPL_SAMPLER_H

namespace hpl {

	//-----------------------------------------

	enum class TextureWrap
	{
		ClampToEdge,
		Repeat,
		MirrorRepeat,
	};

	//-----------------------------------------

	enum class TextureFilter
	{
		Nearest,
		Linear,
	};

	//-----------------------------------------

	enum class TextureCompareFunc
	{
		Disabled,

		Never,
		Always,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual
	};

	//-----------------------------------------

	struct SamplerDesc {
		TextureWrap wrapU = TextureWrap::ClampToEdge;
		TextureWrap wrapV = TextureWrap::ClampToEdge;
		TextureWrap wrapW = TextureWrap::ClampToEdge;

		TextureFilter minFilter = TextureFilter::Nearest;
		TextureFilter magFilter = TextureFilter::Nearest;
		TextureFilter mipmapFilter = TextureFilter::Nearest;

		float minLod = 0.0f;
		float maxLod = 32.0f;

		int maxAnisotropy = 1;
		TextureCompareFunc compareFunc = TextureCompareFunc::Disabled;
	};

}

#endif // HPL_SAMPLER_H
