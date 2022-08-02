/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_BITMAP_H
#define HPL_BITMAP_H

#include "system/String.h"

namespace hpl {

	class Bitmap
	{
	public:
		Bitmap();
		Bitmap(int width, int height);
		Bitmap(const Bitmap&) = delete;
		Bitmap(Bitmap &&source);
		~Bitmap();

		bool Create(int width, int height);
		bool CreateFromPixels(void* source, int width, int height);

		/**
		 * Draw this bitmap onto another at specified position
		 */
		void DrawToBitmap(const Bitmap &dest, int x, int y) const;

		/**
		 * Copy rectangular area from passed source bitmap onto this one at origin.
		 */
		void CopyFromBitmap(const Bitmap &src, int x, int y, int w, int h) const;

		/**
		 * Draws a solid rect onto the bitmap. IF h and w is than w and how of the bitmap is used.
		 */
		void FillRect(const cRect2l &aRect, const cColor &aColor);

		/**
		 * Flip the R and G channels to convert between RGBA and BGRA format.
		 */
		void TogglePixelFormat();

		template <typename T=uint8_t>
		T* GetRawData() const { return reinterpret_cast<T*>(data_); }

		int GetBPP() const { return 32; }
		int GetNumChannels() const { return 4; }

		int GetHeight() const { return height_; }
		int GetWidth() const { return width_; }

		void SetPath(const tString& newPath) { path_ = newPath; }
		tString GetPath() const { return path_; }
		tString GetFileName() const { return cString::GetFileName(path_); }

	private:
		int height_;
		int width_;
		uint32_t *data_;
		tString path_;
	};

};
#endif // HPL_BITMAP_H
