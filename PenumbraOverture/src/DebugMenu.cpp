//  DebugMenu.cpp
//  PenumbraOverture
//  (c) 2021 by zenmumbler

#include "DebugMenu.h"
#include "Init.h"
#include "GraphicsHelper.h"
#include "ModelExport.h"

#include "imgui/imgui.h"

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
	
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(360, 400), ImGuiCond_FirstUseEver);
	
	if (ImGui::Begin("Debug Views")) {
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
}
