#include "includes.h"

#define dont

TickbaseSystem g_tickbase;

void TickbaseSystem::PreMovement() {

	// invalidate next shift amount and the ticks to shift prior to shifting.
	m_shift_data.m_next_shift_amount = m_shift_data.m_ticks_to_shift = 0;
}

void TickbaseSystem::PostMovement() {
	// perform sanity checks to make sure we're able to shift.
	if (!g_cl.m_processing) {
		m_shift_data.m_prepare_recharge = true;
		return;
	}

	if (!g_cl.m_cmd || !g_cl.m_weapon) {
		m_shift_data.m_prepare_recharge = true;
		return;
	}

	if (g_cl.m_weapon_id == REVOLVER ||
		g_cl.m_weapon_id == C4 ||
		g_cl.m_weapon_type == WEAPONTYPE_GRENADE)
	{
		m_shift_data.m_prepare_recharge = true;
		m_shift_data.m_did_shift_before = false;
		m_shift_data.m_should_be_ready = false;
		return;
	}

	// don't attempt to shift if we're supposed to.
	if (!m_shift_data.m_should_attempt_shift) {
		m_shift_data.m_prepare_recharge = true;
		m_shift_data.m_did_shift_before = false;
		m_shift_data.m_should_be_ready = false;
		return;
	}

	// setup variables we will later use in order to dermine if we're able to shift tickbase or not.
	float flNonShiftedTime = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase() - (g_cl.m_max_lag + 2));

	// determine if we are able to shoot right now (at the time of the shift).
	bool bCanShootNormally = g_cl.CanFireWeapon(game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()));

	// determine if we are able to shoot in the previous iShiftAmount ticks (iShiftAmount ticks before we shifted).
	bool bCanShootIn12Ticks = g_cl.CanFireWeapon(flNonShiftedTime);

	bool bIsShooting = g_cl.IsFiring(game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()));

	// determine if we are able to shift the tickbase respective to previously setup variables (rofl).
	m_shift_data.m_can_shift_tickbase = bCanShootIn12Ticks || !bCanShootNormally && (m_shift_data.m_did_shift_before);

	// if we can shift tickbase, shift enough ticks in order to double-tap.
	// always prioritise fake-duck if we wish to.
	if (m_shift_data.m_can_shift_tickbase /*&& !g_hvh.m_fake_duck*/) {
		// Tell the cheat to shift tick-base and disable fakelag
		m_shift_data.m_next_shift_amount = g_cl.m_max_lag + 2;
	}
	else {
		m_shift_data.m_next_shift_amount = 0;
		m_shift_data.m_should_be_ready = false;
	}

	// we want to recharge after stopping fake duck.
	/*if( g_hvh.m_fake_duck ) {
		m_shift_data.m_prepare_recharge = true;

		m_shift_data.m_next_shift_amount = 0;
		m_shift_data.m_should_be_ready = false;

		return;
	}*/

	// are we even supposed to shift tickbase?
	if (m_shift_data.m_next_shift_amount > 0) {
		// determine if we're able to double-tap.
		if (bCanShootIn12Ticks) {
			if (m_shift_data.m_prepare_recharge && !bIsShooting) {
				m_shift_data.m_needs_recharge = g_cl.m_max_lag + 2;
				m_shift_data.m_prepare_recharge = false;
			}
			else {
				if (bIsShooting) {
					// store shifted command.
					m_prediction.m_shifted_command = g_cl.m_cmd->m_command_number;

					// store shifted ticks.
					m_prediction.m_shifted_ticks = abs(m_shift_data.m_current_shift);

					// store original tickbase.
					m_prediction.m_original_tickbase = g_cl.m_local->m_nTickBase();

					// update our wish ticks to shift, and later shift tickbase.
					m_shift_data.m_ticks_to_shift = m_shift_data.m_next_shift_amount;
				}
			}
		}
		else {
			m_shift_data.m_prepare_recharge = true;
			m_shift_data.m_should_be_ready = false;
		}
	}
	else {
		m_shift_data.m_prepare_recharge = true;
		m_shift_data.m_should_be_ready = false;
	}

	// note DidShiftBefore state.
	m_shift_data.m_did_shift_before = m_shift_data.m_next_shift_amount > 0;
}