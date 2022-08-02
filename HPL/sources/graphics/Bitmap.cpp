/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */

#include "graphics/Bitmap.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	Bitmap::Bitmap()
	: width_(0), height_(0), data_{nullptr}, path_{}
	{
	}

	Bitmap::Bitmap(int width, int height) : Bitmap() {
		Create(width, height);
	}

	Bitmap::Bitmap(Bitmap &&source) {
		width_ = source.width_;
		height_ = source.height_;
		data_ = source.data_;
		path_ = source.path_;

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

		for (int y=0; y<vertCopy; y++)
		{
			for (int x=0; x<horizCopy; x++)
			{
				*destBuffer++ = *srcBuffer++;
			}
			srcBuffer += sourceSkip;
			destBuffer += destSkip;
		}
	}

	//-----------------------------------------------------------------------

	void Bitmap::CopyFromBitmap(const Bitmap &src, int x, int y, int w, int h) const
	{
		if (x < 0 || y < 0 || w > width_ || h > height_) {
			return;
		}
		if (data_ == nullptr || src.data_ == nullptr) {
			return;
		}

		int sourceSkip = src.GetWidth() - w;
		int destSkip = width_ - w;
		auto srcBuffer = src.data_ + (y * src.GetWidth()) + x;
		auto destBuffer = data_;

		for (int col=0; col < h; col++)
		{
			for (int row=0; row < w; row++)
			{
				*destBuffer++ = *srcBuffer++;
			}
			srcBuffer += sourceSkip;
			destBuffer += destSkip;
		}
	}

	//-----------------------------------------------------------------------

	void Bitmap::TogglePixelFormat() {
		if (data_ == nullptr) {
			return;
		}

		uint32_t* current = data_;
		for (int y = 0; y < height_; ++y) {
			for (int x = 0; x < width_; ++x) {
				uint32_t pixIn = *current;
				// flip R and G RGBA -> BGRA
				uint32_t pixOut = (pixIn & 0xff00ff00) | ((pixIn & 0xff) << 16) | ((pixIn & 0xff0000) >> 16);
				*current++ = pixOut;
			}
		}
	}

	//-----------------------------------------------------------------------

	void Bitmap::FillRect(const cRect2l &aRect, const cColor &aColor)
	{
		if (data_ == nullptr) {
			return;
		}

		int x = aRect.x;
		int y = aRect.y;
		int w = aRect.w <= 0 ? width_ : aRect.w;
		int h = aRect.h <= 0 ? height_ : aRect.h;

		uint32_t col =
			(static_cast<uint8_t>(aColor.r * 255.0f) << 24) |
			(static_cast<uint8_t>(aColor.g * 255.0f) << 16) |
			(static_cast<uint8_t>(aColor.b * 255.0f) << 8) |
			(static_cast<uint8_t>(aColor.a * 255.0f));

		int horizFill = std::min(width_ - x, w);
		int vertFill = std::min(height_ - y, h);
		int skip = width_ - horizFill;

		auto pixels = data_ + (y * width_) + x;

		for (int y=0; y<vertFill; y++)
		{
			for (int x=0; x<horizFill; x++)
			{
				*pixels = col;
			}
			pixels += skip;
		}
	}

	//-----------------------------------------------------------------------

}
