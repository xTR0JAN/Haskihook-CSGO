#pragma once

class Movement {
public:
	ang_t m_qRealAngles = {};
	float  m_speed;
	float  m_ideal;
	float  m_ideal2;
	vec3_t m_mins;
	vec3_t m_maxs;
	vec3_t m_origin;
	float  m_switch_value = 1.f;
	int    m_strafe_index;
	float  m_old_yaw;
	float  m_circle_yaw;
	bool   m_invert;
	bool   m_edge_jump;
	bool   m_slow_motion;
	vec3_t old_movement = {};
	ang_t m_qAngles = {};
	ang_t m_qAnglesView = {};

	struct {
		bool m_autopeeking;
		bool m_should_retrack;
		vec3_t m_old_origin;
	} m_autopeek_data;

public:
	void JumpRelated();
	void Fast_stop();
	void Stop(float speed);
	void Strafe();
	void DoPrespeed();
	bool GetClosestPlane(vec3_t& plane);
	bool WillCollide(float time, float step);
	void MoonWalk(CUserCmd* cmd);
	void FixMove(CUserCmd* cmd, const ang_t& old_angles);
	void AutoPeek();
	void AutoStopFIX();
	void EdgeJump();
	void QuickStop();
	void ClampMovementSpeed(float speed);
	void FakeWalk();
};

extern Movement g_movement;