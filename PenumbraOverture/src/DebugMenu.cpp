//  DebugMenu.cpp
//  PenumbraOverture
//  (c) 2021-3 by zenmumbler

#include "DebugMenu.h"
#include "Init.h"
#include "GraphicsHelper.h"
#include "MapHandler.h"
#include "ModelExport.h"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "imgui/imgui.h"

using namespace hpl;

cDebugMenu::cDebugMenu(cInit *apInit)
: iUpdateable("DebugMenu")
{
	mbEnabled = false;
	mpInit = apInit;
}

const char* ALL_MAPS[] = {
	"level00_01_boat_cabin.dae",
	"level01_01_outside.dae",
	"level01_02_mine_entrance.dae",
	"level01_03_shafts.dae",
	"level01_04_old_storage.dae",
	"level01_05_office.dae",
	"level01_06_workshop.dae",
	"level01_07_lead_mine_shafts_A.dae",
	"level01_07_lead_mine_shafts_B.dae",
	"level01_07_lead_mine_shafts_C.dae",
	"level01_08_water_cave.dae",
	"level01_09_generator_room.dae",
	"level01_10_small_shaft.dae",
	"level01_11_new_storage_room.dae",
	"level01_12_water_cave_tunnels.dae",
	"level01_13_A_iron_mine_shafts.dae",
	"level01_13_B_iron_storage_tunnels.dae",
	"level01_13_C_iron_base_tunnels.dae",
	"level01_14_refinery.dae",
	"level01_15_icecave.dae",
	"level01_16_destroyed_shaft.dae",
	"level01_17_worm_nest.dae",
	"level01_18_mining_room.dae",
	"level01_19_door_shaft.dae",
	"level01_20_base_entrance.dae"
};

int currentMapIndex = 0;

void cDebugMenu::OnDraw() {
	if (! mbEnabled) {
		return;
	}
/*
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(360, 400), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Penumbra Debug")) {
		if (ImGui::CollapsingHeader("Gameplay")) {
			ImGui::ListBox("Select a map", &currentMapIndex, ALL_MAPS, sizeof(ALL_MAPS) / sizeof(char*), 10);

			if (ImGui::Button("Jump To Map")) {
				mpInit->mpMapHandler->Load(ALL_MAPS[currentMapIndex], "link1");
			}
		}

		if (ImGui::CollapsingHeader("Geometry")) {
			auto renderer = mpInit->mpGame->GetGraphics()->GetRenderer3D();
			auto renderFlags = renderer->GetDebugFlags();

			ImGui::CheckboxFlags("Show Wireframes", &renderFlags, eRendererDebugFlag_RenderLines);
			ImGui::CheckboxFlags("Show Normals", &renderFlags, eRendererDebugFlag_DrawNormals);
			ImGui::CheckboxFlags("Show Tangents", &renderFlags, eRendererDebugFlag_DrawTangents);
			ImGui::Separator();
			ImGui::CheckboxFlags("Show Bounding Boxes", &renderFlags, eRendererDebugFlag_DrawBoundingBox);
			ImGui::CheckboxFlags("Show Bounding Spheres", &renderFlags, eRendererDebugFlag_DrawBoundingSphere);
			ImGui::Separator();
			ImGui::CheckboxFlags("Show Light Bounds", &renderFlags, eRendererDebugFlag_DrawLightBoundingBox);
			ImGui::CheckboxFlags("Disable Lighting", &renderFlags, eRendererDebugFlag_DisableLighting);
			ImGui::Separator();
			ImGui::CheckboxFlags("Draw Physics Boxes", &renderFlags, eRendererDebugFlag_DrawPhysicsBox);

			renderer->SetDebugFlags(renderFlags);
		}
		
		if (ImGui::CollapsingHeader("Export")) {
			if (ImGui::Button("Test Model Export")) {
				ExportModels(mpInit->mpGame);
			}
		}
	}
	ImGui::End();
*/
}
