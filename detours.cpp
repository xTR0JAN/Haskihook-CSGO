#include "includes.h"

Detours g_detours{ };

void Detours::init() {
	// install detours.
	oReportHit = (decltype(&ReportHit))DetourFunction(reinterpret_cast<byte*>(g_csgo.ReportHit), reinterpret_cast<byte*>(ReportHit));

	//oSetupBones = reinterpret_cast<Hooks::SetupBones_t>((decltype(&SetupBones))DetourFunction(reinterpret_cast<byte*>((PBYTE)g_csgo.SetupBonesPointer[13]), reinterpret_cast<byte*>(SetupBones)));
}