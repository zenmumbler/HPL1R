/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */
#include "resources/LoadImage.h"
#include "stb/stb_image.h"
#include "system/Log.h"

namespace hpl {

	std::optional<Bitmap> LoadBitmapFile(const tString &fullPath)
	{
		int width, height, chans;
		const auto pixels = stbi_load(fullPath.c_str(), &width, &height, &chans, 4);

		if (pixels == nullptr) {
			Error("Failed to load image: %s!\n", fullPath.c_str());
			return std::nullopt;
		}

		Bitmap bmp;
		if (! bmp.CreateFromPixels(pixels, width, height)) {
			Error("Failed to create bitmap for image: %s!\n", fullPath.c_str());
			stbi_image_free(pixels);
			return std::nullopt;
		}
		bmp.TogglePixelFormat();
		stbi_image_free(pixels);

	//		Log("Image: %s - %d x %d - Chans: %d\n", asFilePath.c_str(), width, height, chans);

		return bmp;
	}

	//-----------------------------------------------------------------------

	void GetSupportedImageFormats(std::vector<tString> &alstFormats)
	{
		alstFormats.push_back("bmp");
		alstFormats.push_back("jpg");
		alstFormats.push_back("jpeg");
		alstFormats.push_back("png");
		alstFormats.push_back("tga");
	}

}
