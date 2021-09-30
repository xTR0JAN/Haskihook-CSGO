#pragma once

class InputPrediction {
public:
	float m_curtime;
	float m_frametime;

	struct PredictionData_t {
		int m_flUnpredictedFlags;
		vec3_t m_vecUnpredictedVelocity;
		vec3_t m_vecUnpredictedOrigin;
	} PredictionData;

	struct Variables_t {
		float m_flFrametime;
		float m_flCurtime;
		float m_flVelocityModifier;
		int   m_fTickcount;
		float m_flForwardMove;
		float m_flSideMove;

		vec3_t m_vecVelocity;
		vec3_t m_vecOrigin;
		int m_fFlags;
	} m_stored_variables;

	struct viewmodel_t {
		float m_flViewmodelCycle;
		float m_flViewmodelAnimTime;
	} StoredViewmodel;

public:
	void update( );
	void run( );
	void restore( );
};

extern InputPrediction g_inputpred;