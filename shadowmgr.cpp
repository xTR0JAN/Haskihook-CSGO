#include "includes.h"

void Hooks::ComputeShadowDepthTextures( const CViewSetup &view, bool unk ) {
	if( !unk )
		return g_hooks.m_shadow_mgr.GetOldMethod< ComputeShadowDepthTextures_t >( IClientShadowMgr::COMPUTESHADOWDEPTHTEXTURES )( this, view, unk );

	g_hooks.m_shadow_mgr.GetOldMethod< ComputeShadowDepthTextures_t >( IClientShadowMgr::COMPUTESHADOWDEPTHTEXTURES )( this, view, unk );
}