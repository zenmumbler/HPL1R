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

		PixelFormat GetPixelFormat() const { return _format; }

		const uint8_t *GetData(int level) const { return _data; }
		int GetByteSize(int level) const { return _byteSize; }
		int GetMipMapCount() { return _levels; }
		int GetWidth(int level) const { return _width; }
		int GetHeight(int level) const { return _height; }

	private:
		PixelFormat _format;
		uint8_t *_data;
		int _byteSize;
		int _levels;
		int _width;
		int _height;
	};

	void GetSupportedImageFormatsX(std::vector<tString> &formats);

}

#endif // HPL_IMAGE_H
