//
//  SDLBMPFile.h
//  Rehatched - BMP file reader because SDL2_image does not support 32-bit BMPs
//
//  Created by zenmumbler on 03/11/2020.
//

#ifndef HPL_SDLBMPFILE_H
#define HPL_SDLBMPFILE_H

#include <SDL2/SDL.h>

#include "system/SystemTypes.h"

namespace hpl {

	SDL_Surface* LoadBMPFileUsingSDLImage(const tString& asFilePath);
	SDL_Surface* LoadBMPFile(const tString& asFilePath);

}

#endif /* HPL_SDLBMPFILE_H */
