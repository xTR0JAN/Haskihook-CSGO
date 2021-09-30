#include "includes.h"

void Hooks::DrawModelExecute( uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone ) {
	// disables csgo's hdr effect.

	// disable rendering of shadows.
	if (strstr(info.m_model->m_name, XOR("shadow")) != nullptr && g_menu.main.visuals.removes.get(6))
		return;

	// disable rendering of sleeves.
	if (strstr(info.m_model->m_name, XOR("sleeve")) != nullptr && g_menu.main.visuals.removes.get(5))
		return;

	static auto hdr = g_csgo.m_material_system->FindMaterial(XOR("dev/blurfiltery_nohdr"), XOR("Other textures"));
	hdr->SetFlag(MATERIAL_VAR_NO_DRAW, true);

	// dont override glow.
	if (g_csgo.m_studio_render->IsForcedMaterialOverriden())
		return g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(this, ctx, state, info, bone);

	// do chams.
	if (g_chams.DrawModel(ctx, state, info, bone)) {
		g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(this, ctx, state, info, bone);
		g_csgo.m_studio_render->ForcedMaterialOverride(nullptr);
	}
}