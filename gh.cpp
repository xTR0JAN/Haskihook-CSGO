#include "includes.h"
#include "gh.h"

void IEngineTrace::TraceLine(const vec3_t& src, const vec3_t& dst, int mask, IHandleEntity* entity, int collision_group, CGameTrace* trace) {
	static auto trace_filter_simple = pattern::find(g_csgo.m_client_dll, XOR("55 8B EC 83 E4 F0 83 EC 7C 56 52")) + 0x3D;

	std::uintptr_t filter[4] = { *reinterpret_cast<std::uintptr_t*>(trace_filter_simple), reinterpret_cast<std::uintptr_t>(entity), collision_group, 0 };

	TraceRay(Ray(src, dst), mask, reinterpret_cast<CTraceFilter*>(&filter), trace);
}

void IEngineTrace::TraceHull(const vec3_t& src, const vec3_t& dst, const vec3_t& mins, const vec3_t& maxs, int mask, IHandleEntity* entity, int collision_group, CGameTrace* trace) {
	static auto trace_filter_simple = pattern::find(g_csgo.m_client_dll, XOR("55 8B EC 83 E4 F0 83 EC 7C 56 52")) + 0x3D;

	std::uintptr_t filter[4] = { *reinterpret_cast<std::uintptr_t*>(trace_filter_simple), reinterpret_cast<std::uintptr_t>(entity), collision_group, 0 };

	TraceRay(Ray(src, dst, mins, maxs), mask, reinterpret_cast<CTraceFilter*>(&filter), trace);
}

float& Player::get_creation_time() {
	return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + 0x29B0);
}

void c_grenade_prediction::on_create_move(CUserCmd* cmd) {
	m_data = {};

	if (!g_cl.m_processing || !g_menu.main.visuals.tracers.get())
		return;

	const auto weapon = reinterpret_cast<Weapon*>(g_csgo.m_entlist->GetClientEntityFromHandle(g_cl.m_local->GetActiveWeapon()));
	if (!weapon || !weapon->m_bPinPulled() && weapon->m_fThrowTime() == 0.f)
		return;

	const auto weapon_data = weapon->GetWpnData();
	if (!weapon_data || weapon_data->m_weapon_type != 9)
		return;

	m_data.m_owner = g_cl.m_local;
	m_data.m_index = weapon->m_iItemDefinitionIndex();

	auto view_angles = cmd->m_view_angles;

	if (view_angles.x < -90.f) {
		view_angles.x += 360.f;
	}
	else if (view_angles.x > 90.f) {
		view_angles.x -= 360.f;
	}

	view_angles.x -= (90.f - std::fabsf(view_angles.x)) * 10.f / 90.f;

	auto direction = vec3_t();

	math::AngleVectors(view_angles, direction);

	const auto throw_strength = std::clamp< float >(weapon->m_flThrowStrength(), 0.f, 1.f);
	const auto eye_pos = g_cl.m_shoot_pos;
	const auto src = vec3_t(eye_pos.x, eye_pos.y, eye_pos.z + (throw_strength * 12.f - 12.f));

	auto trace = CGameTrace();

	g_csgo.m_engine_trace->TraceHull(src, src + direction * 22.f, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f }, MASK_SOLID | CONTENTS_CURRENT_90, g_cl.m_local, COLLISION_GROUP_NONE, &trace);

	m_data.predict(trace.m_endpos - direction * 6.f, direction * (std::clamp< float >(weapon_data->m_throw_velocity * 0.9f, 15.f, 750.f) * (throw_strength * 0.7f + 0.3f)) + g_cl.m_local->m_vecVelocity() * 1.25f, g_csgo.m_globals->m_curtime, 0);
}

//const char* index_to_grenade_name(int index)
//{
//	switch (index)
//	{
//	case WEAPON_SMOKEGRENADE: return "smoke"; break;
//	case WEAPON_HEGRENADE: return "he grenade"; break;
//	case WEAPON_MOLOTOV:return "molotov"; break;
//	case WEAPON_INCGRENADE:return "molotov"; break;
//	}
//}
//const char* index_to_grenade_name_icon(int index)
//{
//	switch (index)
//	{
//	case WEAPON_SMOKEGRENADE: return "k"; break;
//	case WEAPON_HEGRENADE: return "j"; break;
//	case WEAPON_MOLOTOV:return "l"; break;
//	case WEAPON_INCGRENADE:return "n"; break;
//	}
//}

//void draw_arc(int x, int y, int radius, int start_angle, int percent, int thickness, Color color) {
//	auto precision = (2 * M_PI) / 30;
//	auto step = M_PI / 180;
//	auto inner = radius - thickness;
//	auto end_angle = (start_angle + percent) * step;
//	auto start_angles = (start_angle * M_PI) / 180;
//
//	for (; radius > inner; --radius) {
//		for (auto angle = start_angles; angle < end_angle; angle += precision) {
//			auto cx = std::round(x + radius * std::cos(angle));
//			auto cy = std::round(y + radius * std::sin(angle));
//
//			auto cx2 = std::round(x + radius * std::cos(angle + precision));
//			auto cy2 = std::round(y + radius * std::sin(angle + precision));
//
//			Drawing::DrawLine(cx, cy, cx2, cy2, color);
//		}
//	}
//}

bool c_grenade_prediction::data_t::draw() const
{
	if (m_path.size() <= 1u || g_csgo.m_globals->m_curtime >= m_expire_time)
		return false;

	float distance = g_cl.m_local->m_vecOrigin().dist_to(m_origin) / 12;

	if (distance > 200.f)
		return false;

	auto prev_screen = vec2_t();
	auto prev_on_screen = render::WorldToScreen(std::get< vec3_t >(m_path.front()), prev_screen);

	for (auto i = 1u; i < m_path.size(); ++i) {
		auto cur_screen = vec2_t();
		const auto cur_on_screen = render::WorldToScreen(std::get< vec3_t >(m_path.at(i)), cur_screen);

		if (prev_on_screen && cur_on_screen) {
			render::line(prev_screen.x, prev_screen.y, cur_screen.x, cur_screen.y, Color(255, 255, 255, 255));

		}

		prev_screen = cur_screen;
		prev_on_screen = cur_on_screen;
	}

	//float percent = ((m_expire_time - g_csgo.m_globals->m_curtime) / game::TICKS_TO_TIME(m_tick));
	//int alpha_damage = 0;

	//if (m_index == HEGRENADE && distance <= 20) {
		//alpha_damage = 255 - 255 * (distance / 20);
	//}

	//if ((m_index == MOLOTOV || m_index == FIREBOMB) && distance <= 15) {
//	alpha_damage = 255 - 255 * (distance / 15);
	//}

	/*Drawing::DrawRect(prev_screen.x - textsizeicon.right / 2 - 3, prev_screen.y - textsizeicon.bottom / 2 - 3, textsizeicon.right + textsize.right + 6 + 3, textsizeicon.bottom + 6, Color(10, 10, 14, 150));
	Drawing::DrawRect(prev_screen.x - textsizeicon.right / 2 - 2, prev_screen.y - textsizeicon.bottom / 2 - 2, textsizeicon.right + textsize.right + 4 + 3, textsizeicon.bottom + 4, Color(36, 36, 40, 150));

	Drawing::DrawRect(prev_screen.x - textsizeicon.right / 2 - 2, prev_screen.y - textsizeicon.bottom / 2 - 2, textsizeicon.right + textsize.right + 4 + 3, textsizeicon.bottom + 4, Color(255, 0, 0, alpha_damage));
	Drawing::DrawRect(prev_screen.x - textsizeicon.right / 2 - 3, prev_screen.y - textsizeicon.bottom / 2 - 3, percent, 3, Color(100, 100, 255, 150));

	Drawing::DrawString(F::GrenadeWarningFont, prev_screen.x, prev_screen.y - 12, Color(255, 255, 255, 255), FONT_CENTER, index_to_grenade_name_icon(m_index));
	Drawing::DrawString(F::ESPFont, prev_screen.x + textsizeicon.right - 8, prev_screen.y - textsize.bottom / 2, Color(255, 255, 255, 255), FONT_LEFT, index_to_grenade_name(m_index));*/

	render::DrawFilledCircle(prev_screen.x, prev_screen.y - 10, 30, 180, Color(26, 26, 30, 255));
	//render::DrawFilledCircle(prev_screen.x, prev_screen.y - 10, 30, 180, Color(255, 0, 0, alpha_damage));

	return true;
}

void c_grenade_prediction::grenade_warning(Player* entity)
{
	auto& predicted_nades = g_grenades_pred.get_list();

	static auto last_server_tick = g_csgo.m_cl->m_server_tick;
	if (last_server_tick != g_csgo.m_cl->m_server_tick) {
		predicted_nades.clear();

		last_server_tick = g_csgo.m_cl->m_server_tick;
	}

	if (entity->dormant() || !g_menu.main.visuals.tracers.get())
		return;

	const auto client_class = entity->GetClientClass();
	if (!client_class || client_class->m_ClassID != 114 && client_class->m_ClassID != 9)
		return;

	if (client_class->m_ClassID == 9) {
		const auto model = entity->GetModel();
		if (!model)
			return;

		const auto studio_model = g_csgo.m_model_info->GetStudioModel(model);
		if (!studio_model || std::string_view(studio_model->m_name).find("fraggrenade") == std::string::npos)
			return;
	}

	const auto handle = entity->GetRefEHandle(); // я знаю что можно было по другому но мне похуй

	if (entity->m_nExplodeEffectTickBegin()) {
		predicted_nades.erase(handle);
		return;
	}

	if (predicted_nades.find(handle) == predicted_nades.end()) {
		predicted_nades.emplace(std::piecewise_construct, std::forward_as_tuple(handle), std::forward_as_tuple(reinterpret_cast<Weapon*>(entity)->m_hThrower(), client_class->m_ClassID == 114 ? MOLOTOV : HEGRENADE, entity->m_vecOrigin(), reinterpret_cast<Player*>(entity)->m_vecVelocity(), entity->get_creation_time(), game::TIME_TO_TICKS(reinterpret_cast<Player*>(entity)->m_flSimulationTime() - entity->get_creation_time())));
	}

	if (predicted_nades.at(handle).draw())
		return;

	predicted_nades.erase(handle);
}
