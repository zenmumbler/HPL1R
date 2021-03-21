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

	struct cTGAHeader {
		Uint8 infolen;      /* length of info field */
		Uint8 has_cmap;     /* 1 if image has colormap, 0 otherwise */
		Uint8 type;

		Uint16 cmap_start;    /* index of first colormap entry */
		Uint16 cmap_len;      /* number of entries in colormap */
		Uint8 cmap_bits;        /* bits per colormap entry */

		Uint16 yorigin;       /* image origin (ignored here) */
		Uint16 xorigin;
		Uint16 width;     /* image size */
		Uint16 height;
		Uint8 bpp;       /* bits/pixel */
		Uint8 flags;
	};

	enum eTGAType {
		TGA_TYPE_INDEXED = 1,
		TGA_TYPE_RGB = 2,
		TGA_TYPE_BW = 3,
		TGA_TYPE_RLE_INDEXED = 9,
		TGA_TYPE_RLE_RGB = 10,
		TGA_TYPE_RLE_BW = 11
	};

	#pragma pack(pop)

	SDL_Surface* LoadTGAFileUsingSDLImage(const tString& asFilePath) {
		return IMG_Load(asFilePath.c_str());
	}

	SDL_Surface* LoadTGAFile(const tString& asFilePath) {
		SDL_RWops *rw = SDL_RWFromFile(asFilePath.c_str(), "rb");
		Uint64 iFileSize = SDL_RWsize(rw);
		
		cTGAHeader header;
		if (SDL_RWread(rw, &header, sizeof(cTGAHeader), 1) < 1) {
			Error("Failed to read TGA file header\n");
			SDL_RWclose(rw);
			return NULL;
		}
		if (header.bpp != 32 || header.type != TGA_TYPE_RGB) {
			// Log("Not a 32-bit linear TGA format, deferring to SDL2_image\n");
			SDL_RWclose(rw);
			return LoadTGAFileUsingSDLImage(asFilePath);
		}
		
		Uint32 iSizeImage = header.width * header.height * 4;
		if (iSizeImage + header.infolen + sizeof(cTGAHeader) > iFileSize) {
			Error("Required bitmap data size exceeds file size\n");
			SDL_RWclose(rw);
			return NULL;
		}
		if (SDL_RWseek(rw, header.infolen + sizeof(cTGAHeader), RW_SEEK_SET) < 0) {
			Error("Could not seek to image data offset\n");
			SDL_RWclose(rw);
			return NULL;
		}
		
		Uint8* pData = hplNewArray(Uint8, iSizeImage);
		if (SDL_RWread(rw, pData, iSizeImage, 1) < 1) {
			Error("Failed to read TGA image data\n");
			hplDeleteArray(pData);
			SDL_RWclose(rw);
			return NULL;
		}

		SDL_RWclose(rw);
		
		// Log("***** USING TGA LOADER\n");
		
		SDL_Surface* pSurface = SDL_CreateRGBSurfaceWithFormat(0, header.width, header.height, 32, SDL_PIXELFORMAT_RGBA32);

		Uint8* pConverted = static_cast<Uint8*>(pSurface->pixels);
		Uint8* pSrc = pData + iSizeImage - (4 * header.width);
		Uint8* pDst = pConverted;
		for (Sint32 y = 0; y < header.height; ++y) {
			for (Sint32 x = 0; x < header.width; ++x) {
				// TODO: this makes no sense, pixelformat is effectively ABGR and this is BGRA, but it works
				/* B */pDst[0] = pSrc[0];
				/* G */pDst[1] = pSrc[1];
				/* R */pDst[2] = pSrc[2];
				/* A */pDst[3] = pSrc[3];
				
				pSrc += 4;
				pDst += 4;
			}

			pSrc -= 2 * 4 * header.width;
			pDst += pSurface->pitch - (4 * header.width);
		}
		
		hplDeleteArray(pData);
		return pSurface;
	}

}
