/*
 * 2024 by zenmumbler
 * This file is part of Rehatched
 */

#ifndef HPL_PIXELFORMAT_H
#define HPL_PIXELFORMAT_H

namespace hpl {

	enum class PixelFormat {
		None,

		// 8bpc Linear
		R8,
		RG8,
		RGB8,
		RGBA8,

		// 8bpc sRGB
		SRGB8,
		SRGBA8,

		// 8bpc Linear BGR
		BGR8,
		BGRA8,

		// BC Linear
		BC1_RGB,
		BC1_RGBA,
		BC3_RGBA,
		BC5_RGBA,

		// BC sRGB
		BC1_SRGB,
		BC1_SRGBA,
		BC3_SRGBA,
		BC5_SRGBA,
	};

	constexpr int ChannelCountForPixelFormat(PixelFormat pf) {
		if (pf == PixelFormat::R8) return 1;
		if (pf == PixelFormat::RG8) return 2;
		if (pf == PixelFormat::RGB8 || pf == PixelFormat::SRGB8 || pf == PixelFormat::BGR8) return 3;
		if (pf == PixelFormat::BC1_RGB || pf == PixelFormat::BC1_SRGB) return 3;
		return 4;
	}

	constexpr bool IsCompressedPixelFormat(PixelFormat pf) {
		return pf >= PixelFormat::BC1_RGB;
	}

	constexpr int BytesPerPixel(PixelFormat pf) {
		if (IsCompressedPixelFormat(pf)) return 0;
		// TODO: update when non-8bpc formats are added
		return ChannelCountForPixelFormat(pf);
	}

}

#endif // HPL_PIXELFORMAT_H
