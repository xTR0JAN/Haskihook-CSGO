#include "includes.h"

Resolver g_resolver{};;

LagRecord* Resolver::FindIdealRecord(AimPlayer* data) {
	LagRecord* first_valid, * current;

	if (data->m_records.empty())
		return nullptr;

	first_valid = nullptr;

	// iterate records.
	for (const auto& it : data->m_records) {
		if (it->dormant() || it->immune() || !it->valid())
			continue;

		// get current record.
		current = it.get();

		// first record that was valid, store it for later.
		if (!first_valid)
			first_valid = current;

		// try to find a record with a shot, lby update, walking or no anti-aim.
		//if (it->m_shot || it->m_mode == Modes::RESOLVE_LAST_LBY || it->m_mode == Modes::RESOLVE_WALK || it->m_mode == Modes::RESOLVE_NONE || it->m_mode == Modes::RESOLVE_LBY_UPDATE)
			//return current;
		if (it->m_shot || it->m_mode == Modes::RESOLVE_BODY || it->m_mode == Modes::RESOLVE_WALK || it->m_mode == Modes::RESOLVE_NONE)
			return current;
	}

	// none found above, return the first valid record if possible.
	return (first_valid) ? first_valid : nullptr;
}

LagRecord* Resolver::FindLastRecord(AimPlayer* data) {
	LagRecord* current;

	if (data->m_records.empty())
		return nullptr;

	// iterate records in reverse.
	for (auto it = data->m_records.crbegin(); it != data->m_records.crend(); ++it) {
		current = it->get();

		// if this record is valid.
		// we are done since we iterated in reverse.
		if (current->valid() && !current->immune() && !current->dormant())
			return current;
	}

	return nullptr;
}

void Resolver::OnBodyUpdate(Player* player, float value) {
	AimPlayer* data = &g_aimbot.m_players[player->index() - 1];

	// set data.
	data->m_old_body = data->m_body;
	data->m_body = value;
}

float Resolver::GetAwayAngle(LagRecord* record) {
	float  delta{ std::numeric_limits< float >::max() };
	vec3_t pos;
	ang_t  away;

	if (g_cl.m_net_pos.empty()) {
		math::VectorAngles(g_cl.m_local->m_vecOrigin() - record->m_pred_origin, away);
		return away.y;
	}

	float owd = (g_cl.m_latency / 2.f);

	float target = record->m_pred_time;

	// iterate all.
	for (const auto& net : g_cl.m_net_pos) {
		float dt = std::abs(target - net.m_time);

		// the best origin.
		if (dt < delta) {
			delta = dt;
			pos = net.m_pos;
		}
	}

	math::VectorAngles(pos - record->m_pred_origin, away);
	return away.y;
}

void Resolver::MatchShot(AimPlayer* data, LagRecord* record) {
	// do not attempt to do this in nospread mode.
	if (g_menu.main.misc.mode.get() == 1)
		return;

	float shoot_time = -1.f;

	Weapon* weapon = data->m_player->GetActiveWeapon();
	if (weapon) {
		// with logging this time was always one tick behind.
		// so add one tick to the last shoot time.
		shoot_time = weapon->m_fLastShotTime() + g_csgo.m_globals->m_interval;
	}

	if (game::TIME_TO_TICKS(shoot_time) == game::TIME_TO_TICKS(record->m_sim_time)) {
		if (record->m_lag <= 1)
			record->m_shot = true;

		// more then 1 choke, cant hit pitch, apply prev pitch.
		else if (data->m_records.size() >= 2) {
			LagRecord* previous = data->m_records[1].get();

			if (previous && !previous->dormant())
				record->m_eye_angles.x = previous->m_eye_angles.x;
			record->m_resolver_mode = "PREV PITCH";
			return;
		}
		record->m_resolver_mode = "SHOT RECORD";
	}
	/*float delta = game::TIME_TO_TICKS(shoot_time) - game::TIME_TO_TICKS(record->m_sim_time);

	// this record has a shot on it.
	if (delta == 1)
	{
		record->m_shot = true;
	}
	else if (delta == 0)
	{
		record->m_shot = false;

		if (data->m_records.size() >= 2)
		{
			LagRecord* previous = data->m_records[1].get();

			if (previous && !previous->dormant())
				record->m_eye_angles.x = previous->m_eye_angles.x;
		}
	}*/
}

/*void Resolver::AntiFreestand(LagRecord* record) {
	// constants
	constexpr float STEP{ 4.f };
	constexpr float RANGE{ 32.f };

	// best target.
	vec3_t enemypos = record->m_player->GetShootPosition();
	float away = GetAwayAngle(record);

	// construct vector of angles to test.
	std::vector< AdaptiveAngle > angles{ };
	angles.emplace_back(away - 180.f);
	angles.emplace_back(away + 90.f);
	angles.emplace_back(away - 90.f);

	// start the trace at the your shoot pos.
	vec3_t start = g_cl.m_local->GetShootPosition();

	// see if we got any valid result.
	// if this is false the path was not obstructed with anything.
	bool valid{ false };

	// iterate vector of angles.
	for (auto it = angles.begin(); it != angles.end(); ++it) {

		// compute the 'rough' estimation of where our head will be.
		vec3_t end{ enemypos.x + std::cos(math::deg_to_rad(it->m_yaw)) * RANGE,
			enemypos.y + std::sin(math::deg_to_rad(it->m_yaw)) * RANGE,
			enemypos.z };

		// compute the direction.
		vec3_t dir = end - start;
		float len = dir.normalize();

		// should never happen.
		if (len <= 0.f)
			continue;

		// step thru the total distance, 4 units per step.
		for (float i{ 0.f }; i < len; i += STEP) {
			// get the current step position.
			vec3_t point = start + (dir * i);

			// get the contents at this point.
			int contents = g_csgo.m_engine_trace->GetPointContents(point, MASK_SHOT_HULL);

			// contains nothing that can stop a bullet.
			if (!(contents & MASK_SHOT_HULL))
				continue;

			float mult = 1.f;

			// over 50% of the total length, prioritize this shit.
			if (i > (len * 0.5f))
				mult = 1.25f;

			// over 90% of the total length, prioritize this shit.
			if (i > (len * 0.75f))
				mult = 1.25f;

			// over 90% of the total length, prioritize this shit.
			if (i > (len * 0.9f))
				mult = 2.f;

			// append 'penetrated distance'.
			it->m_dist += (STEP * mult);

			// mark that we found anything.
			valid = true;
		}
	}

	if (!valid) {
		return;
	}

	// put the most distance at the front of the container.
	std::sort(angles.begin(), angles.end(),
		[](const AdaptiveAngle& a, const AdaptiveAngle& b) {
			return a.m_dist > b.m_dist;
		});

	// the best angle should be at the front now.
	AdaptiveAngle* best = &angles.front();

	record->m_eye_angles.y = best->m_yaw;
}*/



void Resolver::SetMode(LagRecord* record) {

	float speed = record->m_velocity.length_2d();

	if ((record->m_flags & FL_ONGROUND) && speed > 0.1f && !record->m_fake_walk)
		record->m_mode = Modes::RESOLVE_WALK;

	// if on ground, not moving or fakewalking.
	else if ((record->m_flags & FL_ONGROUND) && (speed < 0.1f || record->m_fake_walk))
		record->m_mode = Modes::RESOLVE_STAND;

	// if not on ground.
	else if (!(record->m_flags & FL_ONGROUND))
		record->m_mode = Modes::RESOLVE_AIR;

	else if (record->m_mode != Modes::RESOLVE_AIR && record->m_mode != Modes::RESOLVE_STAND && record->m_mode != Modes::RESOLVE_WALK)
		record->m_mode = Modes::RESOLVE_UNKNOWM;
}

void Resolver::ResolveAngles(Player* player, LagRecord* record) {
	AimPlayer* data = &g_aimbot.m_players[player->index() - 1];

	// mark this record if it contains a shot.
	MatchShot(data, record);

	// next up mark this record with a resolver mode that will be used.
	SetMode(record);

	if (g_menu.main.misc.mode.get() == 1)
		record->m_eye_angles.x = 90.f;

	// we arrived here we can do the acutal resolve.
	if (record->m_mode == Modes::RESOLVE_WALK)
		ResolveWalk(data, record, player);

	else if (record->m_mode == Modes::RESOLVE_STAND)
		ResolveStand(data, record, player);

	else if (record->m_mode == Modes::RESOLVE_AIR)
		ResolveAir(data, record, player);

	else if (record->m_mode == Modes::RESOLVE_UNKNOWM)
		GetDirectionAngle(player->index(), player, record);


	//data->resolver_mode = XOR("ANTIFREESTAND");  // was walking antifreestand
   // record->m_resolver_mode = "FS";
	  // normalize the eye angles, doesn't really matter but its clean.
	math::NormalizeAngle(record->m_eye_angles.y);
}

void Resolver::ResolveWalk(AimPlayer* data, LagRecord* record, Player* player) {
	// apply lby to eyeangles.
	if (data->m_missed_shots > 4) { /* && data->m_missed_shots < 8*/
		GetDirectionAngle(player->index(), player, record);// Fuck this shit throw that retards head behind the wall // fixed
		record->m_mode = RESOLVE_WALK;
		//data->resolver_mode = XOR("WALKING ANTIFREESTAND");
		record->m_resolver_mode = "WALK FS";
	}
	else
		record->m_eye_angles.y = record->m_body;
	record->m_mode = RESOLVE_WALK;
	//data->resolver_mode = XOR("WALKING");
	record->m_resolver_mode = "WALKING";


	// delay body update.
	data->m_body_update = record->m_anim_time + 0.23f;  // 0.22

	// reset stand and body index.
	data->m_moving_index = 0;
	data->m_stand_index = 0;
	data->m_stand_index2 = 0;
	data->m_body_index = 0;
	data->m_freestanding_index = 0;
	data->m_lby_delta_index = 0;
	data->m_unknown_move = 0;
	data->m_last_move = 0;
	data->m_missed_shots = 0;


	// copy the last record that this player was walking
	// we need it later on because it gives us crucial data.
	std::memcpy(&data->m_walk_record, record, sizeof(LagRecord));
}

bool CheckSequence(Player* player, int sequence, bool checklayers = false, int layerstocheck = 15) //decent check could be improved - stolen from cthulhu (sopmk)
{
	// sanity pls
	if (!player || !player->alive())
		return false;

	for (int i = 0; i < checklayers ? layerstocheck : 15; i++) //Check layers
	{
		auto layer = player->m_AnimOverlay()[i];

		if (player->GetSequenceActivity(layer.m_sequence) == sequence) //Check for sequence 
			return true;
	}

	return false; // well this uh 
}
//This are good for eachother like a married couple 
bool CheckLBY(Player* player, LagRecord* record, LagRecord* prev_record) // I would recommend a sequence check -LoOsE (note from sopmk: sequence checks arent always effective, as some lby breakers suppress 979, hence why i'm checking animation values too)
{
	if (player->m_vecVelocity().length_2d() > 1.1f)
		return false; // cant break here

	bool choking = fabs(player->m_flSimulationTime() - player->m_flOldSimulationTime()) > g_csgo.m_globals->m_interval;

	if (int i = 0; i < 13, i++)
	{
		auto layer = record->m_layers[i];
		auto prev_layer = prev_record->m_layers[i];

		// make sure that the animation happened
		if (layer.m_cycle != prev_layer.m_cycle)
		{
			if (layer.m_cycle > 0.9 || layer.m_weight == 1.f) // triggered layer
			{
				if (i == 3) // adjust layer sanity check. If it is the adjust layer, they are most likely breaking LBY
					return true;

				// lby flick lol!
				if (choking && fabs(math::NormalizedAngle(record->m_body - prev_record->m_body)) > 5.f)
					return true;
			}
			else if (choking) // for improper LBY breakers
			{
				if (player->GetSequenceActivity(layer.m_sequence) == 979)
				{
					if (player->GetSequenceActivity(prev_layer.m_sequence) == 979)
					{
						return true; // we can be pretty sure that they are breaking LBY
					}
				}
			}
			return false;
		}
		return false;
	}
	return false;
}





bool Resolver::Spin_Detection(AimPlayer* data) {

	if (data->m_records.empty())
		return false;

	spin_step = 0;

	size_t size{};

	// iterate records.
	for (const auto& it : data->m_records) {
		if (it->dormant())
			break;

		// increment total amount of data.
		++size;
	}

	if (size > 2) {
		LagRecord* record = data->m_records[0].get();

		spindelta = (record->m_body - data->m_records[1].get()->m_body) / data->m_records[1].get()->m_lag;
		spinbody = record->m_body;
		float delta2 = (data->m_records[1].get()->m_body - data->m_records[2].get()->m_body) / data->m_records[2].get()->m_lag;

		return spindelta == delta2 && spindelta > 0.5f;
	}
	else
		return false;
}

void Resolver::ResolveStand(AimPlayer* data, LagRecord* record, Player* player) {

	data->m_moved = false;

	// for no-spread call a seperate resolver.
	if (g_menu.main.misc.mode.get() == 1) {
		StandNS(data, record);
		return;
	}

	// get predicted away angle for the player.
	float away = GetAwayAngle(record);

	// pointer for easy access.
	LagRecord* move = &data->m_walk_record;

	C_AnimationLayer* curr = &record->m_layers[3];
	int act = data->m_player->GetSequenceActivity(curr->m_sequence);


	// we have a valid moving record.
	///if (move->m_sim_time > 0.f && !move->dormant() && !record->dormant() && data->m_last_move < 1) { // move->m_sim_time > 0.f && !move->dormant() && !record->dormant() && data->m_last_move < 1
	if (move->m_sim_time > 0.f && !move->dormant() && !record->dormant() && data->m_last_move < 1) {
		vec3_t delta = move->m_origin - record->m_origin;

		// check if moving record is close.
		if (delta.length() <= 100.f) {
			// indicate that we are using the moving lby.
			data->m_moved = true;
		}
	}

	/*if (!((int)record->m_layers[12].m_weight * 1000.f) && record->m_player->m_vecVelocity().length_2d() > 0.1 &&
		((int)record->m_layers[6].m_weight * 1000.f) == ((int)record->m_layers[6].m_weight * 1000.f)) {

		// todo: maybe make this actually do something?
		auto m_layer_delta1 = abs(record->m_layers[6].m_playback_rate - record->m_layers[6].m_playback_rate);
		auto m_layer_delta2 = abs(record->m_layers[6].m_playback_rate - record->m_layers[6].m_playback_rate);
		auto m_layer_delta3 = abs(record->m_layers[6].m_playback_rate - record->m_layers[6].m_playback_rate);

		if (m_layer_delta1 < m_layer_delta2
			|| m_layer_delta3 <= m_layer_delta2                                              //Okay this is ghetto since bonesetup is not proper so it will mess shit up maybe or maybe not?
			|| (signed int)(float)(m_layer_delta2 * 1000.0))
		{
			if (m_layer_delta1 >= m_layer_delta3
				&& m_layer_delta2 > m_layer_delta3
				&& !(signed int)(float)(m_layer_delta3 * 1000.0))
			{
				record->m_mode = Modes::RESOLVE_BODY;
			}
		}
	}*/


	bool breaking = CheckLBY(data->m_player, record, FindLastRecord(data));
	// a valid moving context was found
	if (data->m_moved == true) {
		float diff = math::NormalizedAngle(record->m_body - move->m_body);
		float delta = record->m_anim_time - move->m_anim_time;


		if (data->m_last_move < 1) {
			record->m_mode = Modes::RESOLVE_LASTMOVE;
			float diff = math::NormalizedAngle(record->m_body - move->m_body);
			float delta = record->m_anim_time - move->m_anim_time;
			//if (data->m_last_move < 1) {
			record->m_mode = Modes::RESOLVE_LASTMOVE;
			//data->m_last_move

			record->m_resolver_mode = "LAST MOVING LBY";

			//const float at_target_yaw = math::CalcAngle(g_cl.m_local->m_vecOrigin(), player->m_vecOrigin()).y;

			record->m_eye_angles.y = move->m_body;

			record->m_resolver_mode = "LAST MOVING LBY";
			//}
		}



		// it has not been time for this first update yet.
		if (data->m_records.size() >= 2)
		{
			LagRecord* previous = data->m_records[1].get();

			if (previous)
			{
				if (record->m_body != previous->m_body && data->m_body_index < 1)
				{
					record->m_eye_angles.y = record->m_body;
					data->m_body_update = record->m_anim_time + 1.1f;
					iPlayers[record->m_player->index()] = false;
					record->m_mode = Modes::RESOLVE_BODY;
					record->m_resolver_mode = "MOVED LBY FLICK";
				}
			}
		}
	}
	else
		GetDirectionAngle(player->index(), player, record);

}
// LBY SHOULD HAVE UPDATED HERE.
/*else if (record->m_anim_time >= data->m_body_update) {
	// only shoot the LBY flick 3 times.
	// if we happen to miss then we most likely mispredicted.
	if (data->m_records.size() >= 2)
	{
		LagRecord* previous = data->m_records[1].get();

		if (previous)
		{
			if (record->m_body != previous->m_body && data->m_body_index < 1)
			{
				record->m_eye_angles.y = record->m_body;
				data->m_body_update = record->m_anim_time + 1.1f;
				iPlayers[record->m_player->index()] = false;
				record->m_mode = Modes::RESOLVE_BODY;
				record->m_resolver_mode = "MOVED LBY FLICK";
			}
		}*/



		// set to stand1 -> known last move.
	/*	record->m_mode = Modes::RESOLVE_STAND1;

		// ok, no fucking update. apply big resolver.
		record->m_eye_angles.y = move->m_body;

		if (!breaking && !(data->m_stand_index % 4))
			record->m_eye_angles.y = record->m_body;

		// every third shot do some fuckery.
		if (!(data->m_stand_index % 3))
			record->m_eye_angles.y += record->m_body + 90.f;

		// jesus we can fucking stop missing can we?
		if (data->m_stand_index > 6 && act != 980) {
			// lets just hope they switched ang after move.
			record->m_eye_angles.y = move->m_body + 180.f;
		}

		// we missed 4 shots.
		else if (data->m_stand_index > 4 && act != 980) {
			// try backwards.
			record->m_eye_angles.y = away + 180.f;
		}*/

		//return;
//else if (data->m_moved = false || data->m_body_index > 1) {
	//GetDirectionAngle(player->index(), player, record);// Fuck this shit throw that retards head behind the wall // fixed
//}
// 	   
// stand2 -> no known last move.
//record->m_mode = Modes::RESOLVE_STAND2;

/*	if (breaking)
	{
		switch (data->m_stand_index2 % 6) {

		case 0:
			record->m_eye_angles.y = move->m_body;
			break;

		case 1:
			record->m_eye_angles.y = record->m_body + 115.f;
			break;

		case 2:
			record->m_eye_angles.y = record->m_body - 115.f;
			break;

		case 3:
			record->m_eye_angles.y = record->m_body + 180.f;
			break;

		case 4:
			record->m_eye_angles.y = record->m_body;
			break;

		case 5:
			record->m_eye_angles.y = away + 180.f;
			break;

		default:
			break;
		}
	}*/
	/*else // we should be able to hit LBY, since they arent breaking it
	{
		record->m_eye_angles.y = data->m_player->m_flLowerBodyYawTarget();
		record->m_mode = Modes::RESOLVE_BODY;
	}*/


void Resolver::ResolveAir(AimPlayer* data, LagRecord* record, Player* player) {
	// for no-spread call a seperate resolver.
	if (g_menu.main.misc.mode.get() == 1) {
		AirNS(data, record);
		return;
	}

	// else run our matchmaking air resolver.
	if (record->m_velocity.length_2d() < 60.f) {
		//record->m_mode = Modes::RESOLVE_STAND;

	//	GetDirectionAngle(player->index(), player, record);
		// invoke our stand resolver.
		ResolveStand(data, record, player);

		return;
	}

	// try to predict the direction of the player based on his velocity direction.
	// this should be a rough estimation of where he is looking.
	float velyaw = math::rad_to_deg(std::atan2(record->m_velocity.y, record->m_velocity.x));

	GetDirectionAngle(player->index(), player, record);
	/*switch (data->m_missed_shots % 3) {
	case 0:
		record->m_eye_angles.y = velyaw + 180.f;
		break;

	case 1:
		record->m_eye_angles.y = velyaw - 90.f;
		break;

	case 2:
		record->m_eye_angles.y = velyaw + 90.f;
		break;
	}*/

	data->resolver_mode = XOR("AIR");
	record->m_resolver_mode = "AIR";
}

void Resolver::StandNS(AimPlayer* data, LagRecord* record) {
	// get away angles.
	float away = GetAwayAngle(record);

	switch (data->m_shots % 8) {
	case 0:
		record->m_eye_angles.y = away + 180.f; // + 180.f      //previous resolver angles vvvvvv (dont change them unless you make a better resolver, or add another //)
		break;

	case 1:
		record->m_eye_angles.y = away + 90.f; // + 90.f
		break;

	case 2:
		record->m_eye_angles.y = away - 90.f; // - 90.f
		break;

	case 3:
		record->m_eye_angles.y = away + 45.f; // + 45.f
		break;
	case 4:
		record->m_eye_angles.y = away - 45.f; // - 45.f
		break;

	case 5:
		record->m_eye_angles.y = away + 135.f; // + 135.f
		break;

	case 6:
		record->m_eye_angles.y = away - 135.f; // - 135.f
		break;

	case 7:
		record->m_eye_angles.y = away + 0.f; // + 0.f
		break;

	default:
		break;
	}

	// force LBY to not fuck any pose and do a true bruteforce.
	record->m_body = record->m_eye_angles.y;
}

float Resolver::GetDirectionAngle(int index, Player* player, LagRecord* record) {
	const auto left_thickness = g_cl.m_left_thickness[index];
	const auto right_thickness = g_cl.m_right_thickness[index];
	const auto at_target_yaw = math::CalcAngle(g_cl.m_local->m_vecOrigin(), player->m_vecOrigin()).y;// was g_cl.m_at_target_angle[index];
	const auto at_target_angle = g_cl.m_at_target_angle[index];
	AimPlayer* data;
	data = &g_aimbot.m_players[index - 1];
	auto yessir = player->m_flLowerBodyYawTarget();
	auto sds = player->m_AnimOverlay()[3].m_prev_cycle;
	record->m_mode = Modes::RESOLVE_UNKNOWM;

	//player->m_AnimOverlay()[6].m_weight == 3

	auto angle = 0.f;
	/*	if (data->m_records.size() >= 2 && data->m_last_move < 1 && data->m_unknown_move < 1)
		{
			record->m_mode = Modes::RESOLVE_UNKNOWM;

			LagRecord* previous = data->m_records[1].get();
		/*	if (previous->m_body && previous->m_sim_time != record->m_body && record->m_sim_time) {
				record->m_eye_angles.y = yessir;
				record->m_resolver_mode = "test1";

			}*//*
			if (sds++)
			{
				auto me = player->m_AnimOverlay()[12].m_cycle;
				record->m_resolver_mode = "test2";
				record->m_eye_angles.y = me;
				return me;
			}


		}*/





	if ((left_thickness >= 350 && right_thickness >= 350) || (left_thickness <= 50 && right_thickness <= 50) || (std::fabs(left_thickness - right_thickness) <= 7)) { // was 7
		record->m_mode = Modes::RESOLVE_UNKNOWM;
		switch (data->m_unknown_move % 7) {
		case 0:
			angle = math::normalize_float(at_target_yaw);  // -180  // at_target_yaw + 0
			record->m_eye_angles.y = angle;
			record->m_resolver_mode = "FS 1-1";
			break;
		case 1:
			angle = math::normalize_float(at_target_yaw + 180.f);  // + 0.f  // at_target_yaw - 180
			record->m_eye_angles.y = angle;
			record->m_resolver_mode = "FS 1-2";
			break;
		case 2:
			angle = math::normalize_float(at_target_yaw + 10.f); // + 170.f
			record->m_eye_angles.y = angle;
			record->m_resolver_mode = "FS 1-3";
			break;
		case 3:
			angle = math::normalize_float(at_target_yaw + 20.f); // + 20.f
			record->m_eye_angles.y = angle;
			record->m_resolver_mode = "FS 1-4";
			break;
		case 4:
			angle = math::normalize_float(at_target_yaw - 170.f); // - 170.f
			record->m_eye_angles.y = angle;
			record->m_resolver_mode = "FS 1-5";
			break;
		case 5:
			angle = math::normalize_float(at_target_yaw - 20.f); // - 20.f
			record->m_eye_angles.y = angle;
			record->m_resolver_mode = "FS 1-6";
			break;
		case 6:
			angle = math::normalize_float(at_target_yaw + 180.f); // + 180.f
			record->m_eye_angles.y = angle;
			record->m_resolver_mode = "FS 1-7";
			break;
		default:
			break;

		}
	}
	else {
		if (left_thickness > right_thickness) {
			record->m_mode = Modes::RESOLVE_UNKNOWM;
			switch (data->m_unknown_move % 2) {
			case 0:
				angle = math::normalize_float(at_target_angle - 90.f); // - 90.f
				record->m_eye_angles.y = angle;
				record->m_resolver_mode = "FS 2-1";
				break;
			case 1:
				angle = math::normalize_float(at_target_angle - 70.f); // -70.f
				record->m_eye_angles.y = angle;
				record->m_resolver_mode = "FS 2-2";
				break;
			default:
				break;
			}
		}
		else if (left_thickness == right_thickness) {
			record->m_mode = Modes::RESOLVE_UNKNOWM;
			switch (data->m_unknown_move % 2) {
			case 0:
				angle = math::normalize_float(at_target_angle + 180.f);  // was -180.f (+ 180.f)
				record->m_eye_angles.y = angle;
				record->m_resolver_mode = "FS 3-1";
				break;
			case 1:
				angle = math::normalize_float(at_target_angle + 175.f); // + 175.f
				record->m_eye_angles.y = angle;
				record->m_resolver_mode = "FS 3-2";
				break;
			default:
				break;
			}
		}
		else {
			record->m_mode = Modes::RESOLVE_UNKNOWM;
			switch (data->m_unknown_move % 2) {
			case 0:
				angle = math::normalize_float(at_target_angle + 90.f);
				record->m_eye_angles.y = angle;
				record->m_resolver_mode = "FS 4-1";
				break;
			case 1:
				angle = math::normalize_float(at_target_angle + 70.f);
				record->m_eye_angles.y = angle;
				record->m_resolver_mode = "FS 4-2";
				break;
			default:
				break;

			}
		}
	}
	return angle;

}

void Resolver::AirNS(AimPlayer* data, LagRecord* record) {
	// get away angles.
	float away = GetAwayAngle(record);

	switch (data->m_missed_shots % 9) {
	case 0:
		record->m_eye_angles.y = away + 180.f;
		break;

	case 1:
		record->m_eye_angles.y = away + 150.f;
		break;
	case 2:
		record->m_eye_angles.y = away - 150.f;
		break;

	case 3:
		record->m_eye_angles.y = away + 165.f;
		break;

	case 4:
		record->m_eye_angles.y = away - 165.f;
		break;

	case 5:
		record->m_eye_angles.y = away + 135.f;
		break;

	case 6:
		record->m_eye_angles.y = away - 135.f;
		break;

	case 7:
		record->m_eye_angles.y = away + 90.f;
		break;

	case 8:
		record->m_eye_angles.y = away - 90.f;
		break;

	default:
		break;
	}
}

void Resolver::ResolvePoses(Player* player, LagRecord* record) {
	AimPlayer* data = &g_aimbot.m_players[player->index() - 1];

	// only do this bs when in air.
	if (record->m_mode == Modes::RESOLVE_AIR) {
		// ang = pose min + pose val x ( pose range )

		// lean_yaw
		player->m_flPoseParameter()[2] = g_csgo.RandomInt(0, 4) * 0.25f;

		// body_yaw
		player->m_flPoseParameter()[11] = g_csgo.RandomInt(1, 3) * 0.25f;
	}
}