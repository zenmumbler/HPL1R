//  DebugMenu.h
//  PenumbraOverture
//  (c) 2021 by zenmumbler

#ifndef GAME_DEBUG_MENU_H
#define GAME_DEBUG_MENU_H

#include "StdAfx.h"

class cDebugMenu : public hpl::iUpdateable
{
public:
	cDebugMenu();
	void OnDraw() override;
	
	bool IsEnabled() { return mbEnabled; }
	void SetEnabled(bool enable) { mbEnabled = enable; }
	void ToggleEnabled() { mbEnabled = !mbEnabled; }

private:
	bool mbEnabled;
};

#endif // GAME_DEBUG_MENU_H
