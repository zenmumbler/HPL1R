//  DebugMenu.cpp
//  PenumbraOverture
//  (c) 2021 by zenmumbler

#include "DebugMenu.h"
#include "Init.h"
#include "GraphicsHelper.h"

#include "imgui.h"

using namespace hpl;

cDebugMenu::cDebugMenu(cInit *apInit)
: iUpdateable("DebugMenu")
{
	mbEnabled = false;
	mpInit = apInit;
}

void cDebugMenu::OnDraw() {
	if (! mbEnabled) {
		return;
	}

	ImGui::ShowDemoWindow();
}
