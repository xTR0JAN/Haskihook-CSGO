#include "includes.h"
#include "gh.h"

Client g_cl{ };

// loader will set this fucker.
char username[33] = "\x90\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x90";

// init routine.
ulong_t __stdcall Client::init(void* arg) {
	// if not in interwebz mode, the driver will not set the username.
	g_cl.m_user = XOR("haskihook");

	// stop here if we failed to acquire all the data needed from csgo.
	if (!g_csgo.init())
		return 0;

	g_csgo.m_engine->ExecuteClientCmd("clear");

	// welcome the user.
	g_notify.add(tfm::format(XOR("Welcome to HaskiHook! Hope you have a good time\n")));

	return 1;
}

void Client::DrawHUD() {
	if (!g_menu.main.misc.watermark.get())
		return;

	//if (!g_csgo.m_engine->IsInGame())
		//return; 

	// colors
	const auto col_background = Color(15, 15, 15, 210);
	const auto col_accent = g_menu.main.config.menu_color.get();

	// get time.
	time_t t = std::time(nullptr);
	std::ostringstream time;
	time << std::put_time(std::localtime(&t), ("%H:%M:%S"));

	// get round trip time in milliseconds.
	int ms = std::max(0, (int)std::round(g_cl.m_latency * 1000.f));

	// TODO: 
	// extern t_Username GetUsername;

	// get tickrate.
	int rate = (int)std::round(1.f / g_csgo.m_globals->m_interval);

	// get framerate.
	int fps = (int)std::round(1.f / g_csgo.m_globals->m_frametime);

	// date
	std::string date = XOR(__DATE__, __TIME__, __TIMESTAMP__);

	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/29e4c1fda9698d5cebcdaf1a0de4b829fa149bf8/game/client/c_memorylog.cpp#L79
	bool local = g_csgo.m_engine->GetNetChannelInfo() ? strstr(g_csgo.m_engine->GetNetChannelInfo()->GetAddress(), XOR("loopback")) : XOR("");

	// get server ip / type
	const char* server_ip = g_csgo.m_engine->GetNetChannelInfo() ? g_csgo.m_engine->GetNetChannelInfo()->GetAddress() : XOR("");

	std::string text = g_csgo.m_engine->IsInGame() ? tfm::format(XOR("slaves [DEBUG] | delay: %ims | %itick | %s | %s"), ms, rate, local ? XOR("local server") : server_ip, time.str().data()) : tfm::format(XOR("slaves [DEBUG] | in menu | %s"), time.str().data());
	render::FontSize_t size = render::esp.size(text);

	// background.
	render::rect_filled(m_width - size.m_width - 18, 10, size.m_width + 8, size.m_height + 5, col_background);
	render::rect_filled(m_width - size.m_width - 18, 10, size.m_width + 8, 1, col_accent);
	render::rect_filled(m_width - size.m_width - 17, 9, size.m_width + 6, 2, col_accent);
	render::rect_filled(m_width - size.m_width - 16, 8, size.m_width + 4, 3, col_accent);

	// text.
	render::esp.string(m_width - 15, 12, { 255, 255, 255, 255 }, text, render::ALIGN_RIGHT);
}

void Client::ClanTag()
{
	// lambda function for setting our clantag.
	auto SetClanTag = [&](std::string tag) -> void {
		using SetClanTag_t = int(__fastcall*)(const char*, const char*);
		static auto SetClanTagFn = pattern::find(g_csgo.m_engine_dll, XOR("53 56 57 8B DA 8B F9 FF 15")).as<SetClanTag_t>();

		SetClanTagFn(tag.c_str(), XOR("")); //here you can set nothing lol
	};

	std::string szClanTag = XOR(" haskihook ");
	std::string szSuffix = XOR(" best hake =D ");
	static int iPrevFrame = 0;
	static bool bReset = false;
	int iCurFrame = ((int)(g_csgo.m_globals->m_curtime * 2.f)) % (szClanTag.size() * 2);

	if (g_menu.main.misc.clantag.get()) {
		// are we in a new frame?

		if (iPrevFrame != int(g_csgo.m_globals->m_curtime * 2.4) % 18) {
			switch (int(g_csgo.m_globals->m_curtime * 2.4) % 30) {
				/*	case 0: {  SetClanTag("|h"); break; }
					case 1: {  SetClanTag("|ha"); break; }
					case 2: {  SetClanTag("|has"); break; }
					case 3: {  SetClanTag("|hask"); break; }
					case 4: {  SetClanTag("|haski"); break; }
					case 5: {  SetClanTag("|haski"); break; }
					case 6: {  SetClanTag("|haskih"); break; }
					case 7: {  SetClanTag("|haskiho"); break; }
					case 8: {  SetClanTag("|haskihoo"); break; }
					case 9: {  SetClanTag("|haskihook"); break; }
					case 11: {  SetClanTag("|h4sk1h00k"); break; }
					case 12: {  SetClanTag("|haskihook|"); break; }
					case 13: {  SetClanTag("|haskihook|"); break; }
					case 14: {  SetClanTag("|haskihoo|"); break; }
					case 15: {  SetClanTag("|haskiho|"); break; }
					case 16: {  SetClanTag("|haskih|"); break; }
					case 17: {  SetClanTag("|haski|"); break; }
					case 18: {  SetClanTag("|hask|"); break; }
					case 19: {  SetClanTag("|has|"); break; }
					case 20: {  SetClanTag("|ha|"); break; }
					case 21: {  SetClanTag("|h|"); break; }
					case 22: {  SetClanTag("|"); break; }
					case 23: {  SetClanTag("|DEB"); break; }
					case 24: {  SetClanTag("|DEBU"); break; }
					case 25: {  SetClanTag("DEBUG|"); break; }
					case 26: {  SetClanTag("DEBU|"); break; }
					case 27: {  SetClanTag("DEB|"); break; }
					case 28: {  SetClanTag("DE|"); break; }
					case 29: {  SetClanTag("D|"); break; }
					case 30: {  SetClanTag("|"); break; }*/
			case 0: { SetClanTag("v       "); break; }
			case 1: { SetClanTag("ve      "); break; }
			case 2: { SetClanTag("vel     "); break; }
			case 3: { SetClanTag("velo    "); break; }
			case 4: { SetClanTag("veloc   "); break; }
			case 5: { SetClanTag("veloci  "); break; }
			case 6: { SetClanTag("velocit "); break; }
			case 7: { SetClanTag("velocity"); break; }
			case 8: { SetClanTag("velocity"); break; }
			case 9: { SetClanTag("velocit "); break; }
			case 10: { SetClanTag("veloci  "); break; }
			case 11: { SetClanTag("veloc   "); break; }
			case 12: { SetClanTag("velo    "); break; }
			case 13: { SetClanTag("vel     "); break; }
			case 14: { SetClanTag("ve      "); break; }
			case 15: { SetClanTag("v       "); break; }

			default:;
			}
			iPrevFrame = int(g_csgo.m_globals->m_curtime * 2.4) % 18;
		}

		// do we want to reset after untoggling the clantag?
		bReset = true;
	}
	else {
		// reset our clantag.
		if (bReset) {
			SetClanTag(XOR(""));
			bReset = false;
		}
	}
}

void Client::Skybox()
{
	static auto sv_skyname = g_csgo.m_cvar->FindVar(HASH("sv_skyname"));
	static auto r_3dsky = g_csgo.m_cvar->FindVar(HASH("r_3dsky"));
	r_3dsky->SetValue(XOR("1"));
	if (g_menu.main.misc.skyboxchange.get()) {
		switch (g_menu.main.misc.skybox.get()) {
		case 0: //Tibet
			//sv_skyname->SetValue("cs_tibet");
			sv_skyname->SetValue(XOR("cs_tibet"));
			r_3dsky->SetValue(XOR("0"));
			break;
		case 1: //Embassy
			//sv_skyname->SetValue("embassy");
			sv_skyname->SetValue(XOR("embassy"));
			r_3dsky->SetValue(XOR("0"));
			break;
		case 2: //Italy
			//sv_skyname->SetValue("italy");
			sv_skyname->SetValue(XOR("italy"));
			r_3dsky->SetValue(XOR("0"));
			break;
		case 3: //Daylight 1
			//sv_skyname->SetValue("sky_cs15_daylight01_hdr");
			sv_skyname->SetValue(XOR("sky_cs15_daylight01_hdr"));
			r_3dsky->SetValue(XOR("0"));
			break;
		case 4: //Cloudy
			//sv_skyname->SetValue("sky_csgo_cloudy01");
			sv_skyname->SetValue(XOR("sky_csgo_cloudy01"));
			r_3dsky->SetValue(XOR("0"));
			break;
		case 5: //Night 1
			sv_skyname->SetValue(XOR("sky_csgo_night02"));
			r_3dsky->SetValue(XOR("0"));
			break;
		case 6: //Night 2
			//sv_skyname->SetValue("sky_csgo_night02b");
			sv_skyname->SetValue(XOR("sky_csgo_night02b"));
			r_3dsky->SetValue(XOR("0"));
			break;
		case 7: //Night Flat
			//sv_skyname->SetValue("sky_csgo_night_flat");
			sv_skyname->SetValue(XOR("sky_csgo_night_flat"));
			r_3dsky->SetValue(XOR("0"));
			break;
		case 8: //Day HD
			//sv_skyname->SetValue("sky_day02_05_hdr");
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("sky_day02_05_hdr"));
			break;
		case 9: //Day
			//sv_skyname->SetValue("sky_day02_05");
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("sky_day02_05"));
			break;
		case 10: //Rural
			//sv_skyname->SetValue("sky_l4d_rural02_ldr");
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("sky_l4d_rural02_ldr"));
			break;
		case 11: //Vertigo HD
			//sv_skyname->SetValue("vertigo_hdr");
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("vertigo_hdr"));
			break;
		case 12: //Vertigo Blue HD
			//sv_skyname->SetValue("vertigoblue_hdr");
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("vertigoblue_hdr"));
			break;
		case 13: //Vertigo
			//sv_skyname->SetValue("vertigo");
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("vertigo"));
			break;
		case 14: //Vietnam
			//sv_skyname->SetValue("vietnam");
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("vietnam"));
			break;
		case 15: //Dusty Sky
			//sv_skyname->SetValue("sky_dust");
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("sky_dust"));
			break;
		case 16: //Jungle
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("jungle"));
			break;
		case 17: //Nuke
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("nukeblank"));
			break;
		case 18: //Office
			r_3dsky->SetValue(XOR("0"));
			sv_skyname->SetValue(XOR("office"));
			//game::SetSkybox(XOR("office"));
			break;
		default:
			break;
		}
	}

	/*
	Checkbox	FogOverride; // butt
	Colorpicker	FogColor; // color
	Slider		FogStart; // slider
	Slider		FogEnd; // slider
	Slider		Fogdensity; // slider
	*/
	//g_menu.main.visuals.FogColor.get().r(), g_menu.main.visuals.FogColor.get().g(), g_menu.main.visuals.FogColor.get().b()

	float destiny = g_menu.main.world.Fogdensity.get() / 100.f;

	static const auto fog_enable = g_csgo.m_cvar->FindVar(HASH("fog_enable"));
	fog_enable->SetValue(1); //Âêëþ÷àåò òóìàí íà êàðòå åñëè îí âûêëþ÷åí ïî äåôîëòó
	static const auto fog_override = g_csgo.m_cvar->FindVar(HASH("fog_override"));
	fog_override->SetValue(g_menu.main.world.FogOverride.get()); // Ðàçðåøàåò êàñòîìèçàöèþ òóìàíà
	static const auto fog_color = g_csgo.m_cvar->FindVar(HASH("fog_color"));
	fog_color->SetValue(std::string(std::to_string(g_menu.main.world.FogColor.get().r()) + " " + std::to_string(g_menu.main.world.FogColor.get().g()) + " " + std::to_string(g_menu.main.world.FogColor.get().b())).c_str()); //Öâåò òóìàíà rgb
	static const auto fog_start = g_csgo.m_cvar->FindVar(HASH("fog_start"));
	fog_start->SetValue(g_menu.main.world.FogStart.get()); // Äèñòàíöèÿ ñ êîòîðîé òóìàí ïîÿâëÿåòñÿ
	static const auto fog_end = g_csgo.m_cvar->FindVar(HASH("fog_end"));
	fog_end->SetValue(g_menu.main.world.FogEnd.get()); // Äèñòàíöèÿ ñ êîòîðîé òóìàí ïðîïàäàåò
	static const auto fog_destiny = g_csgo.m_cvar->FindVar(HASH("fog_maxdensity"));
	fog_destiny->SetValue(destiny); //Ìàêñèìàëüíàÿ íàñûùåííîñòü òóìàíà(0-1)
}

void Client::MouseDelta()
{
	// reason for this is to fix mousedx/dy
	// idk why it needs fixing but skeet, onetap and aimware do it.
	// the following code is from aimware

	if (!m_local || !m_cmd)
		return;

	static ang_t delta_viewangles{ };
	ang_t delta = m_cmd->m_view_angles - delta_viewangles;
	delta.clamp();

	static ConVar* sensitivity = g_csgo.m_cvar->FindVar(HASH("sensitivity"));

	if (!sensitivity)
		return;

	if (delta.x != 0.f) {
		static ConVar* m_pitch = g_csgo.m_cvar->FindVar(HASH("m_pitch"));

		if (!m_pitch)
			return;

		int final_dy = static_cast<int>((delta.x / m_pitch->GetFloat()) / sensitivity->GetFloat());
		if (final_dy <= 32767) {
			if (final_dy >= -32768) {
				if (final_dy >= 1 || final_dy < 0) {
					if (final_dy <= -1 || final_dy > 0)
						final_dy = final_dy;
					else
						final_dy = -1;
				}
				else {
					final_dy = 1;
				}
			}
			else {
				final_dy = 32768;
			}
		}
		else {
			final_dy = 32767;
		}

		m_cmd->m_mousedy = static_cast<short>(final_dy);
	}

	if (delta.y != 0.f) {
		static ConVar* m_yaw = g_csgo.m_cvar->FindVar(HASH("m_yaw"));

		if (!m_yaw)
			return;

		int final_dx = static_cast<int>((delta.y / m_yaw->GetFloat()) / sensitivity->GetFloat());
		if (final_dx <= 32767) {
			if (final_dx >= -32768) {
				if (final_dx >= 1 || final_dx < 0) {
					if (final_dx <= -1 || final_dx > 0)
						final_dx = final_dx;
					else
						final_dx = -1;
				}
				else {
					final_dx = 1;
				}
			}
			else {
				final_dx = 32768;
			}
		}
		else {
			final_dx = 32767;
		}

		m_cmd->m_mousedx = static_cast<short>(final_dx);
	}

	delta_viewangles = m_cmd->m_view_angles;
}

void Client::KillFeed() {
	if (!g_menu.main.misc.killfeed.get())
		return;

	if (!g_csgo.m_engine->IsInGame())
		return;

	// get the addr of the killfeed.
	KillFeed_t* feed = (KillFeed_t*)g_csgo.m_hud->FindElement(HASH("SFHudDeathNoticeAndBotStatus"));
	if (!feed)
		return;

	int size = feed->notices.Count();
	if (!size)
		return;

	for (int i{ }; i < size; ++i) {
		NoticeText_t* notice = &feed->notices[i];

		// this is a local player kill, delay it.
		if (notice->fade == 1.5f)
			notice->fade = g_menu.main.misc.killfeed_time.get();
	}
}

void Client::OnPaint() {
	// update screen size.
	g_csgo.m_engine->GetScreenSize(m_width, m_height);

	// render stuff.
	g_visuals.think();
	g_grenades.paint();
	g_notify.think();

	for (auto k = 0; k < g_csgo.m_entlist->GetHighestEntityIndex(); k++)
	{
		Player* entity = g_csgo.m_entlist->GetClientEntity(k)->as<Player*>();

		if (entity == nullptr ||
			!entity->GetClientClass() ||
			entity == g_cl.m_local)
			continue;

		g_grenades_pred.grenade_warning(entity);
		g_grenades_pred.get_local_data().draw();
	}

	DrawHUD();

	g_visuals.HitmarkerWorld();
	g_visuals.IndicateAngles();

	KillFeed();

	// menu goes last.
	g_gui.think();
}

void Client::OnMapload() {
	// store class ids.
	g_netvars.SetupClassData();

	// createmove will not have been invoked yet.
	// but at this stage entites have been created.
	// so now we can retrive the pointer to the local player.
	m_local = g_csgo.m_entlist->GetClientEntity< Player* >(g_csgo.m_engine->GetLocalPlayer());

	// world materials.
	g_visuals.ModulateWorld();

	// init knife shit.
	g_skins.load();

	m_sequences.clear();

	// if the INetChannelInfo pointer has changed, store it for later.
	g_csgo.m_net = g_csgo.m_engine->GetNetChannelInfo();

	if (g_csgo.m_net) {
		g_hooks.m_net_channel.reset();
		g_hooks.m_net_channel.init(g_csgo.m_net);
		g_hooks.m_net_channel.add(INetChannel::PROCESSPACKET, util::force_cast(&Hooks::ProcessPacket));
		g_hooks.m_net_channel.add(INetChannel::SENDDATAGRAM, util::force_cast(&Hooks::SendDatagram));
	}
}

void Client::StartMove(CUserCmd* cmd) {
	// save some usercmd stuff.
	m_cmd = cmd;
	m_tick = cmd->m_tick;
	m_view_angles = cmd->m_view_angles;
	m_buttons = cmd->m_buttons;
	g_inputpred.m_stored_variables.m_flForwardMove = cmd->m_forward_move;
	g_inputpred.m_stored_variables.m_flSideMove = cmd->m_side_move;

	// get local ptr.
	m_local = g_csgo.m_entlist->GetClientEntity< Player* >(g_csgo.m_engine->GetLocalPlayer());
	if (!m_local)
		return;

	g_csgo.m_net = g_csgo.m_engine->GetNetChannelInfo();

	if (!g_csgo.m_net)
		return;

	// store max choke
	// TODO; 11 -> m_bIsValveDS
	m_max_lag = (m_local->m_fFlags() & FL_ONGROUND) ? 16 : 15;
	m_lag = g_csgo.m_cl->m_choked_commands;
	m_lerp = game::GetClientInterpAmount();
	m_latency = g_csgo.m_net->GetLatency(INetChannel::FLOW_OUTGOING);
	math::clamp(m_latency, 0.f, 1.f);
	m_latency_ticks = game::TIME_TO_TICKS(m_latency);
	m_server_tick = g_csgo.m_cl->m_server_tick;
	m_arrival_tick = m_server_tick + m_latency_ticks;

	// processing indicates that the localplayer is valid and alive.
	m_processing = m_local && m_local->alive();
	if (!m_processing)
		return;

	// make sure prediction has ran on all usercommands.
	// because prediction runs on frames, when we have low fps it might not predict all usercommands.
	// also fix the tick being inaccurate.
	g_inputpred.update();

	// store some stuff about the local player.
	m_flags = m_local->m_fFlags();

	// ...
	m_shot = false;
}

void Client::BackupPlayers(bool restore) {
	if (restore) {
		// restore stuff.
		for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);

			if (!g_aimbot.IsValidTarget(player))
				continue;

			g_aimbot.m_backup[i - 1].restore(player);
		}
	}

	else {
		// backup stuff.
		for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);

			if (!g_aimbot.IsValidTarget(player))
				continue;

			g_aimbot.m_backup[i - 1].store(player);
		}
	}
}

void Client::DoMove() {
	penetration::PenetrationOutput_t tmp_pen_data{ };

	// backup strafe angles (we need them for input prediction)
	m_strafe_angles = m_cmd->m_view_angles;

	// run movement code before input prediction.
	g_movement.JumpRelated();
	g_movement.Strafe();
	g_movement.FakeWalk();
	g_movement.AutoPeek();
	g_movement.Fast_stop();

	// predict input.
	g_inputpred.run();

	// restore original angles after input prediction
	m_cmd->m_view_angles = m_view_angles;

	// convert viewangles to directional forward vector.
	math::AngleVectors(m_view_angles, &m_forward_dir);

	// store stuff after input pred.
	m_shoot_pos = m_local->GetShootPosition();

	// reset shit.
	m_weapon = nullptr;
	m_weapon_info = nullptr;
	m_weapon_id = -1;
	m_weapon_type = WEAPONTYPE_UNKNOWN;
	m_player_fire = m_weapon_fire = false;

	// store weapon stuff.
	m_weapon = m_local->GetActiveWeapon();

	if (m_weapon) {
		m_weapon_info = m_weapon->GetWpnData();
		m_weapon_id = m_weapon->m_iItemDefinitionIndex();
		m_weapon_type = m_weapon_info->m_weapon_type;

		// ensure weapon spread values / etc are up to date.
		if (m_weapon_type != WEAPONTYPE_GRENADE)
			m_weapon->UpdateAccuracyPenalty();

		// run autowall once for penetration crosshair if we have an appropriate weapon.
		if (m_weapon_type != WEAPONTYPE_KNIFE && m_weapon_type != WEAPONTYPE_C4 && m_weapon_type != WEAPONTYPE_GRENADE) {
			penetration::PenetrationInput_t in;
			in.m_from = m_local;
			in.m_target = nullptr;
			in.m_pos = m_shoot_pos + (m_forward_dir * m_weapon_info->m_range);
			in.m_damage = 1.f;
			in.m_damage_pen = 1.f;
			in.m_can_pen = true;

			// run autowall.
			penetration::run(&in, &tmp_pen_data);
		}

		// set pen data for penetration crosshair.
		m_pen_data = tmp_pen_data;

		// can the player fire.
		m_player_fire = g_csgo.m_globals->m_curtime >= m_local->m_flNextAttack() && !g_csgo.m_gamerules->m_bFreezePeriod() && !(g_cl.m_flags & FL_FROZEN);

		UpdateRevolverCock();
		m_weapon_fire = CanFireWeapon(game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()));
	}

	// last tick defuse.
	// todo - dex;  figure out the range for CPlantedC4::Use?
	//              add indicator if valid (on ground, still have time, not being defused already, etc).
	//              move this? not sure where we should put it.
	if (g_input.GetKeyState(g_menu.main.misc.last_tick_defuse.get()) && g_visuals.m_c4_planted) {
		float defuse = (m_local->m_bHasDefuser()) ? 5.f : 10.f;
		float remaining = g_visuals.m_planted_c4_explode_time - g_csgo.m_globals->m_curtime;
		float dt = remaining - defuse - (g_cl.m_latency / 2.f);

		m_cmd->m_buttons &= ~IN_USE;
		if (dt <= game::TICKS_TO_TIME(2))
			m_cmd->m_buttons |= IN_USE;
	}

	// grenade prediction.
	g_grenades.think();

	// run fakelag.
	g_hvh.SendPacket();

	// run aimbot.
	g_aimbot.think();

	// run antiaims.
	g_hvh.AntiAim();
}

void Client::EndMove(CUserCmd* cmd) {
	// update client-side animations.
	UpdateInformation();

	// if matchmaking mode, anti untrust clamp.
	if (g_menu.main.config.mode.get() == 0)
		m_cmd->m_view_angles.SanitizeAngle();

	// fix our movement.
	g_movement.FixMove(cmd, m_strafe_angles);
	g_movement.MoonWalk(cmd);

	// this packet will be sent.
	if (*m_packet) {
		g_hvh.m_step_switch = (bool)g_csgo.RandomInt(0, 1);

		// we are sending a packet, so this will be reset soon.
		// store the old value.
		m_old_lag = m_lag;

		// get radar angles.
		m_radar = cmd->m_view_angles;
		m_radar.normalize();

		// get current origin.
		vec3_t cur = m_local->m_vecOrigin();

		// get prevoius origin.StartMove
		vec3_t prev = m_net_pos.empty() ? cur : m_net_pos.front().m_pos;

		// check if we broke lagcomp.
		m_lagcomp = (cur - prev).length_sqr() > 4096.f;

		// save sent origin and time.
		m_net_pos.emplace_front(g_csgo.m_globals->m_curtime, cur);
	}

	// store some values for next tick.
	m_old_packet = *m_packet;
	m_old_shot = m_shot;
}

void Client::OnTick(CUserCmd* cmd) {
	// TODO; add this to the menu.
	if (g_menu.main.misc.ranks.get() && cmd->m_buttons & IN_SCORE) {
		static CCSUsrMsg_ServerRankRevealAll msg{ };
		g_csgo.ServerRankRevealAll(&msg);
	}

	MouseDelta();

	// store some data and update prediction.
	StartMove(cmd);

	// not much more to do here.
	if (!m_processing)
		return;

	// save the original state of players.
	BackupPlayers(false);

	// run all movement related code.
	DoMove();

	// store stome additonal stuff for next tick
	// sanetize our usercommand if needed and fix our movement.
	EndMove(cmd);

	// restore the players.
	BackupPlayers(true);

	// restore curtime/frametime
	// and prediction seed/player.
	g_inputpred.restore();
	g_aimbot.DoubleTap();
	if (g_aimbot.m_double_tap)
	{
		if (!g_aimbot.CanDT())
			m_charged = false;
		else if (!m_charged && m_tick_to_recharge == 0) {
			m_tick_to_recharge = 17;
		}
	}
}

void Client::SetAngles() {
	if (!g_cl.m_local || !g_cl.m_processing)
		return;

	// set the nointerp flag.
	g_cl.m_local->m_fEffects() |= EF_NOINTERP;

	// apply the rotation.
	g_cl.m_local->SetAbsAngles(m_rotation);
	g_cl.m_local->m_angRotation() = m_rotation;
	g_cl.m_local->m_angNetworkAngles() = m_rotation;

	// set radar angles.
	if (g_csgo.m_input->CAM_IsThirdPerson())
		g_csgo.m_prediction->SetLocalViewAngles(m_radar);
}

void Client::SetAngles2(ang_t angle) {
	if (!g_cl.m_local || !g_cl.m_processing || !g_menu.main.antiaim.fake_yaw.get())
		return;

	// set the nointerp flag.
	g_cl.m_local->m_fEffects() |= EF_NOINTERP;

	// apply the rotation.
	g_cl.m_local->SetAbsAngles(angle);
	g_cl.m_local->m_angRotation() = angle;
	g_cl.m_local->m_angNetworkAngles() = angle;
}

void Client::UpdateAnimations() {
	if (!g_cl.m_local || !g_cl.m_processing)
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return;

	//get standing view
	vec3_t stand_eye_pos = g_csgo.m_game_movement->GetPlayerViewOffset(false);
	vec3_t duck_eye_pos = g_csgo.m_game_movement->GetPlayerViewOffset(true);

	// fix model folding
	if (!g_visuals.m_thirdperson && state->m_land && !GetAsyncKeyState(0x20)) {
		if (g_cl.m_local->m_bDucking()) {
			g_cl.m_local->ModifyEyePosition(state, &duck_eye_pos);
			g_hooks.m_UpdateClientSideAnimation(g_cl.m_local);

			// stop landing animation.
			g_cl.m_local->m_AnimOverlay()[5].m_weight = 0.f;

			// prevent model sway on player.
			g_cl.m_local->m_AnimOverlay()[12].m_weight = 0.f;

			// update animations with last networked data.
			g_cl.m_local->SetPoseParameters(g_cl.m_poses);

			// update abs yaw with last networked abs yaw.
			g_cl.m_local->SetAbsAngles(ang_t(0.f, g_cl.m_abs_yaw, 0.f));
		}
		else {
			g_cl.m_local->ModifyEyePosition(state, &stand_eye_pos);
			g_hooks.m_UpdateClientSideAnimation(g_cl.m_local);

			// stop landing animation.
			g_cl.m_local->m_AnimOverlay()[5].m_weight = 0.f;

			// prevent model sway on player.
			g_cl.m_local->m_AnimOverlay()[12].m_weight = 0.f;

			// update animations with last networked data.
			g_cl.m_local->SetPoseParameters(g_cl.m_poses);

			// update abs yaw with last networked abs yaw.
			g_cl.m_local->SetAbsAngles(ang_t(0.f, g_cl.m_abs_yaw, 0.f));
		}

	}

	//sync attachments with player position.
	static const auto jiggle_bones = g_csgo.m_cvar->FindVar(HASH("r_jiggle_bones"));
	jiggle_bones->SetValue(0);

	// prevent model sway on player.
	g_cl.m_local->m_AnimOverlay()[12].m_weight = 0.f;

	// update animations with last networked data.
	g_cl.m_local->SetPoseParameters(g_cl.m_poses);

	// stop falling animation.
	//g_cl.m_local->m_AnimOverlay()[4].m_weight = 0.f;

	// update abs yaw with last networked abs yaw.
	g_cl.m_local->SetAbsAngles(ang_t(0.f, g_cl.m_abs_yaw, 0.f));
}


void Client::UpdateInformation() {
	if (g_cl.m_lag > 0)
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return;

	// update time.
	m_anim_frame = g_csgo.m_globals->m_curtime - m_anim_time;
	m_anim_time = g_csgo.m_globals->m_curtime;

	// current angle will be animated.
	m_angle = g_cl.m_cmd->m_view_angles;

	// fix landing anim.
	if (state->m_land && !state->m_dip_air && state->m_dip_cycle > 0.f) {
		g_cl.m_local->m_AnimOverlay()[4].m_weight = 0.f;
		m_angle.x = -12.f;
	}
	math::clamp(m_angle.x, -90.f, 90.f);
	m_angle.normalize();

	// write angles to model.
	g_csgo.m_prediction->SetLocalViewAngles(m_angle);

	// set lby to predicted value.
	g_cl.m_local->m_flLowerBodyYawTarget() = m_body;

	// CCSGOPlayerAnimState::Update, bypass already animated checks.
	if (state->m_frame == g_csgo.m_globals->m_frame)
		state->m_frame = g_csgo.m_globals->m_frame - 1;

	// call original, bypass hook.
	g_hooks.m_UpdateClientSideAnimation(g_cl.m_local);

	// get last networked poses.
	g_cl.m_local->GetPoseParameters(g_cl.m_poses);

	// store updated abs yaw.
	g_cl.m_abs_yaw = state->m_goal_feet_yaw;

	// we landed.
	if (!m_ground && state->m_ground) {
		m_body = m_angle.y;
		m_body_pred = m_anim_time;
	}

	// walking, delay lby update by .22.
	else if (state->m_speed > 0.1f) {
		if (state->m_ground)
			m_body = m_angle.y;

		m_body_pred = m_anim_time + 0.22f;
	}

	// standing update every 1.1s
	else if (m_anim_time > m_body_pred) {
		m_body = m_angle.y;
		m_body_pred = m_anim_time + 1.1f;
	}

	// save updated data.
	m_rotation = g_cl.m_local->m_angAbsRotation();
	m_speed = state->m_speed;
	m_ground = state->m_ground;
}

void Client::print(const std::string text, ...) {
	va_list     list;
	int         size;
	std::string buf;

	if (text.empty())
		return;

	va_start(list, text);

	// count needed size.
	size = std::vsnprintf(0, 0, text.c_str(), list);

	// allocate.
	buf.resize(size);

	// print to buffer.
	std::vsnprintf(buf.data(), size + 1, text.c_str(), list);

	va_end(list);

	// print to console.
	g_csgo.m_cvar->ConsoleColorPrintf(g_menu.main.config.menu_color.get(), XOR("")); //[haskihook]
	g_csgo.m_cvar->ConsoleColorPrintf(colors::white, buf.c_str());
}

bool Client::CanFireWeapon(float curtime) {
	// the player cant fire.
	if (!m_player_fire)
		return false;

	if (m_weapon_type == WEAPONTYPE_GRENADE)
		return false;

	// if we have no bullets, we cant shoot.
	if (m_weapon_type != WEAPONTYPE_KNIFE && m_weapon->m_iClip1() < 1)
		return false;

	// do we have any burst shots to handle?
	if ((m_weapon_id == GLOCK || m_weapon_id == FAMAS) && m_weapon->m_iBurstShotsRemaining() > 0) {
		// new burst shot is coming out.
		if (g_csgo.m_globals->m_curtime >= m_weapon->m_fNextBurstShot())
			return true;
	}

	// r8 revolver.
	if (m_weapon_id == REVOLVER) {
		int act = m_weapon->m_Activity();

		// mouse1.
		if (!m_revolver_fire) {
			if ((act == 185 || act == 193) && m_revolver_cock == 0)
				return g_csgo.m_globals->m_curtime >= m_weapon->m_flNextPrimaryAttack();

			return false;
		}
	}

	// yeez we have a normal gun.
	if (g_csgo.m_globals->m_curtime >= m_weapon->m_flNextPrimaryAttack())
		return true;

	return false;
}

bool Client::IsFiring(float curtime) {
	const auto weapon = g_cl.m_weapon;
	if (!weapon)
		return false;

	const auto IsZeus = m_weapon_id == Weapons_t::ZEUS;
	const auto IsKnife = !IsZeus && m_weapon_type == WEAPONTYPE_KNIFE;

	if (weapon->IsGrenade())
		return !weapon->m_bPinPulled() && weapon->m_fThrowTime() > 0.f && weapon->m_fThrowTime() < curtime;
	else if (IsKnife)
		return (g_cl.m_cmd->m_buttons & (IN_ATTACK) || g_cl.m_cmd->m_buttons & (IN_ATTACK2)) && CanFireWeapon(curtime);
	else
		return g_cl.m_cmd->m_buttons & (IN_ATTACK) && CanFireWeapon(curtime);
}

void Client::UpdateRevolverCock() {
	// default to false.
	m_revolver_fire = false;

	// reset properly.
	if (m_revolver_cock == -1)
		m_revolver_cock = 0;

	// we dont have a revolver.
	// we have no ammo.
	// player cant fire
	// we are waiting for we can shoot again.
	if (m_weapon_id != REVOLVER || m_weapon->m_iClip1() < 1 || !m_player_fire || g_csgo.m_globals->m_curtime < m_weapon->m_flNextPrimaryAttack()) {
		// reset.
		m_revolver_cock = 0;
		m_revolver_query = 0;
		return;
	}

	// calculate max number of cocked ticks.
	// round to 6th decimal place for custom tickrates..
	int shoot = (int)(0.25f / (std::round(g_csgo.m_globals->m_interval * 1000000.f) / 1000000.f));

	// amount of ticks that we have to query.
	m_revolver_query = shoot - 1;

	// we held all the ticks we needed to hold.
	if (m_revolver_query == m_revolver_cock) {
		// reset cocked ticks.
		m_revolver_cock = -1;

		// we are allowed to fire, yay.
		m_revolver_fire = true;
	}

	else {
		// we still have ticks to query.
		// apply inattack.
		if (g_menu.main.config.mode.get() == 0 && m_revolver_query > m_revolver_cock)
			m_cmd->m_buttons |= IN_ATTACK;

		// count cock ticks.
		// do this so we can also count 'legit' ticks
		// that didnt originate from the hack.
		if (m_cmd->m_buttons & IN_ATTACK)
			m_revolver_cock++;

		// inattack was not held, reset.
		else m_revolver_cock = 0;
	}

	// remove inattack2 if cocking.
	if (m_revolver_cock > 0)
		m_cmd->m_buttons &= ~IN_ATTACK2;
}

void Client::UpdateIncomingSequences() {
	if (!g_csgo.m_net)
		return;

	if (m_sequences.empty() || g_csgo.m_net->m_in_seq > m_sequences.front().m_seq) {
		// store new stuff.
		m_sequences.emplace_front(g_csgo.m_globals->m_realtime, g_csgo.m_net->m_in_rel_state, g_csgo.m_net->m_in_seq);
	}

	// do not save too many of these.
	while (m_sequences.size() > 2048)
		m_sequences.pop_back();
}