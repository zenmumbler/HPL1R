/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_BITMAP_H
#define HPL_BITMAP_H

#include <cstdint>

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

		template <typename T=uint8_t>
		T* GetRawData() const { return reinterpret_cast<T*>(data_); }

		int GetNumChannels() const { return 4; }

		int GetHeight() const { return height_; }
		int GetWidth() const { return width_; }

	private:
		int height_;
		int width_;
		uint32_t *data_;
	};

}

#endif // HPL_BITMAP_H
