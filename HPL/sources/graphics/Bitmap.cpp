/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */

#include "graphics/Bitmap.h"
#include <algorithm>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	Bitmap::Bitmap()
	: width_(0), height_(0), data_{nullptr}
	{
	}

	Bitmap::Bitmap(int width, int height) : Bitmap() {
		Create(width, height);
	}

	Bitmap::Bitmap(Bitmap &&source) {
		width_ = source.width_;
		height_ = source.height_;
		data_ = source.data_;

		source.width_ = source.height_ = 0;
		source.data_ = nullptr;
	}

	Bitmap::~Bitmap()
	{
		if (data_) {
			delete[] data_;
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool Bitmap::Create(int width, int height) {
		if (width < 0 || height < 0 || width > 32768 || height > 32768) {
			return false;
		}
		if (data_) {
			delete[] data_;
		}
		width_ = width;
		height_ = height;
		if (width_ > 0 && height_ > 0) {
			data_ = new uint32_t[width * height];
		}
		else {
			data_ = nullptr;
		}
		return true;
	}

	//-----------------------------------------------------------------------

	bool Bitmap::CreateFromPixels(void* source, int width, int height) {
		if (width < 0 || height < 0 || width > 32768 || height > 32768) {
			return false;
		}
		if (data_) {
			delete[] data_;
		}
		width_ = width;
		height_ = height;
		if (width_ > 0 && height_ > 0) {
			data_ = new uint32_t[width * height];
			memcpy(data_, source, width * height * 4);
		}
		else {
			data_ = nullptr;
		}
		return true;
	}

	//-----------------------------------------------------------------------

	void Bitmap::DrawToBitmap(const Bitmap &dest, int x, int y) const
	{
		if (x < 0 || y < 0) {
			return;
		}
		if (data_ == nullptr || dest.data_ == nullptr) {
			return;
		}

		int horizCopy = std::min(width_, dest.GetWidth() - x);
		int vertCopy = std::min(height_, dest.GetHeight() - y);

		if (horizCopy < 0 || vertCopy < 0) {
			return;
		}

		int sourceSkip = width_ - horizCopy;
		int destSkip = dest.GetWidth() - horizCopy;

		auto destBuffer = dest.data_ + (y * dest.GetWidth()) + x;
		auto srcBuffer = data_;

		for (int h=0; h < vertCopy; h++)
		{
			for (int w=0; w < horizCopy; w++)
			{
				*destBuffer++ = *srcBuffer++;
			}
			srcBuffer += sourceSkip;
			destBuffer += destSkip;
		}
	}

	//-----------------------------------------------------------------------

}
