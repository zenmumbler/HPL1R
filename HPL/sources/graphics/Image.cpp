/*
 * 2024 by zenmumbler
 * This file is part of Rehatched
 */

#include "graphics/Image.h"
#include "system/String.h"
#include "system/Log.h"

#include "stb/stb_image.h"

namespace hpl {

	Image::Image(tString name)
		: iResourceBase{std::move(name)}
	{
		_data = nullptr;
		_format = PixelFormat::None;
		_width = 0;
		_height = 0;
	}

	Image::~Image() {
		if (_data) {
			stbi_image_free(_data); // stbi_image_free is just free(x) by default
		}
	}

	bool Image::CreateBlank(int width, int height, PixelFormat format) {
		if (_data != nullptr) {
			Error("Images are immutable after creation.");
			return false;
		}
		if (IsCompressedPixelFormat(format)) {
			Error("Cannot create a blank image with a compressed pixel format.");
			return false;
		}
		if (width <= 0 || height <= 0 || width > 32768 || height > 32768) {
			return false;
		}

		auto byteSize = width * height * BytesPerPixel(format);
		_data = static_cast<uint8_t*>(malloc(byteSize)); // use malloc to have compat with stbi allocs

		_format = format;
		_width = width;
		_height = height;

		return true;
	}

	bool Image::CreateFromFile(const tString& fullPath) {
		if (_data != nullptr) {
			Error("Images are immutable after creation.");
			return false;
		}

		auto extension = cString::ToLowerCase(cString::GetFileExt(fullPath));
		if (extension == "dds") {
			Error("DDS image loading not implemented yet.");
			return false;
		}
		else {
			int chans;
			_data = stbi_load(fullPath.c_str(), &_width, &_height, &chans, 4);

			if (_data == nullptr) {
				Error("Failed to load image: %s!\n", fullPath.c_str());
				return false;
			}
		}

		return true;
	}

	void GetSupportedImageFormatsX(std::vector<tString> &formats)
	{
		// gpu-optimized formats
		formats.push_back("dds");

		// standard formats
		formats.push_back("bmp");
		formats.push_back("jpg");
		formats.push_back("jpeg");
		formats.push_back("png");
		formats.push_back("tga");
	}

}
