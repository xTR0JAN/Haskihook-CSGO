#include "includes.h"

Chams g_chams{ };;

void Chams::SetColor(Color col, color_type_t type) {

	if (type == color_type_t::envmap_tint) {

		bool did_find = false;
		auto material = silhouette->FindVar(XOR("$envmaptint"), &did_find);

		if (did_find) {
			(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)material + 44))((uintptr_t)material, col.r() / 255.f, col.g() / 255.f, col.b() / 255.f);
		}
	}
	else {
		g_csgo.m_render_view->SetColorModulation(col);
	}
}

void Chams::SetAlpha(float alpha, IMaterial* mat) {
	if (mat)
		mat->AlphaModulate(alpha);

	else
		g_csgo.m_render_view->SetBlend(alpha);
}

void Chams::SetupMaterial(IMaterial* mat, Color col, bool z_flag) {
	g_csgo.m_render_view->SetColorModulation(col);


	// mat->SetFlag( MATERIAL_VAR_HALFLAMBERT, flags );
	mat->SetFlag(MATERIAL_VAR_ZNEARER, z_flag);
	mat->SetFlag(MATERIAL_VAR_NOFOG, z_flag);
	mat->SetFlag(MATERIAL_VAR_IGNOREZ, z_flag);

	g_csgo.m_studio_render->ForcedMaterialOverride(mat);
}

void Chams::init() {
	std::ofstream("csgo/materials/onetap_overlay.vmt") << R"#("VertexLitGeneric"
{
			"$basetexture" "VGUI/white_additive"
		"$bumpmap" "de_nuke/hr_nuke/pool_water_normals_002"
		"$bumptransform" "center 0.5 0.5 scale 0.25 0.25 rotate 0.0 translate 0.0 0.0"
		"$ignorez" "0"
		"$nofog" "1"
		"$model" "1"
		"$color2" "[1.0, 1.0, 1.0]"
		"$halflambert" "1"
		"$envmap" "env_cubemap"
		"$envmaptint" "[0.1 0.1 0.1]"
		"$envmapfresnel" "1.0"
		"$envmapfresnelminmaxexp" "[1.0, 1.0, 1.0]"
		"$phong" "1"
		"$phongexponent" "1024"
		"$phongboost" "4.0"
		"$phongfresnelranges" "[1.0, 1.0, 1.0]"
		"$rimlight" "1"
		"$rimlightexponent" "4.0"
		"$rimlightboost" "2.0"
    
		"Proxies"
		{
			"TextureScroll"
			{
				"textureScrollVar" "$bumptransform"
				"textureScrollRate" "0.25"
				"textureScrollAngle" "0.0"
			}
		}
	}
)#";

	std::ofstream("csgo/materials/ghost.vmt") << R"#("VertexLitGeneric" {
"$basetexture" "vgui/white_additive"
	  "$ignorez" "0"
	  "$additive" "0"
	  "$envmap"  "models/effects/cube_white"
	  "$normalmapalphaenvmapmask" "1"
	  "$envmaptint" "[0.12 0.1 0.22]"
	  "$envmapfresnel" "1"
	  "$envmapfresnelminmaxexp" "[0 1 2]"
	  "$envmapcontrast" "1"
	  "$nofog" "1"
	  "$model" "1"
	  "$nocull" "0"
	  "$selfillum" "1"
	  "$halflambert" "1"
	  "$znearer" "0"
	  "$flat" "1"
	})#";

	std::ofstream("csgo\\materials\\simple_regular_reflective.vmt") << R"#("VertexLitGeneric" {
            "$basetexture" "vgui/white_additive"
            "$ignorez"      "0"
            "$envmap"       "env_cubemap"
            "$normalmapalphaenvmapmask"  "1"
            "$envmapcontrast"             "1"
            "$nofog"        "1"
            "$model"        "1"
            "$nocull"       "0"
            "$selfillum"    "1"
            "$halflambert"  "1"
            "$znearer"      "0"
            "$flat"         "1"
    })#";

	std::ofstream("csgo/materials/onetap_overlay2.vmt") << R"#("VertexLitGeneric"
{
			"$basetexture"				    "vgui/white"
			"$envmap"						"env_cubemap"
			"$envmaptint"                   "[.10 .10 .10]"
			"$pearlescent"					"0"
			"$phong"						"1"
			"$phongexponent"				"10"
			"$phongboost"					"1.0"
			"$rimlight"					    "1"
			"$rimlightexponent"		        "1"
			"$rimlightboost"		        "1"
			"$model"						"1"
			"$nocull"						"0"
			"$halflambert"				    "1"
			"$lightwarptexture"             "metalic"
}
)#";


	// find stupid materials.
	debugambientcube = g_csgo.m_material_system->FindMaterial(XOR("debug/debugambientcube"), XOR("Model textures")); // texture
	debugambientcube->IncrementReferenceCount();

	debugdrawflat = g_csgo.m_material_system->FindMaterial(XOR("debug/debugdrawflat"), XOR("Model textures"));
	debugdrawflat->IncrementReferenceCount();

	silhouette = g_csgo.m_material_system->FindMaterial("onetap_overlay", "Model textures");
	silhouette->IncrementReferenceCount();

	ghost = g_csgo.m_material_system->FindMaterial(XOR("ghost"), XOR("Model textures")); // glow
	ghost->IncrementReferenceCount();

	materialMetallnZ = g_csgo.m_material_system->FindMaterial("simple_regular_reflective", "Model textures"); // shine
	materialMetallnZ->IncrementReferenceCount();

	glow_armsrace = g_csgo.m_material_system->FindMaterial("onetap_overlay2", "Model textures"); // metallic
	glow_armsrace->IncrementReferenceCount();
}

void Chams::DrawShotRecord(Player* ent, uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone) {

	for (int i{ }; i < m_hits->size(); i++) {
		auto data = m_hits->at(i);

		if (ent != data.ent)
			continue;

		if (!data.time || data.time + game::TIME_TO_TICKS(1) < g_csgo.m_globals->m_tick_count)
			continue;

		g_csgo.m_studio_render->ForcedMaterialOverride(silhouette);
		g_csgo.m_render_view->SetBlend(1.0f);
		SetColor({ 255, 255, 255, 255 }, color_type_t::envmap_tint);

		g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_csgo.m_model_render, ctx, state, info, data.m_last_pose);
		g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_csgo.m_model_render, ctx, state, info, bone);
		g_csgo.m_studio_render->ForcedMaterialOverride(nullptr);
	}

}

bool Chams::GenerateLerpedMatrix(int index, BoneArray* out) {
	LagRecord* current_record;
	AimPlayer* data;

	Player* ent = g_csgo.m_entlist->GetClientEntity< Player* >(index);
	if (!ent)
		return false;

	if (!g_aimbot.IsValidTarget(ent))
		return false;

	data = &g_aimbot.m_players[index - 1];
	if (!data || data->m_records.empty())
		return false;

	if (data->m_records.size() < 2)
		return false;

	auto* channel_info = g_csgo.m_engine->GetNetChannelInfo();
	if (!channel_info)
		return false;

	static float max_unlag = 0.2f;
	static auto sv_maxunlag = g_csgo.m_cvar->FindVar(HASH("sv_maxunlag"));
	if (sv_maxunlag) {
		max_unlag = sv_maxunlag->GetFloat();
	}

	for (auto it = data->m_records.rbegin(); it != data->m_records.rend(); it++) {
		current_record = it->get();

		bool end = it + 1 == data->m_records.rend();

		if (current_record && current_record->valid() && (!end && ((it + 1)->get()))) {
			if (current_record->m_origin.dist_to(ent->GetAbsOrigin()) < 1.f) {
				return false;
			}

			vec3_t next = end ? ent->GetAbsOrigin() : (it + 1)->get()->m_origin;
			float  time_next = end ? ent->m_flSimulationTime() : (it + 1)->get()->m_sim_time;

			float total_latency = channel_info->GetAvgLatency(0) + channel_info->GetAvgLatency(1);
			std::clamp(total_latency, 0.f, max_unlag);

			float correct = total_latency + g_cl.m_lerp;
			float time_delta = time_next - current_record->m_sim_time;
			float add = end ? 1.f : time_delta;
			float deadtime = current_record->m_sim_time + correct + add;

			float curtime = g_csgo.m_globals->m_curtime;
			float delta = deadtime - curtime;

			float mul = 1.f / add;
			vec3_t lerp = math::Interpolate(next, current_record->m_origin, std::clamp(delta * mul, 0.f, 1.f));

			matrix3x4_t ret[128];

			std::memcpy(ret,
				current_record->m_bones,
				sizeof(ret));

			for (size_t i{ }; i < 128; ++i) {
				vec3_t matrix_delta = current_record->m_bones[i].GetOrigin() - current_record->m_origin;
				ret[i].SetOrigin(matrix_delta + lerp);
			}

			std::memcpy(out,
				ret,
				sizeof(ret));

			return true;
		}
	}

	return false;
}

void Chams::RenderHistoryChams(int index) {
	AimPlayer* data;

	Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(index);
	if (!player)
		return;

	if (player->dormant())
		return;

	if (!g_aimbot.IsValidTarget(player))
		return;

	bool enemy = g_cl.m_local && player->enemy(g_cl.m_local);
	if (enemy) {
		data = &g_aimbot.m_players[index - 1];
		if (!data)
			return;

		// get color.
		Color color = g_menu.main.players.chams_enemy_history_col.get();

		IMaterial* material;

		switch (g_menu.main.players.chams_enemy_history_mat.get())
		{
		case 1:
			material = debugdrawflat; break;
		case 2:
			material = debugambientcube; break;
		case 3:
			material = silhouette; break;
		case 4:
			material = materialMetallnZ; break;
		case 5:
			material = glow_armsrace; break;
		case 6:
			material = ghost; break;
		default:
			material = debugdrawflat; break;
		}

		material->ColorModulate(color);

		// override blend.
		SetAlpha(color.a() / 255.f);

		// set material and color.
		g_csgo.m_studio_render->ForcedMaterialOverride(material);

		// was the matrix properly setup?
		BoneArray arr[128];
		if (Chams::GenerateLerpedMatrix(index, arr)) {
			// backup the bone cache before we fuck with it.
			auto backup_bones = player->m_BoneCache().m_pCachedBones;

			// replace their bone cache with our custom one.
			player->m_BoneCache().m_pCachedBones = arr;

			// manually draw the model.
			player->DrawModel();

			// reset their bone cache to the previous one.
			player->m_BoneCache().m_pCachedBones = backup_bones;
		}
	}
}

void Chams::ApplyChams(IMaterial* matt, std::string color, float alpha, int material_flag, bool flag_value, uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone, bool chams_background) {

	if (!g_cl.m_processing)
		return;

	g_menu.main.players.chams_enemy_history_mat.get() == 1 ? debugdrawflat->SetFlag(material_flag, flag_value) : g_menu.main.players.chams_enemy_history_mat.get() == 2 ? debugambientcube->SetFlag(material_flag, flag_value) : g_menu.main.players.chams_enemy_history_mat.get() == 3 ? silhouette->SetFlag(material_flag, flag_value) : g_menu.main.players.chams_enemy_history_mat.get() == 4 ? materialMetallnZ->SetFlag(material_flag, flag_value) : g_menu.main.players.chams_enemy_history_mat.get() == 5 ? glow_armsrace->SetFlag(material_flag, flag_value) : g_menu.main.players.chams_enemy_history_mat.get() == 6 ? ghost->SetFlag(material_flag, flag_value) : 0;
	g_csgo.m_studio_render->ForcedMaterialOverride(g_menu.main.players.chams_enemy_history_mat.get() == 1 ? debugdrawflat : g_menu.main.players.chams_enemy_history_mat.get() == 2 ? debugambientcube : g_menu.main.players.chams_enemy_history_mat.get() == 3 ? silhouette : g_menu.main.players.chams_enemy_history_mat.get() == 4 ? materialMetallnZ : g_menu.main.players.chams_enemy_history_mat.get() == 5 ? glow_armsrace : g_menu.main.players.chams_enemy_history_mat.get() == 6 ? ghost : nullptr);
	g_csgo.m_render_view->SetBlend(g_menu.main.players.chams_enemy_vis.get().a() / 255.f);
	SetColor(g_menu.main.players.chams_enemy_vis.get(), color_type_t::normal);
}

bool Chams::DrawModel(uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone) {

	if (!g_csgo.m_engine->IsInGame()) {
		m_running = false;
		return true;
	}

	//local weapon
	//if( !g_cl.m_weapon )
		//return true;

	if (g_cl.m_processing)
		m_running = true;

	if (!m_running)
		return true;

	// store the model name for later use.
	const char* name = g_csgo.m_model_info->GetModelName(info.m_model);

	float complete = (g_csgo.m_globals->m_curtime - g_visuals.m_hit_start) / 0.6f;
	int alpha = (1.f - complete) * 240;

	// arms, sleeves, weapon, v_

	if (strstr(name, XOR("arms")) && g_menu.main.players.chams_friendly_enable.get()) {
		ApplyChams(ghost, XOR("esp_chams_enemies_invis_color"), g_menu.main.players.chams_enemy_invis.get().a() / 255.f, MATERIAL_VAR_IGNOREZ, false, ctx, state, info, bone);
	}


	// not sure if this is right.
	Player* player = reinterpret_cast<Player*>(g_csgo.m_entlist->GetClientEntity(info.m_index));

	if (!player)
		return true;


	if (strstr(name, XOR("models/player"))) {

		// is an enemy.
		if (player->m_iTeamNum() != g_cl.m_local->m_iTeamNum()) {

			if (player->alive()) {

				// hidden
				if (g_menu.main.players.chams_enemy_enable.get()) {
					ApplyChams(debugdrawflat, XOR("esp_chams_enemies_invis_color"), g_menu.main.players.chams_enemy_invis.get().a() / 255.f, MATERIAL_VAR_IGNOREZ, true, ctx, state, info, bone);

					// IMPORTANT: if we don't call the original this material will get overriden.
					g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_csgo.m_model_render, ctx, state, info, bone);
				}

				// visible
				if (g_menu.main.players.chams_enemy_enable.get()) {
					ApplyChams(silhouette, XOR("esp_chams_enemies_color"), g_menu.main.players.chams_enemy_vis.get().a() / 255.f, MATERIAL_VAR_IGNOREZ, false, ctx, state, info, bone);
				}
			}

			// now we can do ragdoll chams.
		//	if (!player->alive()) {
			//	if (g_cfg[XOR("chams_enemy_ragdoll")].get< bool >()) {
					//ApplyChams(silhouette, XOR("esp_chams_enemies_color"), 255.f, MATERIAL_VAR_IGNOREZ, true, ctx, state, info, bone);
					
					//g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_csgo.m_model_render, ctx, state, info, bone);
			//}
			//}

			// do local player chams.
		}
		else if (player == g_cl.m_local) {

			if (g_menu.main.players.chams_local_enable.get()) {

				ApplyChams(debugdrawflat, XOR("chams_local_col"), 255.f, MATERIAL_VAR_IGNOREZ, false, ctx, state, info, bone);

				//g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >(IVModelRender::DRAWMODELEXECUTE)(g_csgo.m_model_render, ctx, state, info, bone);
			}

			// blend in scope

			//if (g_cl.m_local->m_bIsScoped()) {
			//	g_csgo.m_render_view->SetBlend(g_menu.main.players.chams_local_enable.get() ? 50.f : 255.f);
			//}
		}

	}


	return true;
}

void Chams::SceneEnd() {

	if (!g_cl.m_processing)
		return;

	// store and sort ents by distance.
	if (SortPlayers()) {
		// iterate each player and render them.
		for (const auto& p : m_players) {
			// check if is an enemy.
			bool enemy = g_cl.m_local && p->enemy(g_cl.m_local);

			if (enemy && g_menu.main.players.chams_enemy_history_enable.get()) {
				RenderHistoryChams(p->index());
			}
		}
	}

	// restore.
	g_csgo.m_studio_render->ForcedMaterialOverride(nullptr);
	g_csgo.m_render_view->SetColorModulation(colors::white);
	g_csgo.m_render_view->SetBlend(1.f);
}

bool Chams::IsInViewPlane(const vec3_t& world) {
	float w;

	const VMatrix& matrix = g_csgo.m_engine->WorldToScreenMatrix();

	w = matrix[3][0] * world.x + matrix[3][1] * world.y + matrix[3][2] * world.z + matrix[3][3];

	return w > 0.001f;
}

bool Chams::SortPlayers() {
	// lambda-callback for std::sort.
	// to sort the players based on distance to the local-player.
	static auto distance_predicate = [](Entity* a, Entity* b) {
		vec3_t local = g_cl.m_local->GetAbsOrigin();

		// note - dex; using squared length to save out on sqrt calls, we don't care about it anyway.
		float len1 = (a->GetAbsOrigin() - local).length_sqr();
		float len2 = (b->GetAbsOrigin() - local).length_sqr();

		return len1 < len2;
	};

	// reset player container.
	m_players.clear();

	// find all players that should be rendered.
	for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
		// get player ptr by idx.
		Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);

		// validate.
		if (!player || !player->IsPlayer() || !player->alive() || player->dormant())
			continue;

		// do not draw players occluded by view plane.
		if (!IsInViewPlane(player->WorldSpaceCenter()))
			continue;

		m_players.push_back(player);
	}

	// any players?
	if (m_players.empty())
		return false;

	// sorting fixes the weird weapon on back flickers.
	// and all the other problems regarding Z-layering in this shit game.
	std::sort(m_players.begin(), m_players.end(), distance_predicate);

	return true;
}
