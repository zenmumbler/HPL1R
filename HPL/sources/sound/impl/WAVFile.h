/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */

#ifndef HPL_WAVFILE_H
#define HPL_WAVFILE_H

#include "system/StringTypes.h"

namespace hpl {

	uint32_t LoadWAVFile(const tString& filePath, int *channels, int *format, int *rate, short **samples);

}

#endif /* HPL_WAVFILE_H */
