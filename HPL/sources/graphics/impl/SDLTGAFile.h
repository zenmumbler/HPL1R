/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_SDLTGAFILE_H
#define HPL_SDLTGAFILE_H

#include <SDL2/SDL.h>

#include "system/SystemTypes.h"

namespace hpl {

	SDL_Surface* LoadTGAFileUsingSDLImage(const tString& asFilePath);
	SDL_Surface* LoadTGAFile(const tString& asFilePath);

}

#endif /* HPL_SDLTGAFILE_H */
