/*
 * 2024 by zenmumbler
 * This file is part of Rehatched
 */

#ifndef HPL_IMAGE_H
#define HPL_IMAGE_H

#include "graphics/PixelFormat.h"
#include "resources/ResourceBase.h"

namespace hpl {

	class Image : public iResourceBase
	{
	public:
		Image(tString name);
		~Image();

		bool CreateBlank(int width, int height, PixelFormat format);
		bool CreateFromFile(const tString& fullPath);

		const uint8_t *GetData() const { return _data; }
		PixelFormat GetPixelFormat() const { return _format; }
		int GetWidth() const { return _width; }
		int GetHeight() const { return _height; }

	private:
		uint8_t *_data;
		PixelFormat _format;
		int _width;
		int _height;
	};

	void GetSupportedImageFormatsX(std::vector<tString> &formats);

}

#endif // HPL_IMAGE_H
