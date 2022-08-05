/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#import <Cocoa/Cocoa.h>
#include "system/System.h"

namespace hpl {

	static NSString* getGamePathForBundle(NSBundle *bundle)
	{
		NSString *dataPath = nil;

		NSString *startPath = [[bundle infoDictionary] objectForKey:@"StartFolder"];
		if ([startPath isEqualToString:@"PARENT"]) {
			dataPath = [bundle bundlePath];
			dataPath = [dataPath stringByDeletingLastPathComponent];
		} else { // default is resource
			dataPath = [bundle resourcePath];
		}

		return dataPath;
	}

	tString GetDataDir()
	{
		tString sTemp;

		@autoreleasepool {
			NSBundle *bundle = [NSBundle mainBundle];
			NSString *dataPath = getGamePathForBundle(bundle);

			sTemp = [dataPath UTF8String];
		}

		return sTemp;
	}

	void MacOSAlertBox(eMsgBoxType eType, tString caption, tString message)
	{
		@autoreleasepool {
			NSAlert *alert = [[NSAlert alloc] init];
			
			switch (eType) {
				case eMsgBoxType_Error:
					[alert setAlertStyle:NSAlertStyleCritical];
					break;
				case eMsgBoxType_Warning:
					[alert setAlertStyle:NSAlertStyleWarning];
					break;
				case eMsgBoxType_Info:
				case eMsgBoxType_Default:
					[alert setAlertStyle:NSAlertStyleInformational];
			}
			
			[alert setMessageText: [NSString stringWithFormat:@"%s", caption.c_str()]];
			[alert setInformativeText: [NSString stringWithFormat:@"%s", message.c_str()]];
			
			[alert runModal];
		}
	}

}
