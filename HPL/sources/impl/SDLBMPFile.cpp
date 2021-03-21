//
//  SDLBMPFile.h
//  Rehatched - BMP file reader because SDL2_image does not support 32-bit BMPs
//
//  Created by zenmumbler on 03/11/2020.
//

#include "SDLBMPFile.h"
#include "LowLevelSystem.h"

#include <SDL2_image/SDL_image.h>

namespace hpl {

	#pragma pack(push, 1)

	struct cBMPFileHeader {
		Uint16 bfType;  //specifies the file type
		Uint32 bfSize;  //specifies the size in bytes of the bitmap file
		Uint16 bfReserved1;  //reserved; must be 0
		Uint16 bfReserved2;  //reserved; must be 0
		Uint32 bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
	};

	struct cBMPImageHeader {
		Uint32 biSize;  //specifies the number of bytes required by the struct
		Sint32 biWidth;  //specifies width in pixels
		Sint32 biHeight;  //species height in pixels
		Uint16 biPlanes; //specifies the number of color planes, must be 1
		Uint16 biBitCount; //specifies the number of bit per pixel
		Uint32 biCompression;//spcifies the type of compression
		Uint32 biSizeImage;  //size of image in bytes
		Sint32 biXPelsPerMeter;  //number of pixels per meter in x axis
		Sint32 biYPelsPerMeter;  //number of pixels per meter in y axis
		Uint32 biClrUsed;  //number of colors used by th ebitmap
		Uint32 biClrImportant;  //number of colors that are important
	};

	#pragma pack(pop)

	SDL_Surface* LoadBMPFileUsingSDLImage(const tString& asFilePath) {
		return IMG_Load(asFilePath.c_str());
	}

	SDL_Surface* LoadBMPFile(const tString& asFilePath) {
		SDL_RWops *rw = SDL_RWFromFile(asFilePath.c_str(), "rb");
		Sint64 iFileSize = SDL_RWsize(rw);
		
		cBMPFileHeader header;
		if (SDL_RWread(rw, &header, sizeof(cBMPFileHeader), 1) < 1) {
			Error("Failed to read BMP file header\n");
			SDL_RWclose(rw);
			return NULL;
		}
		if (header.bfType != 0x4D42) {
			Error("File is not a BMP file\n");
			SDL_RWclose(rw);
			return NULL;
		}
		if (static_cast<Sint64>(header.bfSize) != iFileSize) {
			Error("Internal BMP file size field does not match actual file size\n");
			SDL_RWclose(rw);
			return NULL;
		}
		if (static_cast<Sint64>(header.bfOffBits) >= iFileSize) {
			Error("Invalid BMP internal file offset of bitmap data\n");
			SDL_RWclose(rw);
			return NULL;
		}

		cBMPImageHeader imgHeader;
		if (SDL_RWread(rw, &imgHeader, sizeof(cBMPImageHeader), 1) < 1) {
			Error("Failed to read BMP image header\n");
			SDL_RWclose(rw);
			return NULL;
		}
		if (static_cast<Sint64>(imgHeader.biSize) != sizeof(cBMPImageHeader)) {
			Error("Unexpected internal image header size\n");
			SDL_RWclose(rw);
			return NULL;
		}
		if (imgHeader.biBitCount != 32 || imgHeader.biCompression != 0) {
			// Log("Not a 32-bit BMP or weird format, deferring to SDL2_image\n");
			SDL_RWclose(rw);
			return LoadBMPFileUsingSDLImage(asFilePath);
		}
		
		Uint32 iSizeImage = imgHeader.biWidth * imgHeader.biHeight * 4;
		if (iSizeImage + header.bfOffBits > iFileSize) {
			Error("Required bitmap data size exceeds file size\n");
			SDL_RWclose(rw);
			return NULL;
		}
		if (SDL_RWseek(rw, header.bfOffBits, RW_SEEK_SET) < 0) {
			Error("Could not seek to image data offset\n");
			SDL_RWclose(rw);
			return NULL;
		}
		
		Uint8* pData = hplNewArray(Uint8, iSizeImage);
		if (SDL_RWread(rw, pData, iSizeImage, 1) < 1) {
			Error("Failed to read BMP image data\n");
			hplDeleteArray(pData);
			SDL_RWclose(rw);
			return NULL;
		}

		SDL_RWclose(rw);
		
		SDL_Surface* pSurface = SDL_CreateRGBSurfaceWithFormat(0, imgHeader.biWidth, imgHeader.biHeight, 32, SDL_PIXELFORMAT_RGBA32);

		Uint8* pConverted = static_cast<Uint8*>(pSurface->pixels);
		Uint8* pSrc = pData + iSizeImage - (4 * imgHeader.biWidth);
		Uint8* pDst = pConverted;
		for (Sint32 y = 0; y < imgHeader.biHeight; ++y) {
			for (Sint32 x = 0; x < imgHeader.biWidth; ++x) {
				// TODO: this makes no sense, pixelformat is effectively ABGR and this is BGRA, but it works
				/* B */pDst[0] = pSrc[0];
				/* G */pDst[1] = pSrc[1];
				/* R */pDst[2] = pSrc[2];
				/* A */pDst[3] = pSrc[3];
				
				pSrc += 4;
				pDst += 4;
			}
			
			pSrc -= 2 * 4 * imgHeader.biWidth;
			pDst += pSurface->pitch - (4 * imgHeader.biWidth);
		}
		
		hplDeleteArray(pData);
		return pSurface;
	}

}
