/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_LOADIMAGE_H
#define HPL_LOADIMAGE_H

#include <optional>
#include <vector>
#include "system/String.h"
#include "graphics/Bitmap.h"

namespace hpl {

	std::optional<Bitmap> LoadBitmapFile(const tString &fullPath);
	void GetSupportedImageFormats(std::vector<tString> &formats);

}

#endif // HPL_LOADIMAGE_H