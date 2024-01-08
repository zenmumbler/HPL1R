/*
 * 2024 by zenmumbler
 * This file is part of Rehatched
 */

#ifndef HPL_PIXELFORMAT_H
#define HPL_PIXELFORMAT_H

#include <cstddef>

namespace hpl {

	enum class PixelFormat {
		None,

		// 8bpc Linear RGB
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

	constexpr bool IsCompressedPixelFormat(PixelFormat pf) {
		return pf >= PixelFormat::BC1_RGB;
	}

	constexpr int BytesPerPixel(PixelFormat pf) {
		if (IsCompressedPixelFormat(pf)) return 0;
		return 4;
	}

}

#endif // HPL_PIXELFORMAT_H
