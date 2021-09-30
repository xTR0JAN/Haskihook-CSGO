#include "includes.h"

void Hooks::LevelInitPreEntity(const char* map) {
	float rate{ 1.f / g_csgo.m_globals->m_interval };

	// set rates when joining a server.
	g_csgo.cl_updaterate->SetValue(rate);
	g_csgo.cl_cmdrate->SetValue(rate);

	g_aimbot.reset();
	g_visuals.m_hit_start = g_visuals.m_hit_end = g_visuals.m_hit_duration = 0.f;

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelInitPreEntity_t >(CHLClient::LEVELINITPREENTITY)(this, map);
}

void Hooks::LevelInitPostEntity() {
	g_cl.OnMapload();

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelInitPostEntity_t >(CHLClient::LEVELINITPOSTENTITY)(this);
}

void Hooks::LevelShutdown() {
	g_aimbot.reset();

	g_cl.m_local = nullptr;
	g_cl.m_weapon = nullptr;
	g_cl.m_processing = false;
	g_cl.m_weapon_info = nullptr;

	g_cl.m_sequences.clear();

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelShutdown_t >(CHLClient::LEVELSHUTDOWN)(this);
}

/*int Hooks::IN_KeyEvent( int evt, int key, const char* bind ) {
	// see if this key event was fired for the drop bind.
	/*if( bind && FNV1a::get( bind ) == HASH( "drop" ) ) {
		// down.
		if( evt ) {
			g_cl.m_drop = true;
			g_cl.m_drop_query = 2;
			g_cl.print( "drop\n" );
		}
		// up.
		else
			g_cl.m_drop = false;
		// ignore the event.
		return 0;
	}
	return g_hooks.m_client.GetOldMethod< IN_KeyEvent_t >( CHLClient::INKEYEVENT )( this, evt, key, bind );
}*/

void WriteUsercmd(bf_write* buf, CUserCmd* in, CUserCmd* out)
{
	static auto WriteUsercmdF = pattern::find(g_csgo.m_client_dll, XOR("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"));

	__asm
	{
		mov ecx, buf
		mov edx, in
		push out
		call WriteUsercmdF
		add esp, 4
	}
}

bool Hooks::WriteUsercmdDeltaToBuffer(int slot, bf_write* buf, int from, int to, bool isnewcommand)
{
	if (g_cl.m_processing && g_csgo.m_engine->IsConnected() && g_csgo.m_engine->IsInGame()) {
		if (g_csgo.m_gamerules->m_bFreezePeriod())
			return g_hooks.m_client.GetOldMethod< WriteUsercmdDeltaToBuffer_t >(CHLClient::USRCMDTODELTABUFFER)(this, slot, buf, from, to, isnewcommand);

		if (g_cl.m_tick_to_shift <= 0 || g_csgo.m_cl->m_choked_commands > 3)
			return g_hooks.m_client.GetOldMethod< WriteUsercmdDeltaToBuffer_t >(CHLClient::USRCMDTODELTABUFFER)(this, slot, buf, from, to, isnewcommand);

		if (from != -1)
			return true;

		uintptr_t stackbase;
		__asm mov stackbase, ebp;
		CCLCMsg_Move_t* msg = reinterpret_cast<CCLCMsg_Move_t*>(stackbase + 0xFCC);
		auto net_channel = *reinterpret_cast <INetChannel**> (reinterpret_cast <uintptr_t> (g_csgo.m_cl) + 0x9C);
		int32_t new_commands = msg->new_commands;

		int32_t next_cmdnr = g_csgo.m_cl->m_last_outgoing_command + g_csgo.m_cl->m_choked_commands + 1;
		int32_t total_new_commands = std::min(g_cl.m_tick_to_shift, 17);
		g_cl.m_tick_to_shift -= total_new_commands;

		from = -1;
		msg->new_commands = total_new_commands;
		msg->backup_commands = 0;

		for (to = next_cmdnr - new_commands + 1; to <= next_cmdnr; to++) {
			if (!g_hooks.m_client.GetOldMethod< WriteUsercmdDeltaToBuffer_t >(CHLClient::USRCMDTODELTABUFFER)(this, slot, buf, from, to, isnewcommand))
				return false;

			from = to;
		}

		CUserCmd* last_realCmd = g_csgo.m_input->GetUserCmd(slot, from);
		CUserCmd fromCmd;

		if (last_realCmd)
			fromCmd = *last_realCmd;

		CUserCmd toCmd = fromCmd;
		toCmd.m_command_number++;
		toCmd.m_tick++;

		for (int i = new_commands; i <= total_new_commands; i++) {
			WriteUsercmd(buf, &toCmd, &fromCmd);
			fromCmd = toCmd;
			toCmd.m_command_number++;
			toCmd.m_tick++;
		}
		return true;

	}
	else
		return g_hooks.m_client.GetOldMethod< WriteUsercmdDeltaToBuffer_t >(CHLClient::USRCMDTODELTABUFFER)(this, slot, buf, from, to, isnewcommand);
}

void Hooks::FrameStageNotify(Stage_t stage) {

	// save stage.
	if (stage != FRAME_START)
		g_cl.m_stage = stage;

	// damn son.
	g_cl.m_local = g_csgo.m_entlist->GetClientEntity< Player* >(g_csgo.m_engine->GetLocalPlayer());

	if (stage == FRAME_RENDER_START) {
		// apply local player animated angles.
		g_cl.SetAngles();

		// apply local player animation fix.
		g_cl.UpdateAnimations();

		// draw our custom beams.
		g_visuals.DrawBeams();

		g_visuals.draw_sound();
	}

	/*if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {

		// update our sound esp.
		g_dormancy.PositionCorrection(stage);
	}*/

	// call og.
	g_hooks.m_client.GetOldMethod< FrameStageNotify_t >(CHLClient::FRAMESTAGENOTIFY)(this, stage);

	if (stage == FRAME_RENDER_START) {
		if (g_cl.m_local) {
			if (g_cl.m_local->alive()) {
				//static auto last_count = 0;
				//auto& client_impact_list = *(CUtlVector <client_hit_verify_t>*)((uintptr_t)g_cl.m_local + 0xBC00);
				////client_hit_verify_t
				//for (auto i = client_impact_list.Count(); i > last_count; --i)
				//	g_csgo.m_debug_overlay->AddBoxOverlay(client_impact_list[i - 1].position, vec3_t(-1.5, -1.5, -1.5), vec3_t(1.5, 1.5, 1.5), ang_t(0, 0, 0), 255, 0, 0, 127, 4);
				//
				//if (client_impact_list.Count() != last_count)
				//	last_count = client_impact_list.Count();
			}
		}
	}

	else if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		// restore non-compressed netvars.
		// g_netdata.apply( );

		g_cl.Skybox();
		g_cl.ClanTag();
		g_skins.think();
	}

	else if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END) {
		g_visuals.NoSmoke();
	}

	else if (stage == FRAME_NET_UPDATE_END) {
		// restore non-compressed netvars.
		g_netdata.apply();

		// update all players.
		for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);
			if (!player || player->m_bIsLocalPlayer())
				continue;

			AimPlayer* data = &g_aimbot.m_players[i - 1];
			data->OnNetUpdate(player);
		}
	}
}

bool __cdecl Detours::ReportHit(Hit_t* info) {
	// add world hitmarkers
	if (info) {
		WorldHitmarkerData_t data;

		data.m_alpha = 1.f;
		data.m_time = g_inputpred.m_stored_variables.m_flCurtime;

		data.m_pos_x = info->m_pos_x;
		data.m_pos_y = info->m_pos_y;
		data.m_pos_z = info->m_pos_z;

		g_visuals.m_world_hitmarkers.push_back(data);
	}
	return g_detours.oReportHit(info);
}

void __cdecl Detours::CL_Move(float accumulated_extra_samples, bool bFinalTick)
{
	// call og.
	g_detours.oCL_Move(accumulated_extra_samples, bFinalTick);

	g_tickbase.m_shift_data.m_shifting = true;

	for (int i = g_tickbase.m_shift_data.m_ticks_to_shift; i > 0; g_tickbase.m_shift_data.m_ticks_to_shift--, i--)
	{
		// create an extra movement command (will call CreateMove).
		g_detours.oCL_Move(accumulated_extra_samples, bFinalTick);
	}

	g_tickbase.m_shift_data.m_shifting = false;
}

bool __fastcall Detours::SetupBones(void* ecx, void* edx, matrix3x4_t* bone_to_world_out, int max_bones, int bone_mask, float curtime)
{
	auto pEnt = reinterpret_cast<Player*>(uintptr_t(ecx) - 0x4);

	if (!pEnt || !pEnt->IsPlayer())
		return g_detours.oSetupBones(ecx, bone_to_world_out, max_bones, bone_mask, curtime);

	if (pEnt->index() == g_csgo.m_engine->GetLocalPlayer())
	{
		// fix model sway.
		g_cl.m_local->m_AnimOverlay()[12].m_weight = 0.0f;

		// remove model balance adjust animation (eye candy).
		g_cl.m_local->m_AnimOverlay()[3].m_cycle = 0.f;
		g_cl.m_local->m_AnimOverlay()[3].m_weight = 0.f;

		*(int*)((DWORD)pEnt + 0x2698) = 0;
		bone_mask |= 0x200;

			if (ecx) {
				*reinterpret_cast<int*> (reinterpret_cast<uintptr_t> (ecx) + 0x269C) = 0;

			*reinterpret_cast<float*> (reinterpret_cast<uintptr_t> (pEnt) + 2664) = 0;

			static auto offs_bone_mask = g_entoffsets.m_nForceBone + 0x20;
			*reinterpret_cast<int*> (reinterpret_cast<uintptr_t> (pEnt) + offs_bone_mask) = 0;
		}


		*(uint32_t*)(uintptr_t(pEnt) + 0xA68) = 0;
	}

	pEnt->m_fEffects() |= EF_NOINTERP;
	bool m_bSetuped = g_detours.oSetupBones(ecx, bone_to_world_out, max_bones, bone_mask, curtime);
	pEnt->m_fEffects() &= ~EF_NOINTERP;
	return m_bSetuped;
}