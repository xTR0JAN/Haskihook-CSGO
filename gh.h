#pragma once

#include "includes.h"

class c_grenade_prediction {
public:
	struct data_t {
		__forceinline data_t() = default;

		__forceinline data_t(Player* owner, int index, const vec3_t& origin, const vec3_t& velocity, float throw_time, int offset) : data_t() {
			m_owner = owner;
			m_index = index;

			predict(origin, velocity, throw_time, offset);
		}

		__forceinline bool physics_simulate() {
			if (m_detonated)
				return true;

			static const auto sv_gravity = g_csgo.m_cvar->FindVar(HASH("sv_gravity"));

			const auto new_velocity_z = m_velocity.z - (sv_gravity->GetFloat() * 0.4f) * g_csgo.m_globals->m_interval;

			const auto move = vec3_t(
				m_velocity.x * g_csgo.m_globals->m_interval,
				m_velocity.y * g_csgo.m_globals->m_interval,
				(m_velocity.z + new_velocity_z) / 2.f * g_csgo.m_globals->m_interval
			);

			m_velocity.z = new_velocity_z;

			auto trace = CGameTrace();

			physics_push_entity(move, trace);

			if (m_detonated)
				return true;

			if (trace.m_fraction != 1.f) {
				update_path< true >();

				perform_fly_collision_resolution(trace);
			}

			return false;
		}

		__forceinline void physics_trace_entity(const vec3_t& src, const vec3_t& dst, std::uint32_t mask, CGameTrace& trace) {
			g_csgo.m_engine_trace->TraceHull(
				src, dst, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f },
				mask, m_owner, m_collision_group, &trace
			);

			if (trace.m_startsolid
				&& (trace.m_contents & CONTENTS_CURRENT_90)) {
				trace.clear();

				g_csgo.m_engine_trace->TraceHull(
					src, dst, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f },
					mask & ~CONTENTS_CURRENT_90, m_owner, m_collision_group, &trace
				);
			}

			if (!trace.hit()
				|| !trace.m_entity
				|| !reinterpret_cast<Entity*>(trace.m_entity)->IsPlayer())
				return;

			trace.clear();

			g_csgo.m_engine_trace->TraceLine(src, dst, mask, m_owner, m_collision_group, &trace);
		}

		__forceinline void physics_push_entity(const vec3_t& push, CGameTrace& trace) {
			physics_trace_entity(m_origin, m_origin + push,
				m_collision_group == COLLISION_GROUP_DEBRIS
				? (MASK_SOLID | CONTENTS_CURRENT_90) & ~CONTENTS_MONSTER
				: MASK_SOLID | CONTENTS_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE | CONTENTS_CURRENT_90 | CONTENTS_HITBOX,
				trace
			);

			if (trace.m_startsolid) {
				m_collision_group = COLLISION_GROUP_INTERACTIVE_DEBRIS;

				g_csgo.m_engine_trace->TraceLine(
					m_origin - push, m_origin + push,
					(MASK_SOLID | CONTENTS_CURRENT_90) & ~CONTENTS_MONSTER,
					m_owner, m_collision_group, &trace
				);
			}

			if (trace.m_fraction) {
				m_origin = trace.m_endpos;
			}

			if (!trace.m_entity)
				return;

			if (reinterpret_cast<Entity*>(trace.m_entity)->IsPlayer()
				|| m_index != 68 && m_index != MOLOTOV && m_index != FIREBOMB) // TAGRENADE = 68
				return;

			static const auto weapon_molotov_maxdetonateslope = g_csgo.m_cvar->FindVar(HASH("weapon_molotov_maxdetonateslope"));

			if (m_index != 68
				&& trace.m_plane.m_normal.z < std::cos(math::deg_to_rad(weapon_molotov_maxdetonateslope->GetFloat()))) // TAGRENADE = 68
				return;

			detonate< true >();
		}

		__forceinline void perform_fly_collision_resolution(CGameTrace& trace) {
			auto surface_elasticity = 1.f;

			if (trace.m_entity) {
#if 0 /* ayo reis wtf */
				if (const auto v8 = trace.m_surface.m_name) {
					if (*(DWORD*)v8 != 'spam' || *((DWORD*)v8 + 1) != '_sc/') {
						if (*((DWORD*)v8 + 1) == '_ed/'
							&& *((DWORD*)v8 + 2) == 'ekal'
							&& *((DWORD*)v8 + 3) == 'alg/'
							&& *((DWORD*)v8 + 4) == 'g/ss'
							&& *((DWORD*)v8 + 5) == 'ssal') {
							goto BREAKTROUGH;
						}
					}
					else if (*((DWORD*)v8 + 2) == 'iffo'
						&& *((DWORD*)v8 + 3) == 'g/ec'
						&& *((DWORD*)v8 + 4) == 'ssal'
						&& *((DWORD*)v8 + 5) == 'bru/'
						&& *((DWORD*)v8 + 6) == 'g_na'
						&& *((DWORD*)v8 + 7) == 'ssal') {
						goto BREAKTROUGH;
					}
				}
#endif
				if (game::IsBreakable(trace.m_entity)) {
				BREAKTROUGH:
					m_velocity *= 0.4f;

					return;
				}

				const auto is_player = reinterpret_cast<Entity*>(trace.m_entity)->IsPlayer();
				if (is_player) {
					surface_elasticity = 0.3f;
				}

				if (trace.m_entity->index()) {
					if (is_player
						&& m_last_hit_entity == trace.m_entity) {
						m_collision_group = COLLISION_GROUP_DEBRIS;

						return;
					}

					m_last_hit_entity = trace.m_entity;
				}
			}

			auto velocity = vec3_t();

			const auto back_off = m_velocity.dot(trace.m_plane.m_normal) * 2.f;

			for (auto i = 0u; i < 3u; i++) {
				const auto change = trace.m_plane.m_normal[i] * back_off;

				velocity[i] = m_velocity[i] - change;

				if (std::fabs(velocity[i]) >= 1.f)
					continue;

				velocity[i] = 0.f;
			}

			velocity *= std::clamp< float >(surface_elasticity * 0.45f, 0.f, 0.9f);

			if (trace.m_plane.m_normal.z > 0.7f) {
				const auto speed_sqr = velocity.length_sqr();
				if (speed_sqr > 96000.f) {
					const auto l = velocity.normalized().dot(trace.m_plane.m_normal);
					if (l > 0.5f) {
						velocity *= 1.f - l + 0.5f;
					}
				}

				if (speed_sqr < 400.f) {
					m_velocity = vec3_t(0, 0, 0);
				}
				else {
					m_velocity = velocity;

					physics_push_entity(velocity * ((1.f - trace.m_fraction) * g_csgo.m_globals->m_interval), trace);
				}
			}
			else {
				m_velocity = velocity;

				physics_push_entity(velocity * ((1.f - trace.m_fraction) * g_csgo.m_globals->m_interval), trace);
			}

			if (m_bounces_count > 20)
				return detonate< false >();

			++m_bounces_count;
		}

		__forceinline void think() {
			switch (m_index) {
			case SMOKE:
				if (m_velocity.length_sqr() <= 0.01f) {
					detonate< false >();
				}

				break;
			case DECOY:
				if (m_velocity.length_sqr() <= 0.04f) {
					detonate< false >();
				}

				break;
			case FLASHBANG:
			case HEGRENADE:
			case MOLOTOV:
			case FIREBOMB:
				if (game::TICKS_TO_TIME(m_tick) > m_detonate_time) {
					detonate< false >();
				}

				break;
			}

			m_next_think_tick = m_tick + game::TIME_TO_TICKS(0.2f);
		}

		template < bool _bounced >
		__forceinline void detonate() {
			m_detonated = true;

			update_path< _bounced >();
		}

		template < bool _bounced >
		__forceinline void update_path() {
			m_last_update_tick = m_tick;

			m_path.emplace_back(m_origin, _bounced);
		}

		__forceinline void predict(const vec3_t& origin, const vec3_t& velocity, float throw_time, int offset) {
			m_origin = origin;
			m_velocity = velocity;
			m_collision_group = COLLISION_GROUP_PROJECTILE;

			const auto tick = game::TIME_TO_TICKS(1.f / 30.f);

			m_last_update_tick = -tick;

			switch (m_index) {
			case SMOKE: m_next_think_tick = game::TIME_TO_TICKS(1.5f); break;
			case DECOY: m_next_think_tick = game::TIME_TO_TICKS(2.f); break;
			case FLASHBANG:
			case HEGRENADE:
				m_detonate_time = 1.5f;
				m_next_think_tick = game::TIME_TO_TICKS(0.02f);

				break;
			case MOLOTOV:
			case FIREBOMB:
				static const auto molotov_throw_detonate_time = g_csgo.m_cvar->FindVar(HASH("molotov_throw_detonate_time"));

				m_detonate_time = molotov_throw_detonate_time->GetFloat();
				m_next_think_tick = game::TIME_TO_TICKS(0.02f);

				break;
			}

			for (; m_tick < game::TIME_TO_TICKS(60.f); ++m_tick) {
				if (m_next_think_tick <= m_tick) {
					think();
				}

				if (m_tick < offset)
					continue;

				if (physics_simulate())
					break;

				if (m_last_update_tick + tick > m_tick)
					continue;

				update_path< false >();
			}

			if (m_last_update_tick + tick <= m_tick) {
				update_path< false >();
			}

			m_expire_time = throw_time + game::TICKS_TO_TIME(m_tick);
		}

		bool draw() const;

		bool											m_detonated{};
		Player* m_owner{};
		vec3_t											m_origin{}, m_velocity{};
		Entity* m_last_hit_entity{};
		Collision_Group_t								m_collision_group{};
		float											m_detonate_time{}, m_expire_time{};
		int												m_index{}, m_tick{}, m_next_think_tick{},
			m_last_update_tick{}, m_bounces_count{};
		std::vector< std::pair< vec3_t, bool > >		m_path{};
	} m_data{};

	std::unordered_map< unsigned long, data_t > m_list{};
public:
	__forceinline c_grenade_prediction() = default;

	void on_create_move(CUserCmd* cmd);

	__forceinline const data_t& get_local_data() const { return m_data; }

	__forceinline std::unordered_map< unsigned long, data_t >& get_list() { return m_list; }

	virtual void grenade_warning(Player* entity);
};

inline c_grenade_prediction g_grenades_pred;