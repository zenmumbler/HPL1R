/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_SDLBMPFILE_H
#define HPL_SDLBMPFILE_H

#include <SDL2/SDL.h>

#include "system/SystemTypes.h"

namespace hpl {

	SDL_Surface* LoadBMPFileUsingSDLImage(const tString& asFilePath);
	SDL_Surface* LoadBMPFile(const tString& asFilePath);

}

#endif /* HPL_SDLBMPFILE_H */
