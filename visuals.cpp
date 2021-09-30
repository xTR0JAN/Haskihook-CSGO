#include "includes.h"

#define MIN1(a,b) ((a) < (b) ? (a) : (b))

Visuals g_visuals{ };;

void Visuals::ModulateWorld() {
	std::vector< IMaterial* > world, props;

	// iterate material handles.
	for (uint16_t h{ g_csgo.m_material_system->FirstMaterial() }; h != g_csgo.m_material_system->InvalidMaterial(); h = g_csgo.m_material_system->NextMaterial(h)) {
		// get material from handle.
		IMaterial* mat = g_csgo.m_material_system->GetMaterial(h);
		if (!mat)
			continue;

		// store world materials.
		if (FNV1a::get(mat->GetTextureGroupName()) == HASH("World textures"))
			world.push_back(mat);

		// store props.
		else if (FNV1a::get(mat->GetTextureGroupName()) == HASH("StaticProp textures"))
			props.push_back(mat);
	}

	// night
	const float darkness = g_menu.main.world.night_darkness.get() / 100.f;

	if (g_menu.main.world.world.get() == 1) {
		for (const auto& w : world)
			w->ColorModulate(darkness, darkness, darkness);

		// IsUsingStaticPropDebugModes my nigga
		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != 0) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(0);
		}

		for (const auto& p : props)
			p->ColorModulate(0.5f, 0.5f, 0.5f);

		//game::SetSkybox(XOR("sky_csgo_night02"));
	}

	// disable night.
	else {
		for (const auto& w : world)
			w->ColorModulate(1.f, 1.f, 1.f);

		// restore r_DrawSpecificStaticProp.
		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != -1) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(-1);
		}

		for (const auto& p : props)
			p->ColorModulate(1.f, 1.f, 1.f);
	}

	// transparent props.
	if (g_menu.main.world.transparent_props.get()) {

		// IsUsingStaticPropDebugModes my nigga
		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != 0) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(0);
		}

		float alpha = g_menu.main.world.transparent_props_amount.get() / 100;
		for (const auto& p : props)
			p->AlphaModulate(alpha);
	}

	// disable transparent props.
	else {

		// restore r_DrawSpecificStaticProp.
		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != -1) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(-1);
		}

		for (const auto& p : props)
			p->AlphaModulate(1.0f);
	}
}

void gain_mouse_pos(vec2_t& last, vec2_t& new_) {
	POINT p_mouse_pos;
	GetCursorPos(&p_mouse_pos);
	ScreenToClient(FindWindow(0, XOR("Counter-Strike: Global Offensive")), &p_mouse_pos);
	last = new_;
	new_ = vec2_t(static_cast<int>(p_mouse_pos.x), static_cast<int>(p_mouse_pos.y));
}

void Visuals::Spectators() {
	static vec2_t					  main_mouse, last_mouse, s_drag;
	bool                              is_dragging;
	std::vector< std::string >        spectators{ XOR("") };
	vec2_t							  position = vec2_t(100, 100);

	if (!g_menu.main.visuals.spectators.get())
		return;

	// get mouse position.
	gain_mouse_pos(main_mouse, last_mouse);

	// dragging logic.
	if (main_mouse.x > position.x - s_drag.x && main_mouse.y > position.y - s_drag.y && main_mouse.x < (position.x - s_drag.x) + 200 && main_mouse.y < (position.y - s_drag.y) + 20 && GetAsyncKeyState(VK_LBUTTON)) {
		s_drag.x += main_mouse.x - last_mouse.x;
		s_drag.y += main_mouse.y - last_mouse.y;
		is_dragging = true;
	}

	// set our new position.
	position.x -= s_drag.x;
	position.y -= s_drag.y;

	if (g_csgo.m_engine->IsInGame()) {

		// collect spectators
		for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);
			if (!player)
				continue;

			if (player->m_bIsLocalPlayer())
				continue;

			if (player->dormant())
				continue;

			if (player->m_lifeState() == LIFE_ALIVE)
				continue;

			if (player->GetObserverTarget() != g_cl.m_local)
				continue;

			player_info_t info;
			if (!g_csgo.m_engine->GetPlayerInfo(i, &info))
				continue;

			spectators.push_back(std::string(info.m_name).substr(0, 24));
		}

	}

	size_t total_size = spectators.size() * (15);

	render::rect_filled(position.x, position.y + 6, 200, (spectators.size() * 18), { 20, 25, 30, 200 }); // background 

	// render background.
	render::rect_filled(position.x, position.y, 200, 19, Color(30, 35, 40, 50));

	// render line
	//render::DrawRectGradientHorizontal(position.x, position.y - 19, 30, 4, g_menu.main.config.menu_color.get(), Color(180, 180, 180, 195));
	//render::DrawRectGradientHorizontal(position.x + 30, position.y - 5, 30, 4, Color(180, 180, 180, 195), g_menu.main.config.menu_color.get());
	//render::DrawRectGradientHorizontal(position.x + 60, position.y - 5, 140, 4, g_menu.main.config.menu_color.get(), g_menu.main.config.menu_color.get());

	render::DrawRectGradientHorizontal(position.x, position.y + 5, 30, 2, g_menu.main.config.menu_color.get(), g_menu.main.config.menu_color.get());
	render::DrawRectGradientHorizontal(position.x + 30, position.y + 5, 30, 2, g_menu.main.config.menu_color.get(), g_menu.main.config.menu_color.get());
	render::DrawRectGradientHorizontal(position.x + 60, position.y + 5, 140, 2, g_menu.main.config.menu_color.get(), g_menu.main.config.menu_color.get());

	//render::DrawRectGradientHorizontal(position.x, position.y, 30, 2, g_menu.main.config.menu_color.get(), Color(180, 180, 180, 195));
	//render::DrawRectGradientHorizontal(position.x + 30, position.y, 30, 2, Color(180, 180, 180, 195), g_menu.main.config.menu_color.get());
	//render::DrawRectGradientHorizontal(position.x + 30, position.y, 30, 2, Color(180, 180, 180, 195), g_menu.main.config.menu_color.get());


	// render text.
	render::esp.string(position.x + 130, position.y + 8, { 255,255,255,255 }, XOR("spectators"), render::ALIGN_RIGHT);

	if (spectators.empty()) {
		return;
	}

	for (size_t i{}; i < spectators.size(); ++i) {
		const std::string& name = spectators[i];

		render::esp.string(position.x + 4, (position.y + 8) + (i * (18)),
			{ 255,255,255,255 }, name, render::ALIGN_LEFT);
	}
}

void Visuals::ThirdpersonThink() {
	ang_t                          offset;
	vec3_t                         origin, forward;
	static CTraceFilterSimple_game filter{ };
	CGameTrace                     tr;

	// for whatever reason overrideview also gets called from the main menu.
	if (!g_csgo.m_engine->IsInGame())
		return;

	// check if we have a local player and he is alive.
	bool alive = g_cl.m_local && g_cl.m_local->alive();

	static int tp_anim = 0;

	// camera should be in thirdperson.
	if (m_thirdperson) {

		// if alive and not in thirdperson already switch to thirdperson.
		if (alive && !g_csgo.m_input->CAM_IsThirdPerson())
			g_csgo.m_input->CAM_ToThirdPerson();

		// if dead and spectating in firstperson switch to thirdperson.
		else if (g_cl.m_local->m_iObserverMode() == 4) {

			// if in thirdperson, switch to firstperson.
			// we need to disable thirdperson to spectate properly.
			if (g_csgo.m_input->CAM_IsThirdPerson()) {
				g_csgo.m_input->CAM_ToFirstPerson();
				g_csgo.m_input->m_camera_offset.z = 0.f;
			}

			g_cl.m_local->m_iObserverMode() = 5;
		}
	}

	// camera should be in firstperson.
	else if (g_csgo.m_input->CAM_IsThirdPerson()) {
		g_csgo.m_input->CAM_ToFirstPerson();
		g_csgo.m_input->m_camera_offset.z = 0.f;
	}

	// if after all of this we are still in thirdperson.
	if (g_csgo.m_input->CAM_IsThirdPerson()) {

		// get camera angles.
		g_csgo.m_engine->GetViewAngles(offset);

		// get our viewangle's forward directional vector.
		math::AngleVectors(offset, &forward);

		// cam_idealdist convar.
		offset.z = g_menu.main.world.thirdperson_distance.get();

		// start pos.
		origin = g_cl.m_shoot_pos;

		// setup trace filter and trace.
		filter.SetPassEntity(g_cl.m_local);

		g_csgo.m_engine_trace->TraceRay(
			Ray(origin, origin - (forward * offset.z), { -16.f, -16.f, -16.f }, { 16.f, 16.f, 16.f }), MASK_NPCWORLDSTATIC, (ITraceFilter*)&filter, &tr);

		// adapt distance to travel time.
		math::clamp(tr.m_fraction, 0.f, 1.f);
		offset.z *= tr.m_fraction;

		static float old_frac = 0;

		// animated thirdperson based on distance
		old_frac = math::interpolate(old_frac, tr.m_fraction, (tr.m_fraction < old_frac&& old_frac - tr.m_fraction > 0.1f) ? 0.125f : (g_csgo.m_globals->m_frametime * 3.4f));

		offset.z *= old_frac;

		// override camera angles.
		g_csgo.m_input->m_camera_offset = { offset.x, offset.y, offset.z };
	}
}

// meme...
void Visuals::IndicateAngles()
{
	if (!g_csgo.m_engine->IsInGame() && !g_csgo.m_engine->IsConnected())
		return;

	if (!g_menu.main.players.draw_angles.get())
		return;

	if (!g_csgo.m_input->CAM_IsThirdPerson())
		return;

	if (!g_cl.m_local || g_cl.m_local->m_iHealth() < 1)
		return;

	const auto& pos = g_cl.m_local->GetRenderOrigin();
	vec2_t tmp;

	if (render::WorldToScreen(pos, tmp))
	{
		vec2_t draw_tmp;
		const vec3_t real_pos(50.f * std::cos(math::deg_to_rad(g_cl.m_radar.y)) + pos.x, 50.f * sin(math::deg_to_rad(g_cl.m_radar.y)) + pos.y, pos.z);

		if (render::WorldToScreen(real_pos, draw_tmp))
		{
			render::line(tmp.x, tmp.y, draw_tmp.x, draw_tmp.y, { 0, 220, 0, 255 });
			render::esp_small.string(draw_tmp.x, draw_tmp.y, { 0, 220, 0, 255 }, "FAKE", render::ALIGN_LEFT);
		}

		if (g_menu.main.antiaim.fake_yaw.get())
		{
			const vec3_t fake_pos(50.f * cos(math::deg_to_rad(g_cl.m_angle.y)) + pos.x, 50.f * sin(math::deg_to_rad(g_cl.m_angle.y)) + pos.y, pos.z);

			if (render::WorldToScreen(fake_pos, draw_tmp))
			{
				render::line(tmp.x, tmp.y, draw_tmp.x, draw_tmp.y, { 33, 162, 237, 255 });
				render::esp_small.string(draw_tmp.x, draw_tmp.y, { 33, 162, 237, 255 }, "REAL", render::ALIGN_LEFT);
			}
		}

		if (g_menu.main.antiaim.body_fake_stand.get() == 1 || g_menu.main.antiaim.body_fake_stand.get() == 2 || g_menu.main.antiaim.body_fake_stand.get() == 3 || g_menu.main.antiaim.body_fake_stand.get() == 4 || g_menu.main.antiaim.body_fake_stand.get() == 5 || g_menu.main.antiaim.body_fake_stand.get() == 6)
		{
			float lby = g_cl.m_local->m_flLowerBodyYawTarget();
			const vec3_t lby_pos(50.f * cos(math::deg_to_rad(lby)) + pos.x,
				50.f * sin(math::deg_to_rad(lby)) + pos.y, pos.z);

			if (render::WorldToScreen(lby_pos, draw_tmp))
			{
				render::line(tmp.x, tmp.y, draw_tmp.x, draw_tmp.y, { 220, 0, 0, 255 });
				render::esp_small.string(draw_tmp.x, draw_tmp.y, { 220, 0, 0, 255 }, "LBY", render::ALIGN_LEFT);
			}
		}
	}
}

void Visuals::DrawDamageIndicator()
{
	Color  color;

	float complete = (g_csgo.m_globals->m_curtime - m_hit_start) / 2.0f;
	int alpha = (1.f - complete) * 240;

	if (!g_menu.main.players.draw_damage.get() || !g_cl.m_local || !g_cl.m_local->alive())
		return;

	float CurrentTime = g_cl.m_local->m_nTickBase() * g_csgo.m_globals->m_interval;

	for (int i = 0; i < g_cl.DamageIndicator.size(); ++i)
	{
		if (g_cl.DamageIndicator[i].flEraseTime < CurrentTime)
		{
			g_cl.DamageIndicator.erase(g_cl.DamageIndicator.begin() + i);
			continue;
		}

		if (!g_cl.DamageIndicator[i].bInitialized)
		{
			g_cl.DamageIndicator[i].Position = g_cl.DamageIndicator[i].Player->GetBonePosition(HITBOX_HEAD);
			g_cl.DamageIndicator[i].bInitialized = true;
		}

		if (CurrentTime - g_cl.DamageIndicator[i].flLastUpdate > 0.0001f)
		{
			g_cl.DamageIndicator[i].Position.z -= (0.5f * (CurrentTime - g_cl.DamageIndicator[i].flEraseTime));
			g_cl.DamageIndicator[i].flLastUpdate = CurrentTime;
		}

		vec2_t ScreenPosition;

		if (render::WorldToScreen(g_cl.DamageIndicator[i].Position, ScreenPosition))
		{
			render::esp_small.string(ScreenPosition.x, ScreenPosition.y, { 255,255,255, alpha }, std::to_string(g_cl.DamageIndicator[i].iDamage).c_str());
		}
	}
}

constexpr int linesize = 5, linedec = 11;
void Visuals::Hitmarker() {

	static auto cross = g_csgo.m_cvar->FindVar(HASH("weapon_debug_spread_show"));
	cross->SetValue(g_menu.main.visuals.force_xhair.get() && !g_cl.m_local->m_bIsScoped() ? 3 : 0); // force crosshair
	if (!g_menu.main.misc.hitmarker.get())
		return;

	if (g_csgo.m_globals->m_curtime > m_hit_end)
		return;

	if (m_hit_duration <= 0.f)
		return;

	float complete = (g_csgo.m_globals->m_curtime - m_hit_start) / m_hit_duration;

	int x = g_cl.m_width,
		y = g_cl.m_height,
		alpha = (1.f - complete) * 240;

	constexpr int line{ 6 };

	/*render::line(x / 2 - 5, y / 2 - 5, x / 2 - 9, y / 2 - 9, { 255, 255, 255, alpha });
	render::line(x / 2 + 5, y / 2 - 5, x / 2 + 9, y / 2 - 9, { 255, 255, 255, alpha });
	render::line(x / 2 - 5, y / 2 + 5, x / 2 - 9, y / 2 + 9, { 255, 255, 255, alpha });
	render::line(x / 2 + 5, y / 2 + 5, x / 2 + 9, y / 2 + 9, { 255, 255, 255, alpha });*/

	/*if (g_shots.iHeadshot) {
		render::line(x / 2 - linesize, y / 2 - linesize, x / 2 - linedec, y / 2 - linedec, { 255, 215, 0, alpha });
		render::line(x / 2 - linesize, y / 2 + linesize, x / 2 - linedec, y / 2 + linedec, { 255, 215, 0, alpha });
		render::line(x / 2 + linesize, y / 2 + linesize, x / 2 + linedec, y / 2 + linedec, { 255, 215, 0, alpha });
		render::line(x / 2 + linesize, y / 2 - linesize, x / 2 + linedec, y / 2 - linedec, { 255, 215, 0, alpha });
	}
	else {*/
		render::line(x / 2 - linesize, y / 2 - linesize, x / 2 - linedec, y / 2 - linedec, { 255, 255, 255, alpha });
		render::line(x / 2 - linesize, y / 2 + linesize, x / 2 - linedec, y / 2 + linedec, { 255, 255, 255, alpha });
		render::line(x / 2 + linesize, y / 2 + linesize, x / 2 + linedec, y / 2 + linedec, { 255, 255, 255, alpha });
		render::line(x / 2 + linesize, y / 2 - linesize, x / 2 + linedec, y / 2 - linedec, { 255, 255, 255, alpha });
	//}
}

void Visuals::HitmarkerWorld() {
	if (!g_menu.main.misc.hitmarker_world.get())
		return;

	for (size_t i{ }; i < m_world_hitmarkers.size(); ++i) {
		WorldHitmarkerData_t& info = m_world_hitmarkers[i];

		// If the delta between the current time and hurt time is larger than 2 seconds then we should erase
		if (g_csgo.m_globals->m_curtime - info.m_time > 2.0f) {
			info.m_alpha -= (1.0f / 2.0f) * g_csgo.m_globals->m_frametime;
			info.m_alpha = std::clamp<float>(info.m_alpha, 0.0f, 1.0f);
		}

		if (info.m_alpha <= 0.0f) {
			continue;
		}

		if (info.m_world_to_screen) {
			constexpr int line{ 8 };

			auto draw_lines = [&](vec2_t pos_custom, Color clr) -> void {
				render::DrawLine(
					pos_custom.x - 6, pos_custom.y - 6,
					pos_custom.x - 2, pos_custom.y - 2,
					clr.malpha(info.m_alpha), true);

				render::DrawLine(
					pos_custom.x - 6, pos_custom.y + 6,
					pos_custom.x - 2, pos_custom.y + 2,
					clr.malpha(info.m_alpha), true);

				render::DrawLine(
					pos_custom.x + 6, pos_custom.y + 6,
					pos_custom.x + 2, pos_custom.y + 2,
					clr.malpha(info.m_alpha), true);

				render::DrawLine(
					pos_custom.x + 6, pos_custom.y - 6,
					pos_custom.x + 2, pos_custom.y - 2,
					clr.malpha(info.m_alpha), true);

			};
			// hitmarker
			draw_lines(info.m_world_pos, Color(255, 255, 255, 255));
		}
	}
}

void Visuals::NoSmoke() {
	if (!g_menu.main.visuals.removes.get(1))
		return;
	//colado https://www.unknowncheats.me/forum/counterstrike-global-offensive/262635-epic-wireframe-smoke.html
	std::vector<const char*> vistasmoke_mats =
	{
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_emods"
	};

	for (auto mat_s : vistasmoke_mats)
	{
		IMaterial* mat = g_csgo.m_material_system->FindMaterial(mat_s, XOR("Other textures"));
		mat->SetFlag(MATERIAL_VAR_NO_DRAW, true);
	}
}

void Visuals::think() {
	// don't run anything if our local player isn't valid.
	if (!g_cl.m_local)
		return;

	if (g_menu.main.visuals.removes.get(4)
		&& g_cl.m_local->alive()
		&& g_cl.m_local->GetActiveWeapon()
		&& g_cl.m_local->GetActiveWeapon()->GetWpnData()->m_weapon_type == CSWeaponType::WEAPONTYPE_SNIPER_RIFLE
		&& g_cl.m_local->m_bIsScoped()) {

		// rebuild the original scope lines.
		int w = g_cl.m_width,
			h = g_cl.m_height,
			x = w / 2,
			y = h / 2,
			size = g_csgo.cl_crosshair_sniper_width->GetInt();

		// Here We Use The Euclidean distance To Get The Polar-Rectangular Conversion Formula.
		if (size > 1) {
			x -= (size / 2);
			y -= (size / 2);
		}

		Weapon* weapon = g_cl.m_local->GetActiveWeapon();

		float inacc = weapon->GetInaccuracy();

		int pixels = inacc * 75;

		switch (g_menu.main.visuals.no_scope_type.get()) {
		case 0: // default scoppe
			// draw our lines.
			render::rect_filled(0, y, w, size, colors::black);
			render::rect_filled(x, 0, size, h, colors::black);
			break;
		case 1: // dynamic scope feat monolith coder
			

			render::gradient1337reverse(x - 1, y - g_menu.main.visuals.gradientsize.get() / 1.5, 2, g_menu.main.visuals.gradientsize.get() / 1.5 - 10, g_menu.main.visuals.gradientcol.get(), g_menu.main.visuals.gradientcol.get(), false);

			render::gradient1337reverse(x - g_menu.main.visuals.gradientsize.get() / 1.5, y - 1, g_menu.main.visuals.gradientsize.get() / 1.5 - 10, 2, g_menu.main.visuals.gradientcol.get(), g_menu.main.visuals.gradientcol.get(), true);

			render::gradient1337(x - 1, y + 10, 2, g_menu.main.visuals.gradientsize.get() / 1.5 - 10, g_menu.main.visuals.gradientcol.get(), g_menu.main.visuals.gradientcol.get(), false);

			render::gradient1337(x + 10, y - 1, g_menu.main.visuals.gradientsize.get() / 1.5 - 10, 2, g_menu.main.visuals.gradientcol.get(), g_menu.main.visuals.gradientcol.get(), true);
		}
	}

	// draw esp on ents.
	// draw esp on ents.
	for (int i{ 1 }; i <= g_csgo.m_entlist->GetHighestEntityIndex(); ++i) {
		Entity* ent = g_csgo.m_entlist->GetClientEntity(i);
		if (!ent)
			continue;

		draw(ent);
	}

	// draw everything else.
	SpreadCrosshair();
	StatusIndicators();
	Spectators();
	Keybinds();
	PenetrationCrosshair();
	ManualAntiAim();
	DrawDamageIndicator();
	Hitmarker();
	DrawPlantedC4();

	// world to screen world hitmarkers in a game thread.
	for (size_t i{ }; i < m_world_hitmarkers.size(); ++i) {
		WorldHitmarkerData_t& info = m_world_hitmarkers[i];
		info.m_world_to_screen = render::WorldToScreen(vec3_t(info.m_pos_x, info.m_pos_y, info.m_pos_z), info.m_world_pos);
	}
}

void Visuals::StatusIndicators() {
	// dont do if dead.
	if (!g_cl.m_processing)
		return;

	struct Indicator_t { Color color; std::string text; };
	std::vector< Indicator_t > indicators{ };

	static auto alpha = 1.0f;
	static auto switch_alpha = false;

	if (alpha <= 0.0f || alpha >= 1.0f)
		switch_alpha = !switch_alpha;

	alpha += switch_alpha ? 2.0f * g_csgo.m_globals->m_frametime : -2.0f * g_csgo.m_globals->m_frametime;
	alpha = math::clamp1(alpha, 0.0f, 1.0f);

	// LC
	if (g_menu.main.visuals.indicators.get(1) && ((g_cl.m_buttons & IN_JUMP) || !(g_cl.m_flags & FL_ONGROUND))) {
		Indicator_t ind{ };
		ind.color = g_cl.m_lagcomp ? 0xff15c27b : 0xff0000ff;
		ind.text = XOR("LC");

		indicators.push_back(ind);
	}

	// LBY
	if (g_menu.main.visuals.indicators.get(0)) {
		// get the absolute change between current lby and animated angle.
		float change = std::abs(math::NormalizedAngle(g_cl.m_body - g_cl.m_angle.y));

		Indicator_t ind{ };
		ind.color = change > 35.f ? Color(167, 252, 0, 255 * alpha) : Color(239, 48, 56, 255);
		ind.text = XOR("LBY");
		indicators.push_back(ind);
	}


	// PING
	if (g_menu.main.visuals.indicators.get(2)) {
		Indicator_t ind{ };
		ind.color = g_aimbot.m_fake_latency ? 0xff15c27b : 0xff0000ff;
		ind.text = XOR("PING");

		indicators.push_back(ind);
	}

	if (g_menu.main.visuals.indicators.get(3) && g_aimbot.m_damage_toggle) {
		Indicator_t ind{ };
		ind.color = 0xff15c27b;
		ind.text = XOR("DMG");

		indicators.push_back(ind);
	}
	if (g_menu.main.visuals.indicators.get(4) && g_aimbot.m_double_tap) {
		Indicator_t ind{ };
		ind.color = 0xff15c27b;
		ind.text = XOR("DT");

		indicators.push_back(ind);
	}

	if (indicators.empty())
		return;

	// iterate and draw indicators.
	for (size_t i{ }; i < indicators.size(); ++i) {
		auto& indicator = indicators[i];

		render::indicator.string(12, g_cl.m_height - 64 - (30 * i), indicator.color, indicator.text);
	}


	auto local_player = g_cl.m_local;
	int screen_width, screen_height;
	g_csgo.m_engine->GetScreenSize(screen_width, screen_height);

	static float next_lby_update[65];
	//static float last_lby[65];

	const float curtime = g_csgo.m_globals->m_curtime;

	//if (local_player->GetVelocity().Length2D() > 0.1 && !global::is_fakewalking)
	//    return;

	if (local_player->m_vecVelocity().length_2d() > 0.1f && !g_input.GetKeyState(g_menu.main.movement.fakewalk.get()))
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return;
	static float last_lby[65];
	if (last_lby[local_player->index()] != local_player->m_flLowerBodyYawTarget())
	{
		last_lby[local_player->index()] = local_player->m_flLowerBodyYawTarget();
		next_lby_update[local_player->index()] = curtime + 1.125f + g_csgo.m_globals->m_interval;
	}

	if (next_lby_update[local_player->index()] < curtime)
	{
		next_lby_update[local_player->index()] = curtime + 1.125f;
	}

	float time_remain_to_update = next_lby_update[local_player->index()] - local_player->m_flSimulationTime();
	float time_update = next_lby_update[local_player->index()];


	float fill = 0;
	fill = (((time_remain_to_update)));
	static float add = 0.000f;
	add = 1.125f - fill;

	float change1337 = std::abs(math::NormalizedAngle(g_cl.m_body - g_cl.m_angle.y));

	Color color1337 = { 255,0,0,255 };

	if (change1337 > 35.f) {
		color1337 = { 124,195,13,255 };
	}

	// lby bar as line
	//render::rect_filled(14, g_cl.m_height - 74 + 26, 48, 4, { 10, 10, 10, 125 });
	//render::rect_filled(15, g_cl.m_height - 74 + 27, add * 40, 2, color1337);

	// useless
	//render::arccircle(12 + 60, g_cl.m_height - 74 + 23 - 9, 5, 9, 0, 360, { 0,0,0,50 });
	//render::arccircle(12 + 60, g_cl.m_height - 74 + 23 - 9, 6, 8, 0, 340 * add, color1337);
	//render::drawCircle(90, 87, 100, { 255,255,255,255 });

	// circle lby
	if (!((g_cl.m_buttons & IN_JUMP) || !(g_cl.m_flags & FL_ONGROUND)) && g_menu.main.visuals.indicators.get(0)) {
		render::draw_arc(12 + 60, g_cl.m_height - 64 + 23 - 9, 8, 0, 360, 5, { 0,0,0,125 });
		render::draw_arc(12 + 60, g_cl.m_height - 64 + 23 - 9, 7, 0, 340 * add, 3, color1337);
	}
}

void Visuals::SpreadCrosshair() {
	// dont do if dead.
	if (!g_cl.m_processing)
		return;

	if (!g_menu.main.visuals.spread_xhair.get())
		return;

	// get active weapon.
	Weapon* weapon = g_cl.m_local->GetActiveWeapon();
	if (!weapon)
		return;

	WeaponInfo* data = weapon->GetWpnData();
	if (!data)
		return;

	// do not do this on: bomb, knife and nades.
	CSWeaponType type = data->m_weapon_type;
	if (type == WEAPONTYPE_KNIFE || type == WEAPONTYPE_C4 || type == WEAPONTYPE_GRENADE)
		return;

	// moneybot spread circle... change later to normal cuz fps drops if it on or just get_random_float less
	if (weapon) {
		int screen_w, screen_h;
		g_csgo.m_engine->GetScreenSize(screen_w, screen_h);
		int cross_x = screen_w / 2, cross_y = screen_h / 2;

		float recoil_step = screen_h / g_cl.m_fov;

		cross_x -= (int)(g_cl.m_local->m_aimPunchAngle().y * recoil_step);
		cross_y += (int)(g_cl.m_local->m_aimPunchAngle().x * recoil_step);

		weapon->UpdateAccuracyPenalty();
		float inaccuracy = weapon->GetInaccuracy();
		float spread = weapon->GetSpread();

		float cone = inaccuracy * spread;
		cone *= screen_h * 0.7f;
		cone *= 90.f / g_cl.m_fov;

		for (int seed{ }; seed < 256; ++seed) {
			render::set_random_seed(math::random_number(0, 255) + 1);
			float rand_a = render::get_random_float(0.f, 1.0f);
			float pi_rand_a = render::get_random_float(0.f, 2.0f * M_PI);
			float rand_b = render::get_random_float(0.0f, 1.0f);
			float pi_rand_b = render::get_random_float(0.f, 2.f * M_PI);

			float spread_x = cos(pi_rand_a) * (rand_a * inaccuracy) + cos(pi_rand_b) * (rand_b * spread);
			float spread_y = sin(pi_rand_a) * (rand_a * inaccuracy) + sin(pi_rand_b) * (rand_b * spread);

			float max_x = cos(pi_rand_a) * cone + cos(pi_rand_b) * cone;
			float max_y = sin(pi_rand_a) * cone + sin(pi_rand_b) * cone;

			float step = screen_h / g_cl.m_fov * 90.f;
			int screen_spread_x = (int)(spread_x * step * 0.7f);
			int screen_spread_y = (int)(spread_y * step * 0.7f);

			float percentage = (rand_a * inaccuracy + rand_b * spread) / (inaccuracy + spread);

			render::rect_filled(cross_x + screen_spread_x, cross_y + screen_spread_y, 1, 1, Color(255, 255, 255, 255 * (0.4f + percentage * 0.6f) * (0.1f + percentage * 0.9f)));
		}
	}
}

void Visuals::ManualAntiAim() {
	if (!g_cl.m_processing)
		return;

	if (!g_menu.main.antiaim.manul_antiaim.get())
		return;

	int width, height;
	g_csgo.m_engine->GetScreenSize(width, height);

	static auto alpha = 1.0f;
	static auto switch_alpha = false;

	if (alpha <= 0.0f || alpha >= 1.0f)
		switch_alpha = !switch_alpha;

	alpha += switch_alpha ? 2.0f * g_csgo.m_globals->m_frametime : -2.0f * g_csgo.m_globals->m_frametime;
	alpha = math::clamp1(alpha, 0.0f, 1.0f);

	Color color = Color(g_menu.main.antiaim.color_manul_antiaim.get().r(), g_menu.main.antiaim.color_manul_antiaim.get().g(), g_menu.main.antiaim.color_manul_antiaim.get().b(), 255 * alpha);

	// why would we render manual indicators how coder of skeet.idb do it - as pixels haha
	if (g_hvh.m_left) {
		render::triangle(vec2_t(width / 2 - 45, height / 2 + 10), vec2_t(width / 2 - 65, height / 2), vec2_t(width / 2 - 45, height / 2 - 10), color);
	}
	else {
		render::triangle(vec2_t(width / 2 - 45, height / 2 + 10), vec2_t(width / 2 - 65, height / 2), vec2_t(width / 2 - 45, height / 2 - 10), { 0, 0, 0, 125 });
	}

	if (g_hvh.m_right) {
		render::triangle(vec2_t(width / 2 + 45, height / 2 - 10), vec2_t(width / 2 + 65, height / 2), vec2_t(width / 2 + 45, height / 2 + 10), color);
	}
	else {
		render::triangle(vec2_t(width / 2 + 45, height / 2 - 10), vec2_t(width / 2 + 65, height / 2), vec2_t(width / 2 + 45, height / 2 + 10), { 0, 0, 0, 125 });
	}
	if (g_hvh.m_back) {
		render::triangle(vec2_t(width / 2, height / 2 + 70), vec2_t(width / 2 - 10, height / 2 + 50), vec2_t(width / 2 + 10, height / 2 + 50), color);
	}
	else {
		render::triangle(vec2_t(width / 2, height / 2 + 70), vec2_t(width / 2 - 10, height / 2 + 50), vec2_t(width / 2 + 10, height / 2 + 50), { 0, 0, 0, 125 });
	}
}

void Visuals::PenetrationCrosshair() {
	int   x, y;
	bool  valid_player_hit;
	Color final_color;

	if (!g_menu.main.visuals.pen_crosshair.get() || !g_cl.m_processing)
		return;

	x = g_cl.m_width / 2;
	y = g_cl.m_height / 2;


	valid_player_hit = (g_cl.m_pen_data.m_target && g_cl.m_pen_data.m_target->enemy(g_cl.m_local));
	if (valid_player_hit)
		final_color = colors::light_blue;

	else if (g_cl.m_pen_data.m_pen)
		final_color = colors::transparent_green;

	else
		final_color = colors::transparent_red;

	// todo - dex; use fmt library to get damage string here?
	//             draw damage string?

	// draw small square in center of screen.
	int damage1337 = g_cl.m_pen_data.m_damage;

	if (g_menu.main.visuals.pen_damage.get() && (g_cl.m_pen_data.m_pen || valid_player_hit))
		render::esp_small.string(x + 3, y + 2, { final_color }, std::to_string(damage1337).c_str(), render::ALIGN_LEFT);
	if (g_cl.m_pen_data.m_damage > 1) {
		render::line(x - 1, y, x + 1, y, final_color);
		render::line(x, y - 1, x, y + 1, final_color);
		render::rect(x - 1, y - 1, 1, 1, Color(0, 0, 0, 150)); // shadow
		render::rect(x - 1, y + 1, 1, 1, Color(0, 0, 0, 150)); // shadow
		render::rect(x + 1, y - 1, 1, 1, Color(0, 0, 0, 150)); // shadow
		render::rect(x + 1, y + 1, 1, 1, Color(0, 0, 0, 150)); // shadow
	}
}

void Visuals::draw(Entity* ent) {
	if (ent->IsPlayer()) {
		Player* player = ent->as< Player* >();

		// dont draw dead players.
		if (!player->alive())
			return;

		if (player->m_bIsLocalPlayer())
			return;

		// draw player esp.
		DrawPlayer(player);

		//update_positions();
	}

	else if ((g_menu.main.world.items.get() || g_menu.main.world.ammo.get() || g_menu.main.world.icon_item.get() || g_menu.main.world.distance.get()) && ent->IsBaseCombatWeapon() && !ent->dormant())
		DrawItem(ent->as< Weapon* >());

	else if (g_menu.main.world.proj.get())
		DrawProjectile(ent->as< Weapon* >());
}

void Visuals::DrawProjectile(Weapon* ent) {
	vec2_t screen;
	vec3_t origin = ent->GetAbsOrigin();
	if (!render::WorldToScreen(origin, screen))
		return;

	if (!g_csgo.m_engine->IsInGame() && !g_csgo.m_engine->IsConnected())
		return;

	Color col = g_menu.main.world.proj_color.get();

	auto dist_world = g_cl.m_local->m_vecOrigin().dist_to(origin);

	if (dist_world > 250.f) {
		col.a() *= std::clamp((750.f - (dist_world - 400.f)) / 750.f, 0.f, 1.f);
	}

	if (ent->is(HASH("CHostage"))) {
		std::string distance;
		int dist = (((ent->m_vecOrigin() - g_cl.m_local->m_vecOrigin()).length_sqr()) * 0.0625) * 0.001;
		if (dist < 150) {
			render::esp_small.string(screen.x, screen.y, colors::light_blue, XOR("HOSTAGE"), render::ALIGN_CENTER);
			//render::esp_small.string(screen.x, screen.y - 10, colors::light_blue, distance, render::ALIGN_CENTER);
		}
	}

	// draw decoy.
	if (ent->is(HASH("CDecoyProjectile"))) {
		render::esp_small.string(screen.x, screen.y, col, XOR("DECOY"), render::ALIGN_CENTER);
	}

	// draw molotov.
	else if (ent->is(HASH("CMolotovProjectile"))) {
		render::esp_small.string(screen.x, screen.y - 15, col, XOR("MOLLY"), render::ALIGN_CENTER);
		//render::draw_arc(screen.x, screen.y, 6, 0, 360, 1.f, { 255,255,255,255 });
	}

	else if (ent->is(HASH("CBaseCSGrenadeProjectile"))) {
		const model_t* model = ent->GetModel();

		if (model) {
			// grab modelname.
			std::string name{ ent->GetModel()->m_name };

			if (name.find(XOR("flashbang")) != std::string::npos)
				render::esp_small.string(screen.x, screen.y, col, XOR("FLASH"), render::ALIGN_CENTER);

			else if (name.find(XOR("fraggrenade")) != std::string::npos) {
				render::esp_small.string(screen.x, screen.y - 15, col, XOR("HE GRENADE"), render::ALIGN_CENTER);
				//render::draw_arc(screen.x, screen.y, 6, 0, 360, 1.f, { 255,255,255,255 });
			}
		}
	}

	//const double spawn_time_fire = *(float*)(uintptr_t(ent) + 0x20);
	//const float spawn_time_smoke = game::TICKS_TO_TIME(ent->m_nSmokeEffectTickBegin());

	static auto size = vec2_t(25.0f, 5.0f);

	auto inferno = reinterpret_cast<c_cs_inferno*>(ent);
	auto spawn_time = inferno->get_entity_spawn_time();
	auto factor = (spawn_time + c_cs_inferno::expire_time - g_csgo.m_globals->m_curtime) / c_cs_inferno::expire_time;

	auto smoke = reinterpret_cast<c_cs_inferno*>(ent);
	static constexpr float max_time = 17.5;
	const auto actual = (((spawn_time + max_time) - g_csgo.m_globals->m_curtime) / max_time);

	if (actual <= 0)
		return;

	// find classes.
	else if (ent->is(HASH("CInferno"))) {
		//if (dist_world > 400.f) 
		//render::esp_small.string(screen.x, screen.y + 3, col, XOR("FIRE"), render::ALIGN_CENTER);
		//else
			//render::DrawString(F::ESPInfo, screen.x, screen.y + 8.0, Color(255, 255, 255, 255), FONT_CENTER, "FIRE - %.1f", (spawn_time_fire + 7.031) - g_csgo.m_globals->m_curtime);

		if (dist_world > 400.f) {
			col.a() *= std::clamp((750.f - (dist_world - 400.f)) / 750.f, 0.f, 1.f);
		}

		if (g_menu.main.world.proj_range_world_circle.get()) {
			render::Draw3DFilledCircle(origin, 150, Color(176, 0, 0, col.a()));
		}

		// circle timer bar
		if (g_menu.main.world.proj_range.get(0)) {
			render::DrawFilledCircle(screen.x, screen.y - 7, 22, 180, Color(26, 26, 30, col.a()));
			render::DrawFilledCircle(screen.x, screen.y - 7, 22, 180, Color(26, 26, 30, col.a()));
			render::draw_arc(screen.x, screen.y - 7, 22, 0, 360 * factor, 1, Color(255, 0, 0, col.a()));
			render::DrawString(F::Icons, screen.x, screen.y - 21, Color(255, 255, 255, col.a()), FONT_CENTER, XOR("l"));
		}

		render::esp_small.string(screen.x, screen.y - 3, { 255,255,255, col.a() }, XOR("FIRE"), render::ALIGN_CENTER);

		// render time of fire like bar(like drop weap ammo...)
		//if (g_menu.main.visuals.proj_range.get(0)) {
		//	render::rect_filled(screen.x - size.x * 0.5f, screen.y - size.y * 0.5f - 1.0f, size.x, size.y, Color(80, 80, 80, col.a()));
		//	render::rect_filled(screen.x - size.x * 0.5f + 1.0f, screen.y - size.y * 0.5f, (size.x - 4.0f) * factor, size.y - 2.0f, Color(g_menu.main.visuals.proj_range_color.get().r(), g_menu.main.visuals.proj_range_color.get().g(), g_menu.main.visuals.proj_range_color.get().b(), col.a()));
		//	render::rect(screen.x - size.x * 0.5f, screen.y - size.y * 0.5f, size.x, size.y, Color(7, 7, 7, col.a()));
		// }
	}

	else if (ent->is(HASH("CSmokeGrenadeProjectile"))) {

		//if (dist_world > 400.f) 
		//render::esp_small.string(screen.x, screen.y + 3, col, XOR("SMOKE"), render::ALIGN_CENTER);
		//else
			//render::DrawString(F::ESPInfo, screen.x, screen.y + 8.0, Color(255, 255, 255, 255), FONT_CENTER, "SMOKE - %.1f", (spawn_time_smoke + 18.04125) - g_csgo.m_globals->m_curtime);

		if (dist_world > 400.f) {
		  col.a() *= std::clamp((750.f - (dist_world - 400.f)) / 750.f, 0.f, 1.f);
		}

		if (g_menu.main.world.proj_range_world_circle.get()) {
			render::Draw3DFilledCircle(origin, 144, Color(66, 133, 180, col.a()));
		}

		// circle timer bar
		if (g_menu.main.world.proj_range.get(1)) {
			render::DrawFilledCircle(screen.x, screen.y - 7, 22, 180, Color(26, 26, 30, col.a()));
			render::DrawFilledCircle(screen.x, screen.y - 7, 22, 180, Color(26, 26, 30, col.a()));
			render::draw_arc(screen.x, screen.y - 7, 22, 0, 360 * actual, 1, Color(66, 133, 180, col.a()));
			render::DrawString(F::Icons, screen.x, screen.y - 21, Color(255, 255, 255, col.a()), FONT_CENTER, XOR("k"));
		}

		render::esp_small.string(screen.x, screen.y - 3, { 255,255,255, col.a() }, XOR("SMOKE"), render::ALIGN_CENTER);

		// render time of smoke like bar (like drop weap ammo...)
		//if (actual > 0.f && actual < 18.f && g_menu.main.visuals.proj_range.get(1)) {
		//	render::rect_filled(screen.x - size.x * 0.5f, screen.y - size.y * 0.5f - 1.0f, size.x, size.y, Color(80, 80, 80, col.a()));
		//	render::rect_filled(screen.x - size.x * 0.5f + 1.0f, screen.y - size.y * 0.5f, (size.x - 4.0f) * actual, size.y - 2.0f, Color(g_menu.main.visuals.proj_range_color.get().r(), g_menu.main.visuals.proj_range_color.get().g(), g_menu.main.visuals.proj_range_color.get().b(), col.a()));
		//	render::rect(screen.x - size.x * 0.5f, screen.y - size.y * 0.5f, size.x, size.y, Color(7, 7, 7, col.a()));
		// }
	}
}

void Visuals::DrawItem(Weapon* item) {
	vec2_t bottom, top;
	// we only want to draw shit without owner.
	if (!g_csgo.m_engine->IsInGame() && !g_csgo.m_engine->IsConnected())
		return;

	Entity* owner = g_csgo.m_entlist->GetClientEntityFromHandle(item->m_hOwnerEntity());
	if (owner)
		return;

	std::string distance;
	int dist = (((item->m_vecOrigin() - g_cl.m_local->m_vecOrigin()).length_sqr()) * 0.0625) * 0.001;
	//if (dist > 0)
	//distance = tfm::format(XOR("%i FT"), dist);
	if (dist > 0) {
		if (dist > 5) {
			while (!(dist % 5 == 0)) {
				dist = dist - 1;
			}

			if (dist % 5 == 0)
				distance = tfm::format(XOR("%i FT"), dist);
		}
		else
			distance = tfm::format(XOR("%i FT"), dist);
	}

	// is the fucker even on the screen?
	vec2_t screen;
	vec3_t origin = item->GetAbsOrigin();
	if (!render::WorldToScreen(origin, screen))
		return;

	WeaponInfo* data = item->GetWpnData();
	if (!data)
		return;

	Color col = g_menu.main.world.item_color.get();
	Color col1337 = g_menu.main.world.dropammo_color.get();

	auto dist_world = g_cl.m_local->m_vecOrigin().dist_to(origin);

	if (dist_world > 250.f) {
		col.a() *= std::clamp((750.f - (dist_world - 250.f)) / 750.f, 0.f, 1.f);
		col1337.a() *= std::clamp((750.f - (dist_world - 250.f)) / 750.f, 0.f, 1.f);
	}

	if (g_menu.main.world.icon_item.get()) {
		const auto icon = item->getIcon();
		render::DrawString(F::Icons, screen.x, screen.y - render::cs.m_size.m_height + 31, col, FONT_CENTER, icon);
	}

	// render bomb in green.
	if (item->is(HASH("CC4")))
		render::esp_small.string(screen.x, screen.y, { 150, 200, 60, 0xb4 }, XOR("BOMB"), render::ALIGN_CENTER);

	// if not bomb
	// normal item, get its name.
	std::string name{ item->GetLocalizedName() };

	// smallfonts needs uppercase.
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);

	if (g_menu.main.world.items.get() && g_menu.main.world.distance.get())
		render::esp_small.string(screen.x, screen.y - 11, col, distance, render::ALIGN_CENTER);
	else if (!g_menu.main.world.items.get() && g_menu.main.world.distance.get())
		render::esp_small.string(screen.x, screen.y - 1, col, distance, render::ALIGN_CENTER);

	if (g_menu.main.world.items.get())
		render::esp_small.string(screen.x, screen.y - 1, col, name, render::ALIGN_CENTER);

	if (!g_menu.main.world.ammo.get())
		return;

	// nades do not have ammo.
	if (data->m_weapon_type == WEAPONTYPE_GRENADE || data->m_weapon_type == WEAPONTYPE_KNIFE)
		return;

	if (item->m_iItemDefinitionIndex() == 0 || item->m_iItemDefinitionIndex() == C4)
		return;

	std::string ammo = tfm::format(XOR("(%i/%i)"), item->m_iClip1(), item->m_iPrimaryReserveAmmoCount());

	int current = item->m_iClip1(); // clip
	int max = data->m_max_clip1; // maxclip
	float scale = (float)current / max;
	int bar = (int)std::round((26 - 2) * scale);
	render::rect_filled(screen.x - 13, screen.y + 9, 26, 4, { 0,0,0, col1337.a() });
	render::rect_filled(screen.x - 13 + 1, screen.y + 9 + 1, bar, 2, col1337);
}

void Visuals::OffScreen(Player* player, int alpha) {
	vec3_t view_origin, target_pos, delta;
	vec2_t screen_pos, offscreen_pos;
	float  leeway_x, leeway_y, radius, offscreen_rotation;
	bool   is_on_screen;
	Vertex verts[3], verts_outline[3];
	Color  color, color_outline;

	bool dormant = player->dormant();

	// todo - dex; move this?
	static auto get_offscreen_data = [](const vec3_t& delta, float radius, vec2_t& out_offscreen_pos, float& out_rotation) {
		ang_t  view_angles(g_csgo.m_view_render->m_view.m_angles);
		vec3_t fwd, right, up(0.f, 0.f, 1.f);
		float  front, side, yaw_rad, sa, ca;

		// get viewport angles forward directional vector.
		math::AngleVectors(view_angles, &fwd);

		// convert viewangles forward directional vector to a unit vector.
		fwd.z = 0.f;
		fwd.normalize();

		// calculate front / side positions.
		right = up.cross(fwd);
		front = delta.dot(fwd);
		side = delta.dot(right);

		// setup offscreen position.
		out_offscreen_pos.x = radius * -side;
		out_offscreen_pos.y = radius * -front;

		// get the rotation ( yaw, 0 - 360 ).
		out_rotation = math::rad_to_deg(std::atan2(out_offscreen_pos.x, out_offscreen_pos.y) + math::pi);

		// get needed sine / cosine values.
		yaw_rad = math::deg_to_rad(-out_rotation);
		sa = std::sin(yaw_rad);
		ca = std::cos(yaw_rad);

		// rotate offscreen position around.
		out_offscreen_pos.x = (int)((g_cl.m_width / 2.f) + (radius * sa));
		out_offscreen_pos.y = (int)((g_cl.m_height / 2.f) - (radius * ca));
	};

	if (!g_menu.main.players.offscreen.get())
		return;

	if (!g_cl.m_processing || !g_cl.m_local->enemy(player))
		return;

	// get the player's center screen position.
	target_pos = player->WorldSpaceCenter();
	is_on_screen = render::WorldToScreen(target_pos, screen_pos);

	// give some extra room for screen position to be off screen.
	leeway_x = g_cl.m_width / 18.f;
	leeway_y = g_cl.m_height / 18.f;

	// origin is not on the screen at all, get offscreen position data and start rendering.
	if (!is_on_screen
		|| screen_pos.x < -leeway_x
		|| screen_pos.x >(g_cl.m_width + leeway_x)
		|| screen_pos.y < -leeway_y
		|| screen_pos.y >(g_cl.m_height + leeway_y)) {

		float size = 100.f / 100.f;
		float pos = 250.f;

		// get viewport origin.
		view_origin = g_csgo.m_view_render->m_view.m_origin;

		// get direction to target.
		delta = (target_pos - view_origin).normalized();

		// note - dex; this is the 'YRES' macro from the source sdk.
		radius = pos * (g_cl.m_height / 480.f);

		// get the data we need for rendering.
		get_offscreen_data(delta, radius, offscreen_pos, offscreen_rotation);

		// bring rotation back into range... before rotating verts, sine and cosine needs this value inverted.
		// note - dex; reference: 
		// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/game/client/tf/tf_hud_damageindicator.cpp#L182
		offscreen_rotation = -offscreen_rotation;

		// setup vertices for the triangle.
		verts[0] = { offscreen_pos.x + (1 * size) , offscreen_pos.y + (1 * size) };        // 0,  0
		verts[1] = { offscreen_pos.x - (12.f * size), offscreen_pos.y + (24.f * size) }; // -1, 1
		verts[2] = { offscreen_pos.x + (12.f * size), offscreen_pos.y + (24.f * size) }; // 1,  1

		// setup verts for the triangle's outline.
		verts_outline[0] = { verts[0].m_pos.x - 0.2f, verts[0].m_pos.y - 0.2f };
		verts_outline[1] = { verts[1].m_pos.x - 0.2f, verts[1].m_pos.y + 0.2f };
		verts_outline[2] = { verts[2].m_pos.x + 0.2f, verts[2].m_pos.y + 0.2f };

		Weapon* weapon = player->GetActiveWeapon();
		if (weapon) {
			const auto icon = weapon->getIcon();
			if (g_menu.main.players.offscreen_icon.get()) {
				render::DrawString(F::Icons, offscreen_pos.x - 80 - (1.f * size - 80), offscreen_pos.y - 40 + (24.f * size), Color(255, 255, 255, 255), FONT_CENTER, icon);
			}
		}

		// rotate all vertices to point towards our target.
		verts[0] = render::RotateVertex(offscreen_pos, verts[0], offscreen_rotation);
		verts[1] = render::RotateVertex(offscreen_pos, verts[1], offscreen_rotation);
		verts[2] = render::RotateVertex(offscreen_pos, verts[2], offscreen_rotation);

		verts_outline[0] = render::RotateVertex(offscreen_pos, verts_outline[0], offscreen_rotation);
		verts_outline[1] = render::RotateVertex(offscreen_pos, verts_outline[1], offscreen_rotation);
		verts_outline[2] = render::RotateVertex(offscreen_pos, verts_outline[2], offscreen_rotation);

		// render!
		int alpha1337 = sin(abs(fmod(-math::pi + (g_csgo.m_globals->m_curtime * (2 / .75)), (math::pi * 2)))) * 255;

		if (alpha1337 < 0)
			alpha1337 = alpha1337 * (-1);

		if (!dormant) {
			color = { g_menu.main.players.offscreen_color.get().r(), g_menu.main.players.offscreen_color.get().g(), g_menu.main.players.offscreen_color.get().b(), g_menu.main.players.offscreen_color.get().a() = 88 }; // damage_data.m_color;
			color_outline = { g_menu.main.players.offscreen_color.get().r(), g_menu.main.players.offscreen_color.get().g(), g_menu.main.players.offscreen_color.get().b(), g_menu.main.players.offscreen_color.get().a() = 255 };
		}
		else {
			color =  { 255,255,255,g_menu.main.players.offscreen_color.get().a() = 44 };
			color_outline = { 255,255,255, g_menu.main.players.offscreen_color.get().a() = 120 };
		}

		g_csgo.m_surface->DrawSetColor(color); // filled
		g_csgo.m_surface->DrawTexturedPolygon(3, verts);
		g_csgo.m_surface->DrawSetColor(color_outline); // outline
		g_csgo.m_surface->DrawTexturedPolyLine(3, verts_outline);
	}
}

void Visuals::DrawPlayer(Player* player) {
	constexpr float MAX_DORMANT_TIME = 10.f;
	constexpr float DORMANT_FADE_TIME = MAX_DORMANT_TIME / 2.f;

	Rect		  box;
	player_info_t info;
	Color		  color;

	// get player index.
	int index = player->index();

	// now store our dormant data.
	//g_dormancy.SoundDormancy();

	// get reference to array variable.
	float& opacity = m_opacities[index - 1];
	bool& draw = m_draw[index - 1];

	// opacity should reach 1 in 300 milliseconds.
	constexpr int frequency = 1.f / 0.3f;

	// the increment / decrement per frame.
	float step = frequency * g_csgo.m_globals->m_frametime;

	// is player enemy.
	bool enemy = player->enemy(g_cl.m_local);
	bool dormant = player->dormant();

	if (g_menu.main.world.enemy_radar.get() && enemy && !dormant)
		player->m_bSpotted() = true;

	// we can draw this player again.
	if (!dormant)
		draw = true;

	if (!draw)
		return;

	// if non-dormant	-> increment
	// if dormant		-> decrement
	dormant ? opacity -= step : opacity += step;

	// is dormant esp enabled for this player.
	bool dormant_esp = enemy && g_menu.main.players.dormant.get();

	// clamp the opacity.
	math::clamp(opacity, 0.f, 1.f);
	if (!opacity && !dormant_esp)
		return;

	// stay for x seconds max.
	float dt = g_csgo.m_globals->m_curtime - player->m_flSimulationTime();
	if (dormant && dt > MAX_DORMANT_TIME)
		return;

	// calculate alpha channels.
	int alpha = (int)(255.f * opacity);
	int low_alpha = (int)(179.f * opacity);

	// get color based on enemy or not.
	color = g_menu.main.players.box_enemy.get();

	if (dormant && dormant_esp) {
		alpha = 112;
		low_alpha = 80;

		// fade.
		if (dt > DORMANT_FADE_TIME) {
			// for how long have we been fading?
			float faded = (dt - DORMANT_FADE_TIME);
			float scale = 1.f - (faded / DORMANT_FADE_TIME);

			alpha *= scale;
			low_alpha *= scale;
		}

		// override color.
		color = { 112, 112, 112 };
	}

	// override alpha.
	color.a() = alpha;

	// get player info.
	if (!g_csgo.m_engine->GetPlayerInfo(index, &info))
		return;

	// run offscreen ESP.
	OffScreen(player, alpha);

	// attempt to get player box.
	if (!GetPlayerBoxRect(player, box)) {
		// OffScreen( player );
		return;
	}

	DebugAimbotPoints(player);

	bool bone_esp = (enemy && g_menu.main.players.skeleton.get(0)) || (!enemy && g_menu.main.players.skeleton.get(1));
	if (bone_esp)
		DrawSkeleton(player, low_alpha);

	// debug esp
	if (g_menu.main.players.misc_animlayer.get() && enemy)
	{
		render::esp_small.string(box.x + box.w, box.y + 40 + 30, { 255,255,255,255 }, "m_cycle", render::ALIGN_CENTER);
		render::esp_small.string(box.x + box.w, box.y + 60 + 30, { 255,255,255,255 }, "m_weight", render::ALIGN_CENTER);
		render::esp_small.string(box.x + box.w, box.y + 80 + 30, { 255,255,255,255 }, "m_weight_delta_rate", render::ALIGN_CENTER);
		render::esp_small.string(box.x + box.w, box.y + 100 + 30, { 255,255,255,255 }, "m_prev_cycle", render::ALIGN_CENTER);
		render::esp_small.string(box.x + box.w, box.y + 120 + 30, { 255,255,255,255 }, "m_playback_rate", render::ALIGN_CENTER);
		render::esp_small.string(box.x + box.w, box.y + 140 + 30, { 255,255,255,255 }, "m_sequence", render::ALIGN_CENTER);

		render::esp_small.string(box.x + box.w + 20, box.y + 40 + 30, { 255,255,255,255 }, std::to_string(player->m_AnimOverlay()[6].m_cycle), render::ALIGN_CENTER);
		render::esp_small.string(box.x + box.w + 20, box.y + 60 + 30, { 255,255,255,255 }, std::to_string(player->m_AnimOverlay()[6].m_weight), render::ALIGN_CENTER);
		render::esp_small.string(box.x + box.w + 40, box.y + 80 + 30, { 255,255,255,255 }, std::to_string(player->m_AnimOverlay()[6].m_weight_delta_rate), render::ALIGN_CENTER);
		render::esp_small.string(box.x + box.w + 30, box.y + 100 + 30, { 255,255,255,255 }, std::to_string(player->m_AnimOverlay()[6].m_prev_cycle), render::ALIGN_CENTER);
		render::esp_small.string(box.x + box.w + 30, box.y + 120 + 30, { 255,255,255,255 }, std::to_string(player->m_AnimOverlay()[6].m_playback_rate), render::ALIGN_CENTER);
		render::esp_small.string(box.x + box.w + 20, box.y + 140 + 30, { 255,255,255,255 }, std::to_string(player->m_AnimOverlay()[6].m_sequence), render::ALIGN_CENTER);
	}

	// is box esp enabled for this player.
	bool box_esp = (enemy && g_menu.main.players.box.get());

	// render box if specified.
	if (box_esp)
		if (g_menu.main.players.box_outline.get())
			if (!dormant)
				render::rect_outlined(box.x, box.y, box.w, box.h, color, { 10, 10, 10, 200 });
			else
				render::rect_outlined(box.x, box.y, box.w, box.h, color, { 10, 10, 10, low_alpha });
		else
			render::rect_outlined(box.x, box.y, box.w, box.h, color, { 10, 10, 10, 0 });

	// is name esp enabled for this player.
	bool name_esp = (enemy && g_menu.main.players.name.get());

	// draw name.
	if (name_esp) {
		// fix retards with their namechange meme 
		// the point of this is overflowing unicode compares with hardcoded buffers, good hvh strat
		std::string name{ std::string(info.m_name).substr(0, 24) };

		Color clr = g_menu.main.players.name_color.get();
		if (dormant) {
			clr.r() = 130;
			clr.g() = 130;
			clr.b() = 130;
		}
		// override alpha.
		clr.a() = low_alpha;

		render::esp_name.string(box.x + box.w / 2, box.y - render::esp.m_size.m_height, clr, name + (info.m_fake_player ? XOR(" (BOT)") : ""), render::ALIGN_CENTER);
	}

	// is health esp enabled for this player.
	bool health_esp = (enemy && g_menu.main.players.health.get());

	if (health_esp) {
		int y = box.y + 1;
		int h = box.h - 2;

		// retarded servers that go above 100 hp..
		int hp = std::min(100, player->m_iHealth());

		// calculate hp bar color.
		int r = std::min((510 * (100 - hp)) / 100, 255);
		int g = std::min((510 * hp) / 100, 255);

		// get hp bar height.
		int fill = (int)std::round(hp * h / 100.f);

		// render background.
		render::rect_filled(box.x - 6, y - 2, 4, h + 3 + 1, { 10, 10, 10, low_alpha });

		// render actual bar.
		if (dormant)
			render::rect(box.x - 5, y + h - fill - 1, 2, fill + 2, { 222,222,222 , alpha });
		else
			render::rect(box.x - 5, y + h - fill - 1, 2, fill + 2, { r, g, 0, alpha });

		// if hp is below max, draw a string.
		if (dormant) {
			if (hp < 92 && hp >= 10)
				render::esp_small.string(box.x - 6, y + (h - fill) - 9, { 130, 130, 130, low_alpha }, std::to_string(hp), render::ALIGN_CENTER);
			else if (hp <= 9 && hp >= 1)
				render::esp_small.string(box.x - 5, y + (h - fill) - 9, { 130, 130, 130, low_alpha }, std::to_string(hp), render::ALIGN_CENTER);
		}
		else {
			if (hp < 92 && hp >= 10)
				render::esp_small.string(box.x - 6, y + (h - fill) - 9, { 255, 255, 255, low_alpha }, std::to_string(hp), render::ALIGN_CENTER);
			else if (hp <= 9 && hp >= 1)
				render::esp_small.string(box.x - 5, y + (h - fill) - 9, { 255, 255, 255, low_alpha }, std::to_string(hp), render::ALIGN_CENTER);
		}
	}

	// draw flags.
	{
		float change1337 = std::abs(math::NormalizedAngle(g_cl.m_body - g_cl.m_angle.y));

		Color clr = { 124,195,13,255 };

		int y = box.y + 3;

		AimPlayer* data = &g_aimbot.m_players[player->index() - 1];

		// make sure everything is valid.
		if (data && data->m_moved && data->m_records.size()) {
			// grab lag record.
			LagRecord* current = data->m_records.front().get();

			if (current) {
				if (!(current->m_velocity.length_2d() > 0.1 && !current->m_fake_walk) && data->m_body_index <= 3) {
					float cycle = std::clamp<float>(data->m_body_update - current->m_anim_time, 0.f, 1.125f);
					float width = cycle / 1.125f;

					if (width > 0.f) {
						if (g_menu.main.players.lby_update_circle.get())
							render::draw_arc(box.x - 14, y, 5, 0, 360, 3.3f, { 0,0,0,125 });

						if (dormant) {
							clr.r() = g_menu.main.players.lby_update_circle_color.get().r() / 2;
							clr.g() = g_menu.main.players.lby_update_circle_color.get().g() / 2;
							clr.b() = g_menu.main.players.lby_update_circle_color.get().b() / 2;// 180, 60, 120
							clr.a() = 1;
						}
						else {
							clr.r() = g_menu.main.players.lby_update_circle_color.get().r();
							clr.g() = g_menu.main.players.lby_update_circle_color.get().g();
							clr.b() = g_menu.main.players.lby_update_circle_color.get().b();
							clr.a() = 190;
						}

						clr.a() = alpha;
						if (g_menu.main.players.lby_update_circle.get())
							render::draw_arc(box.x - 14, y, 4, 0, 340 * width, 2, Color(g_menu.main.players.lby_update_circle_color.get().r(), g_menu.main.players.lby_update_circle_color.get().g(), g_menu.main.players.lby_update_circle_color.get().b(), clr.a()));
					}
				}
			}
		}

		std::vector< std::pair< std::string, Color > > flags;

		auto items = g_menu.main.players.flags_enemy.GetActiveIndices();

		// NOTE FROM NITRO TO DEX -> stop removing my iterator loops, i do it so i dont have to check the size of the vector
		// with range loops u do that to do that.
		for (auto it = items.begin(); it != items.end(); ++it) {

			// money.
			if (*it == 0 && enemy) {
				if (dormant)
					flags.push_back({ tfm::format(XOR("$%i"), player->m_iAccount()), { 130,130,130, low_alpha } });
				else
					flags.push_back({ tfm::format(XOR("$%i"), player->m_iAccount()), { 150, 200, 60, low_alpha } });
			}

			// armor.
			if (*it == 1 && enemy) {
				// helmet and kevlar.
				if (player->m_bHasHelmet() && player->m_ArmorValue() > 0)
					if (dormant)
						flags.push_back({ XOR("HK"), { 130,130,130, low_alpha } });
					else
						flags.push_back({ XOR("HK"), { 255, 255, 255, low_alpha } });
				// only helmet.
				else if (player->m_bHasHelmet())
					if (dormant)
						flags.push_back({ XOR("HK"), { 130,130,130, low_alpha } });
					else
						flags.push_back({ XOR("HK"), { 255, 255, 255, low_alpha } });

				// only kevlar.
				else if (player->m_ArmorValue() > 0)
					if (dormant)
						flags.push_back({ XOR("K"), { 130,130,130, low_alpha } });
					else
						flags.push_back({ XOR("K"), { 255, 255, 255, low_alpha } });
			}

			// scoped.
			if (*it == 2 && player->m_bIsScoped() && enemy)
				if (dormant)
					flags.push_back({ XOR("ZOOM"), { 130,130,130, low_alpha } });
				else
					flags.push_back({ XOR("ZOOM"), { 60, 180, 225, low_alpha } });

			// flashed.
			if (*it == 3 && player->m_flFlashBangTime() > 0.f && enemy)
				if (dormant)
					flags.push_back({ XOR("FLASH"), { 130,130,130, low_alpha } });
				else
					flags.push_back({ XOR("FLASH"), { 60, 180, 225, low_alpha } });

			// reload.
			if (*it == 4 && enemy) {
				// get ptr to layer 1.
				C_AnimationLayer* layer1 = &player->m_AnimOverlay()[1];

				// check if reload animation is going on.
				if (layer1->m_weight != 0.f && player->GetSequenceActivity(layer1->m_sequence) == 967 /* ACT_CSGO_RELOAD */)
					if (dormant)
						flags.push_back({ XOR("R"), { 130,130,130, low_alpha } });
					else
						flags.push_back({ XOR("R"), { 60, 180, 225, low_alpha } });
			}

			// bomb.
			if (*it == 5 && player->HasC4() && enemy)
				if (dormant)
					flags.push_back({ XOR("BOMB"), { 130,130,130, low_alpha } });
				else
					flags.push_back({ XOR("BOMB"), { 255, 0, 0, low_alpha } });

			if (*it == 6 && g_resolver.iPlayers[player->index()] == true && enemy)
				if (dormant)
					flags.push_back({ XOR("FAKE"), { 130,130,130, low_alpha } });
				else
					flags.push_back({ XOR("FAKE"), { 255,255,255, low_alpha } });

			if (*it == 7 && player->m_bHasDefuser() && enemy)
				if (dormant)
					flags.push_back({ XOR("KITS"), { 130,130,130, low_alpha } });
				else
					flags.push_back({ XOR("KITS"), { 255,255,255, low_alpha } });

			if (*it == 8 && enemy)
				if (dormant)
					flags.push_back({ tfm::format(XOR("%i"), player->m_iPing(player->index())), {  130, 130, 0, low_alpha } });
				else
					flags.push_back({ tfm::format(XOR("%i"), player->m_iPing(player->index())), { 255, 255, 0, low_alpha } });

		}

		/*¸if (g_aimbot.none)
			flags.push_back({ XOR("NONE"), { 255,255,255, 255 } });
		if (g_aimbot.air)
			flags.push_back({ XOR("AIR"), { 255,255,255, 255 } });
		if (g_aimbot.body)
			flags.push_back({ XOR("BODY"), { 255,255,255, 255 } });
		if (g_aimbot.over)
			flags.push_back({ XOR("OVERRIDE"), { 255,255,255, 255 } });
		if (g_aimbot.stand)
			flags.push_back({ XOR("STAND"), { 255,255,255, 255 } });
		if (g_aimbot.stand1)
			flags.push_back({ XOR("STAND1"), { 255,255,255, 255 } });
		if (g_aimbot.stand2)
			flags.push_back({ XOR("STAND2"), { 255,255,255, 255 } });
		if (g_aimbot.stopmove)
			flags.push_back({ XOR("STOP_MOVE"), { 255,255,255, 255 } });
		if (g_aimbot.walk)
			flags.push_back({ XOR("WALK"), { 255,255,255, 255 } });*/

			// iterate flags.
		for (size_t i{ }; i < flags.size(); ++i) {
			// get flag job (pair).
			const auto& f = flags[i];

			int offset = i * (render::esp_small.m_size.m_height - 0.5f);

			// draw flag.
			render::esp_info.string(box.x + box.w + 2, box.y + offset, f.second, f.first);
		}
	}

	// draw bottom bars.
	{
		int  offset1{ 0 };
		int  offset3{ 0 };
		int  offset{ 0 };
		int  distance1337{ 0 };

		// draw lby update bar.
		if (enemy && g_menu.main.players.lby_update.get()) {
			AimPlayer* data = &g_aimbot.m_players[player->index() - 1];

			// make sure everything is valid.
			if (data && data->m_moved && data->m_records.size()) {
				// grab lag record.
				LagRecord* current = data->m_records.front().get();

				if (current) {
					if (!(current->m_velocity.length_2d() > 0.1 && !current->m_fake_walk) && data->m_body_index <= 3) {
						// calculate box width
						float cycle = std::clamp<float>(data->m_body_update - current->m_anim_time, 0.f, 1.125f);
						float width = (box.w * cycle) / 1.125f;

						if (width > 0.f) {
							// draw.
							render::rect_filled(box.x - 1, box.y + box.h + 2, box.w + 2, 4, { 10, 10, 10, low_alpha });

							Color clr = g_menu.main.players.lby_update_color.get();
							if (dormant) {
								clr.r() = 130;
								clr.g() = 100;
								clr.b() = 120;// 180, 60, 120
							}
							clr.a() = alpha;
							render::rect(box.x, box.y + box.h + 3, width, 2, clr);
							// move down the offset to make room for the next bar.
							offset += 5;
							offset3 += 1;
						}
					}
				}
			}
		}
		// draw weapon.
		if ((enemy && g_menu.main.players.weapon.get())) {
			Weapon* weapon = player->GetActiveWeapon();
			if (weapon) {
				WeaponInfo* data = weapon->GetWpnData();
				if (data) {
					int bar;
					float scale;

					// the maxclip1 in the weaponinfo
					int max = data->m_max_clip1;
					int current = weapon->m_iClip1();

					C_AnimationLayer* layer1 = &player->m_AnimOverlay()[1];

					// set reload state.
					bool reload = (layer1->m_weight != 0.f) && (player->GetSequenceActivity(layer1->m_sequence) == 967);

					// ammo bar.
					if (max != -1 && g_menu.main.players.ammo.get()) {
						// check for reload.
						if (reload)
							scale = layer1->m_cycle;

						// not reloading.
						// make the division of 2 ints produce a float instead of another int.
						else
							scale = (float)current / max;

						// relative to bar.
						bar = (int)std::round((box.w - 2) * scale);

						// draw.
						render::rect_filled(box.x - 1, box.y + box.h + 2 + offset, box.w + 2, 4, { 10, 10, 10, low_alpha });

						Color clr = g_menu.main.players.ammo_color.get();
						if (dormant) {
							clr.r() = 120;
							clr.g() = 125;
							clr.b() = 130;//95, 174, 227,
						}
						clr.a() = alpha;
						render::rect(box.x, box.y + box.h + 3 + offset, bar + 2, 2, clr);

						// less then a 5th of the bullets left.
						if (current <= (int)std::round(max / 5) && !reload)
							if (dormant)
								render::esp_small.string(box.x + bar, box.y + box.h + offset, { 130, 130, 130, low_alpha }, std::to_string(current), render::ALIGN_CENTER);
							else
								render::esp_small.string(box.x + bar, box.y + box.h + offset, { 255, 255, 255, low_alpha }, std::to_string(current), render::ALIGN_CENTER);

						offset += 6;
					}

					if (g_menu.main.players.distance.get()) {
						std::string distance;
						int dist = (((player->m_vecOrigin() - g_cl.m_local->m_vecOrigin()).length_sqr()) * 0.0625) * 0.001;

						if (dist < 0)
							distance1337 = 0;

						if (dist > 0) {
							distance1337 = 9 + offset3;
							if (dist > 5) {
								while (!(dist % 5 == 0)) {
									dist = dist - 1;
								}

								if (dist % 5 == 0)
									distance = tfm::format(XOR("%i FT"), dist);
							}
							else
								distance = tfm::format(XOR("%i FT"), dist);

							if (dormant)
								render::esp_small.string(box.x + box.w / 2, box.y + box.h + offset + offset3, { 130, 130, 130, low_alpha }, distance, render::ALIGN_CENTER);
							else
								render::esp_small.string(box.x + box.w / 2, box.y + box.h + offset + offset3, { 255, 255, 255, low_alpha }, distance, render::ALIGN_CENTER);
						}
					}

					// text.
					if (g_menu.main.players.weapon_mode.get(0)) {
						offset1 -= 9;
						// construct std::string instance of localized weapon name.
						std::string name{ weapon->GetLocalizedName() };

						// smallfonts needs upper case.
						std::transform(name.begin(), name.end(), name.begin(), ::toupper);


						if (dormant)
							render::esp_small.string(box.x + box.w / 2, box.y + box.h + offset + distance1337, { 130,130,130, low_alpha }, name, render::ALIGN_CENTER);
						else
							render::esp_small.string(box.x + box.w / 2, box.y + box.h + offset + distance1337, { 255, 255, 255, low_alpha }, name, render::ALIGN_CENTER);

					}

					// icons.
					if (g_menu.main.players.weapon_mode.get(1)) {
						offset -= 0;
						// icons are super fat..
						// move them back up.

						if (!g_csgo.m_engine->IsInGame() && !g_csgo.m_engine->IsConnected())
							return;

						const auto icon = weapon->getIcon();

						Color clr = g_menu.main.players.weapon_icon_color.get();

						if (dormant)
							render::DrawString(F::Icons, box.x + box.w / 2, box.y + box.h + offset - offset1 + distance1337 + 2, Color(130, 130, 130, low_alpha), FONT_CENTER, icon);
						else
							render::DrawString(F::Icons, box.x + box.w / 2, box.y + box.h + offset - offset1 + distance1337 + 2, clr, FONT_CENTER, icon);
					}
					/*std::string name{weapon->GetLocalizedName()};

std::transform(name.begin(), name.end(), name.begin(), ::toupper);


if (dormant)
	render::DrawString(F::Skeet, box.x + box.w / 2, box.y + box.h + offset - offset1 + distance1337 + 2, Color(130, 130, 130, low_alpha), FONT_CENTER, name.c_str());
else
	render::DrawString(F::Skeet, box.x + box.w / 2, box.y + box.h + offset - offset1 + distance1337 + 2, clr, FONT_CENTER, name.c_str());
}*/
				}
			}
		}
	}
}

void Visuals::DrawPlantedC4() {
	bool        mode_2d, mode_3d, is_visible, fatal;
	float       explode_time_diff, dist, range_damage;
	vec3_t      dst, to_target;
	int         final_damage;
	std::string time_str, damage_str;
	Color       damage_color;
	vec2_t      screen_pos;

	int w = g_cl.m_width,
		h = g_cl.m_height,
		x = w / 2,
		y = h / 2;

	std::string iconC4 = tfm::format(XOR("%c"), m_weapon_icons[C4]);

	static auto scale_damage = [](float damage, int armor_value) {
		float new_damage, armor;

		if (armor_value > 0) {
			new_damage = damage * 0.5f;
			armor = (damage - new_damage) * 0.5f;

			if (armor > (float)armor_value) {
				armor = (float)armor_value * 2.f;
				new_damage = damage - armor;
			}

			damage = new_damage;
		}

		return std::max(0, (int)std::floor(damage));
	};

	// store menu vars for later.
	mode_2d = g_menu.main.world.planted_c4.get(0);
	mode_3d = g_menu.main.world.planted_c4.get(1);
	if (!mode_2d && !mode_3d)
		return;

	// bomb not currently active, do nothing.
	if (!m_c4_planted)
		return;

	// calculate bomb damage.
	// references:
	//     https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/shared/cstrike/weapon_c4.cpp#L271
	//     https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/shared/cstrike/weapon_c4.cpp#L437
	//     https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/sdk/sdk_gamerules.cpp#L173
	{
		// get our distance to the bomb.
		// todo - dex; is dst right? might need to reverse CBasePlayer::BodyTarget...
		dst = g_cl.m_local->WorldSpaceCenter();
		to_target = m_planted_c4_explosion_origin - dst;
		dist = to_target.length();

		// calculate the bomb damage based on our distance to the C4's explosion.
		range_damage = m_planted_c4_damage * std::exp((dist * dist) / ((m_planted_c4_radius_scaled * -2.f) * m_planted_c4_radius_scaled));

		// now finally, scale the damage based on our armor (if we have any).
		final_damage = scale_damage(range_damage, g_cl.m_local->m_ArmorValue());
	}

	// m_flC4Blow is set to gpGlobals->curtime + m_flTimerLength inside CPlantedC4.
	explode_time_diff = m_planted_c4_explode_time - g_csgo.m_globals->m_curtime;

	// get formatted strings for bomb.
	time_str = tfm::format(XOR("BOMB: %.1f (-%ihp)"), explode_time_diff, final_damage);
	damage_str = tfm::format(XOR("%i"), final_damage);

	// get damage color.
	damage_color = (final_damage < g_cl.m_local->m_iHealth()) ? colors::white : colors::red;

	// finally do all of our rendering.
	is_visible = render::WorldToScreen(m_planted_c4_explosion_origin, screen_pos);

	std::string bomb = m_last_bombsite.c_str();

	fatal = final_damage >= g_cl.m_local->m_iHealth();

	// 'on screen (2D)'.
	if (mode_2d) {
		std::string timer1337 = tfm::format(XOR("-%.1s"), explode_time_diff);
		std::string damage1337 = fatal ? tfm::format(XOR("FATAL")) : tfm::format(XOR("-%i"), final_damage);
		std::string c4_timer = tfm::format(XOR("%i"), iconC4);
		std::string def_timer = tfm::format(XOR(" - %.1f"), explode_time_diff);

		Color colortimer = { 135, 172, 10, 255 };
		if (explode_time_diff < 10) colortimer = { 200, 200, 110, 255 };
		if (explode_time_diff < 5) colortimer = { 192, 32, 17, 255 };

		if (m_c4_planted && !bombexploded && !bombedefused) {
			if (explode_time_diff > 0.f) {
				render::cs_c4.string(x - 48, y - 205, { 255,255,255,255 }, c4_timer);
				render::esp.string(x - 20, y - 200, { 255,255,255,255 }, def_timer);
			}
			if (g_cl.m_local->m_iHealth() <= final_damage) {
				render::indicator.string(6, 11, { 0,0, 0, 125 }, tfm::format(XOR("FATAL")));
				render::indicator.string(5, 10, { 192, 32, 17, 255 }, tfm::format(XOR("FATAL")));
			}
			else if (final_damage > 1) {
				render::indicator.string(5, 11, { 0,0, 0, 125 }, tfm::format(XOR("- %iHP"), final_damage));
				render::indicator.string(6, 10, { 255, 255, 152, 255 }, tfm::format(XOR("- %iHP"), final_damage));
			}
		}
	}

	// 'on bomb (3D)'.
	if (mode_3d && is_visible) {
		if (explode_time_diff > 0.f)
			render::esp.string(screen_pos.x, screen_pos.y, colors::white, time_str, render::ALIGN_CENTER);
	}
}

bool Visuals::GetPlayerBoxRect(Player* player, Rect& box) {
	vec3_t origin, mins, maxs;
	vec2_t bottom, top;

	// get interpolated origin.
	origin = player->GetAbsOrigin();

	// get hitbox bounds.
	player->ComputeHitboxSurroundingBox(&mins, &maxs);

	// correct x and y coordinates.
	mins = { origin.x, origin.y, mins.z };
	maxs = { origin.x, origin.y, maxs.z + 8.f };

	if (!render::WorldToScreen(mins, bottom) || !render::WorldToScreen(maxs, top))
		return false;

	box.h = bottom.y - top.y;
	box.w = box.h / 2.f;
	box.x = bottom.x - (box.w / 2.f);
	box.y = bottom.y - box.h;

	return true;
}

void Visuals::DrawHistorySkeleton(Player* player, int opacity) {
	const model_t* model;
	studiohdr_t* hdr;
	mstudiobone_t* bone;
	AimPlayer* data;
	LagRecord* record;
	int           parent;
	vec3_t        bone_pos, parent_pos;
	vec2_t        bone_pos_screen, parent_pos_screen;

	if (!g_menu.main.misc.fake_latency.get())
		return;

	// get player's model.
	model = player->GetModel();
	if (!model)
		return;

	// get studio model.
	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	data = &g_aimbot.m_players[player->index() - 1];
	if (!data)
		return;

	record = g_resolver.FindLastRecord(data);
	if (!record)
		return;

	for (int i{ }; i < hdr->m_num_bones; ++i) {
		// get bone.
		bone = hdr->GetBone(i);
		if (!bone || !(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		// get parent bone.
		parent = bone->m_parent;
		if (parent == -1)
			continue;

		// resolve main bone and parent bone positions.
		record->m_bones->get_bone(bone_pos, i);
		record->m_bones->get_bone(parent_pos, parent);

		Color clr = player->enemy(g_cl.m_local) ? g_menu.main.players.skeleton_enemy.get() : g_menu.main.players.skeleton_friendly.get();
		clr.a() = opacity;

		// world to screen both the bone parent bone then draw.
		if (render::WorldToScreen(bone_pos, bone_pos_screen) && render::WorldToScreen(parent_pos, parent_pos_screen))
			render::line(bone_pos_screen.x, bone_pos_screen.y, parent_pos_screen.x, parent_pos_screen.y, clr);
	}
}

void Visuals::DrawSkeleton(Player* player, int opacity) {
	const model_t* model;
	studiohdr_t* hdr;
	mstudiobone_t* bone;
	int           parent;
	BoneArray     matrix[128];
	vec3_t        bone_pos, parent_pos;
	vec2_t        bone_pos_screen, parent_pos_screen;

	// get player's model.
	model = player->GetModel();
	if (!model)
		return;

	// get studio model.
	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	// get bone matrix.
	if (!player->SetupBones(matrix, 128, BONE_USED_BY_ANYTHING, g_csgo.m_globals->m_curtime))
		return;

	if (player->dormant())
		return;

	for (int i{ }; i < hdr->m_num_bones; ++i) {
		// get bone.
		bone = hdr->GetBone(i);
		if (!bone || !(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		// get parent bone.
		parent = bone->m_parent;
		if (parent == -1)
			continue;

		// resolve main bone and parent bone positions.
		matrix->get_bone(bone_pos, i);
		matrix->get_bone(parent_pos, parent);

		Color clr = player->enemy(g_cl.m_local) ? g_menu.main.players.skeleton_enemy.get() : g_menu.main.players.skeleton_friendly.get();
		clr.a() = opacity;

		// world to screen both the bone parent bone then draw.
		if (render::WorldToScreen(bone_pos, bone_pos_screen) && render::WorldToScreen(parent_pos, parent_pos_screen))
			render::line(bone_pos_screen.x, bone_pos_screen.y, parent_pos_screen.x, parent_pos_screen.y, clr);
	}
}

void Visuals::RenderGlow() {
	Color   color;
	Player* player;

	if (!g_cl.m_local)
		return;

	if (!g_csgo.m_glow->m_object_definitions.Count())
		return;

	float blend = g_menu.main.players.glow_blend.get() / 100.f;

	for (int i{ }; i < g_csgo.m_glow->m_object_definitions.Count(); ++i) {
		GlowObjectDefinition_t* obj = &g_csgo.m_glow->m_object_definitions[i];

		// skip non-players.
		if (!obj->m_entity || !obj->m_entity->IsPlayer())
			continue;

		// get player ptr.
		player = obj->m_entity->as< Player* >();

		if (player->m_bIsLocalPlayer())
			continue;

		// get reference to array variable.
		float& opacity = m_opacities[player->index() - 1];

		bool enemy = player->enemy(g_cl.m_local);

		if (enemy && !g_menu.main.players.glow1.get())
			continue;

		if (!enemy && !g_menu.main.players.glow.get(1))
			continue;

		// enemy color
		if (enemy)
			color = g_menu.main.players.glow_enemy.get();

		// friendly color
		else
			color = g_menu.main.players.glow_friendly.get();

		obj->m_render_occluded = true;
		obj->m_render_unoccluded = false;
		obj->m_render_full_bloom = false;
		obj->m_color = { (float)color.r() / 255.f, (float)color.g() / 255.f, (float)color.b() / 255.f };
		obj->m_alpha = opacity * blend;
	}
}

void Visuals::DrawHitboxMatrix(LagRecord* record, Color col, float time) {
	if (!g_menu.main.aimbot.debugaim.get())
		return;
	const model_t* model;
	studiohdr_t* hdr;
	mstudiohitboxset_t* set;
	mstudiobbox_t* bbox;
	vec3_t             mins, maxs, origin;
	ang_t			   angle;

	model = record->m_player->GetModel();
	if (!model)
		return;

	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	set = hdr->GetHitboxSet(record->m_player->m_nHitboxSet());
	if (!set)
		return;

	for (int i{ }; i < set->m_hitboxes; ++i) {
		bbox = set->GetHitbox(i);
		if (!bbox)
			continue;

		// bbox.
		if (bbox->m_radius <= 0.f) {
			// https://developer.valvesoftware.com/wiki/Rotation_Tutorial

			// convert rotation angle to a matrix.
			matrix3x4_t rot_matrix;
			g_csgo.AngleMatrix(bbox->m_angle, rot_matrix);

			// apply the rotation to the entity input space (local).
			matrix3x4_t matrix;
			math::ConcatTransforms(record->m_bones[bbox->m_bone], rot_matrix, matrix);

			// extract the compound rotation as an angle.
			ang_t bbox_angle;
			math::MatrixAngles(matrix, bbox_angle);

			// extract hitbox origin.
			vec3_t origin = matrix.GetOrigin();

			// draw box.
			g_csgo.m_debug_overlay->AddBoxOverlay(origin, bbox->m_mins, bbox->m_maxs, bbox_angle, col.r(), col.g(), col.b(), col.a(), time); // col.a() rendering arms/legs through walls, 0 only visible
		}

		// capsule.
		else {
			// NOTE; the angle for capsules is always 0.f, 0.f, 0.f.

			// create a rotation matrix.
			matrix3x4_t matrix;
			g_csgo.AngleMatrix(bbox->m_angle, matrix);

			// apply the rotation matrix to the entity output space (world).
			math::ConcatTransforms(record->m_bones[bbox->m_bone], matrix, matrix);

			// get world positions from new matrix.
			math::VectorTransform(bbox->m_mins, matrix, mins);
			math::VectorTransform(bbox->m_maxs, matrix, maxs);

			/*Color col;
			if (bbox->m_group == HITGROUP_CHEST)
				col = Color(255,255,255,255);
			else
				col = Color(255, 0, 0, 0);*/

			g_csgo.m_debug_overlay->AddCapsuleOverlay(mins, maxs, bbox->m_radius, col.r(), col.g(), col.b(), col.a(), time, 0, 1); // 1 == through wall, 0 == visible
		}
	}
}

void Visuals::DrawBeams() {
	size_t     impact_count;
	float      curtime, dist;
	bool       is_final_impact;
	vec3_t     va_fwd, start, dir, end;
	BeamInfo_t beam_info;
	Beam_t* beam;

	if (!g_cl.m_local)
		return;


	if (!g_menu.main.visuals.impact_beams.get())
		return;

	auto vis_impacts = &g_shots.m_vis_impacts;

	// the local player is dead, clear impacts.
	if (!g_cl.m_processing) {
		if (!vis_impacts->empty())
			vis_impacts->clear();
	}

	else {
		impact_count = vis_impacts->size();
		if (!impact_count)
			return;

		curtime = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());

		for (size_t i{ impact_count }; i-- > 0; ) {
			auto impact = &vis_impacts->operator[ ](i);
			if (!impact)
				continue;

			// impact is too old, erase it.
			if (std::abs(curtime - game::TICKS_TO_TIME(impact->m_tickbase)) > g_menu.main.visuals.impact_beams_time.get()) {
				vis_impacts->erase(vis_impacts->begin() + i);

				continue;
			}

			// already rendering this impact, skip over it.
			if (impact->m_ignore)
				continue;

			// is this the final impact?
			// last impact in the vector, it's the final impact.
			if (i == (impact_count - 1))
				is_final_impact = true;

			// the current impact's tickbase is different than the next, it's the final impact.
			else if ((i + 1) < impact_count && impact->m_tickbase != vis_impacts->operator[ ](i + 1).m_tickbase)
				is_final_impact = true;

			else
				is_final_impact = false;

			// is this the final impact?
			// is_final_impact = ( ( i == ( impact_count - 1 ) ) || ( impact->m_tickbase != vis_impacts->at( i + 1 ).m_tickbase ) );

			// for good pos of start beam
			static auto last_r = vec3_t(0.0f, 0.0f, -2.0f);

			if (is_final_impact) {
				// calculate start and end position for beam.
				start = impact->m_shoot_pos;

				dir = (impact->m_impact_pos - start).normalized();
				dist = (impact->m_impact_pos - start).length();

				end = start + (dir * dist);

				// setup beam info.
				// note - dex; possible beam models: sprites/physbeam.vmt | sprites/white.vmt 
				
					// note - dex; possible beam models: sprites/physbeam.vmt | sprites/white.vmt 
					beam_info.m_vecStart = start + last_r;
					beam_info.m_vecEnd = end;
					beam_info.m_nModelIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/purplelaser1.vmt"));
					beam_info.m_pszModelName = XOR("sprites/purplelaser1.vmt");
					beam_info.m_flHaloScale = 0.f;
					beam_info.m_flLife = 4.0f;
					beam_info.m_flWidth = g_menu.main.visuals.impact_beams_time.get() / 2; // scale
					beam_info.m_flEndWidth = g_menu.main.visuals.impact_beams_time.get() / 2; // scale
					beam_info.m_flFadeLength = 0.f;
					beam_info.m_flAmplitude = 1.0f;   // beam 'jitter'.
					beam_info.m_flBrightness = 230.0f;
					beam_info.m_flSpeed = 0.2f;  // seems to control how fast the 'scrolling' of beam is... once fully spawned.
					beam_info.m_nStartFrame = 0;
					beam_info.m_flFrameRate = 0.f;
					beam_info.m_nSegments = 2;     // controls how much of the beam is 'split up', usually makes m_flAmplitude and m_flSpeed much more noticeable.
					beam_info.m_bRenderable = true;  // must be true or you won't see the beam.
					beam_info.m_nFlags = 0;
					beam_info.m_flRed = g_menu.main.visuals.impact_beams_color.get().r();
					beam_info.m_flGreen = g_menu.main.visuals.impact_beams_color.get().g();
					beam_info.m_flBlue = g_menu.main.visuals.impact_beams_color.get().b();

				/*if (!impact->m_hit_player) {
					beam_info.m_flRed = g_menu.main.visuals.impact_beams_color.get().r();
					beam_info.m_flGreen = g_menu.main.visuals.impact_beams_color.get().g();
					beam_info.m_flBlue = g_menu.main.visuals.impact_beams_color.get().b();
				}

				else {
					beam_info.m_flRed = g_menu.main.visuals.impact_beams_hurt_color.get().r();
					beam_info.m_flGreen = g_menu.main.visuals.impact_beams_hurt_color.get().g();
					beam_info.m_flBlue = g_menu.main.visuals.impact_beams_hurt_color.get().b();
				}*/

				// attempt to render the beam.
				beam = game::CreateGenericBeam(beam_info);
				if (beam) {
					g_csgo.m_beams->DrawBeam(beam);

					// we only want to render a beam for this impact once.
					impact->m_ignore = true;
				}
			}
		}
	}
}

void Visuals::DebugAimbotPoints(Player* player) {
	if (g_menu.main.players.debug_multpoints.get()) {


		std::vector< vec3_t > p2{ };

		AimPlayer* data = &g_aimbot.m_players.at(player->index() - 1);
		if (!data || data->m_records.empty())
			return;

		LagRecord* front = data->m_records.front().get();
		if (!front || front->dormant())
			return;

		// get bone matrix.
		BoneArray matrix[128];
		if (!g_bones.setup(player, matrix, front))
			return;

		data->SetupHitboxes(front, false);
		if (data->m_hitboxes.empty())
			return;

		for (const auto& it : data->m_hitboxes) {
			std::vector< vec3_t > p1{ };

			if (!data->SetupHitboxPoints(front, matrix, it.m_index, p1))
				continue;

			for (auto& p : p1)
				p2.push_back(p);
		}

		if (p2.empty())
			return;

		for (auto& p : p2) {
			vec2_t screen;

			if (render::WorldToScreen(p, screen))
				render::rect_filled(screen.x, screen.y, 2, 2, { 255, 0, 0, 200 });
		}
	}
}

void Visuals::update_positions() {
	CUtlVector< CSndInfo > sound_info{};

	g_csgo.m_engine_sound->GetActiveSounds(sound_info);

	for (size_t i{}; i < sound_info.GetSize(); ++i) {
		auto& snd = sound_info.GetElements()[i];

		if (snd.origin) {
			int  idx = snd.sound_source;
			auto ent = g_csgo.m_entlist->GetClientEntity(idx);

			static float last_time[65]{};

			if (std::abs(g_csgo.m_globals->m_curtime - last_time[idx]) > 1.f) {
				vec3_t pos = ent->m_vecOrigin();
				pos.z += 10.f;
				store_sound(idx, pos);
				last_time[idx] = g_csgo.m_globals->m_curtime;
			}
		}
	}
}

void Visuals::store_sound(int index, vec3_t origin) {
	if (!g_menu.main.visuals.grenade_predict.get())
		return;

	auto ent = g_csgo.m_entlist->GetClientEntity<>(index);
	if (ent && ent->IsPlayer() && ent != g_cl.m_local) {
		sound_t new_sound;
		new_sound.m_time = g_csgo.m_globals->m_curtime;
		new_sound.m_ent = index;
		new_sound.m_pos = origin;
		new_sound.m_pos.z += 5.f;
		m_sounds.emplace_back(new_sound);
	}
}

void Visuals::Keybinds() {

	if (!g_csgo.m_engine->IsInGame() || !g_cl.m_local->alive())
		return;

	if (!g_menu.main.visuals.keybinds.get())
		return;

	Color color = g_gui.m_color;
	struct Indicator_t { std::string text; std::string mode; };
	std::vector< Indicator_t > indicators{ };

	// what we are btw trying to draw...
	float dsync_height = g_cl.m_local->m_flPoseParameter()[11] * 58;	// this?
	float choke_value = g_csgo.m_cl->m_choked_commands;

	// dsync

	int	x{ 8 };


	render::esp.string(x + 10, g_cl.m_height / 1.6 - 5, color, XOR("fake:"), render::ALIGN_CENTER);
	// outline
	render::rect_filled(x + 35, g_cl.m_height / 1.6, 97, 6, { 10, 10, 10, 160 });
	// line.
	render::rect_filled(x + 35, g_cl.m_height / 1.6, dsync_height * 1.67, 6, { color.r(), color.g(), color.b(), 255 });	// 97/58

	render::esp.string(x + 10, g_cl.m_height / 1.6 + 10, color, XOR("flag:"), render::ALIGN_CENTER);
	// outline
	render::rect_filled(x + 35, g_cl.m_height / 1.6 + 15, 97, 6, { 10, 10, 10, 160 });
	// line.
	render::rect_filled(x + 35, g_cl.m_height / 1.6 + 15, choke_value * (97 / 14.f), 6, { color.r(), color.g(), color.b(), 255 });


	if (g_aimbot.m_double_tap) {
		Indicator_t ind{};
		ind.text = XOR("double tap");
		ind.mode = XOR("[on]");
		indicators.push_back(ind);
	}

	if (g_input.GetKeyState(g_menu.main.aimbot.override.get())) {
		Indicator_t ind{};
		ind.text = XOR("resolver override");
		ind.mode = XOR("[on]");
		indicators.push_back(ind);
	}

	if (g_aimbot.m_damage_toggle) {
		Indicator_t ind{};
		ind.text = XOR("damage override");
		ind.mode = XOR("[on]");
		indicators.push_back(ind);
	}

	if (g_aimbot.m_fake_latency) {
		Indicator_t ind{};
		ind.text = XOR("fake latency");
		ind.mode = XOR("[on]");
		indicators.push_back(ind);
	}

	if (g_hvh.m_left || g_hvh.m_right || g_hvh.m_back) {
		Indicator_t ind{};
		ind.text = g_hvh.m_left ? XOR("manual left") : g_hvh.m_right ? XOR("manual right") : XOR("manual back");
		ind.mode = XOR("[on]");
		indicators.push_back(ind);
	}

	if (g_input.GetKeyState(g_menu.main.aimbot.baim_key.get())) {
		Indicator_t ind{};
		ind.text = XOR("force body aim");
		ind.mode = XOR("[on]");
		indicators.push_back(ind);
	}

	if (g_input.GetKeyState(g_menu.main.movement.autopeek.get())) {
		Indicator_t ind{};
		ind.text = XOR("auto peek");
		ind.mode = XOR("[on]");
		indicators.push_back(ind);
	}

	if (g_input.GetKeyState(g_menu.main.movement.fakewalk.get())) {
		Indicator_t ind{};
		ind.text = XOR("fake walk");
		ind.mode = XOR("[on]");
		indicators.push_back(ind);
	}

	if (!indicators.empty() || g_gui.m_open) {
		render::rect_filled(10, g_cl.m_height / 2 + 10, 150, 17, { 24, 24, 24, 255 });
		render::rect_filled(11, g_cl.m_height / 2 + 11, 148, 2, color);

		render::esp.string(85, g_cl.m_height / 2 + 12, colors::white, XOR("keybinds"), render::ALIGN_CENTER);
	}

	if (indicators.empty())
		return;

	for (size_t i{ }; i < indicators.size(); ++i) {
		auto& indicator = indicators[i];
		auto size = render::indicator.size(indicator.text);

		render::esp.string(17, (g_cl.m_height / 2 + 30) + (i * 15), colors::white, indicator.text, render::ALIGN_LEFT);
		render::esp.string(141, (g_cl.m_height / 2 + 30) + (i * 15), color, indicator.mode, render::ALIGN_CENTER);
	}
}

void Visuals::draw_sound() {
	if (!g_menu.main.visuals.grenade_predict.get())
		return;

	float time = g_csgo.m_globals->m_curtime;

	if (m_sounds.empty())
		return;

	for (size_t i{}; i < m_sounds.size() && !m_sounds.empty(); ++i) {
		auto& tr = m_sounds[i];

		float delta = g_csgo.m_globals->m_curtime - tr.m_time;
		if (delta > 0.02f || std::abs(delta) > 0.02f)
			m_sounds.erase(m_sounds.begin() + i);
	}

	if (!m_sounds.empty()) {
		for (auto& it : m_sounds) {
			auto ent = g_csgo.m_entlist->GetClientEntity(it.m_ent);
			if (!ent || !ent->IsPlayer())
				continue;

			BeamInfo_t beam_info;

			beam_info.m_nType = beam_ring_point;
			beam_info.m_nModelIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/purplelaser1.vmt"));
			beam_info.m_pszModelName = XOR("sprites/purplelaser1.vmt");
			beam_info.m_flHaloScale = 0.0f;
			beam_info.m_flLife = 0.75f;  // 0.09
			beam_info.m_flWidth = 2.f;
			beam_info.m_flEndWidth = 2.f;
			beam_info.m_flFadeLength = 10.0f;
			beam_info.m_flAmplitude = 0.f;
			beam_info.m_flBrightness = 255.f;
			beam_info.m_flSpeed = 2.f;
			beam_info.m_nStartFrame = 0;
			beam_info.m_flFrameRate = 60;
			beam_info.m_flRed = g_menu.main.visuals.impact_beams_color.get().r();
			beam_info.m_flGreen = g_menu.main.visuals.impact_beams_color.get().g();
			beam_info.m_flBlue = g_menu.main.visuals.impact_beams_color.get().b();
			beam_info.m_nSegments = 1;
			beam_info.m_bRenderable = true;
			beam_info.m_nFlags = 0;

			beam_info.m_vecCenter = it.m_pos;
			beam_info.m_flStartRadius = 0.f;
			beam_info.m_flEndRadius = 450.f;

			Beam_t* beam = g_csgo.m_beams->CreateBeamRingPoint(beam_info);

			if (beam) {
				g_csgo.m_beams->DrawBeam(beam);
			}
		}
	}

	for (size_t i{}; i < m_sounds.size() && !m_sounds.empty(); ++i)
		m_sounds.erase(m_sounds.begin() + i);
}

/*void Visuals::Zoom(CViewSetup* view) { // onetap zoom

	float config_value = (g_cfg[XOR("misc_zoom_fov")].get< float >() / 100.f);

	float v45;
	static float dword_43FFAE60;

	if (!g_visuals.m_zoom)
	{
		v45 = -1.0;
	}
	else
	{
		v45 = 1.0;
	}
	float v46 = ((g_csgo.m_globals->m_frametime) * v45) + dword_43FFAE60;
	float v47 = fmaxf(fminf(0.23, v46), 0.0);
	dword_43FFAE60 = v47;
	float v48 = v47 / 0.23;
	if (v48 > 0.0)
	{
		float v49 = (view->m_fov);
		float v50 = (1.0 - fmaxf(fminf(1.0, config_value * 0.89999998), 0.0)) * v49;
		float v51 = ((v49 - v50) * 0.1) + v50;
		v46 = ((v51 - v49) * v48) + v49;
		(view->m_fov) = (((((v50 - v51) * v48) + v51) - v46) * v48) + v46;
	}
}*/