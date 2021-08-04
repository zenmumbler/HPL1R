//  DebugMenu.cpp
//  PenumbraOverture
//  (c) 2021 by zenmumbler

#include "DebugMenu.h"
#include "imgui.h"

using namespace hpl;

cDebugMenu::cDebugMenu()
: iUpdateable("DebugMenu")
{
	mbEnabled = false;
}

void cDebugMenu::OnDraw() {
	if (! mbEnabled) {
		return;
	}

	ImGui::ShowDemoWindow();
}
