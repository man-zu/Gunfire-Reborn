#pragma once

#include "../Default/stdafx.h"

#include <process.h>
#include <commdlg.h>

#include "imgui/imgui.h"
#include "imgui/imconfig.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/ImGuizmo/ImGuizmo.h"
#include "Imgui/ImGuizmo/GraphEditor.h"
#include "Imgui/ImGuizmo/ImSequencer.h"
#include "Imgui/ImGuizmo/ImZoomSlider.h"
#include "Imgui/ImGuizmo/ImGradient.h"
#include "Imgui/ImGuizmo/ImCurveEdit.h"

/* 클라이언트에서 제작하는 모든 클래스들이 공통적으로 자주 사용하는 정의들을 모아둔다. */
namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_TOOL, LEVEL_STAGE1, LEVEL_STAGE2, LEVEL_STAGE1_BOSS, LEVEL_END };
	enum WEAPONTYPE { SKILL, BASIC, BLUEEYE, SR, PISTOL, WEAPON_END };
	enum MONSTERSTATE { MONSTER_MOVE_PLAYER, MONSTER_MOVE_INIT, MONSTER_MOVE_RUN, MONSTER_IDLE, MONSTER_ATTACK, MONSTER_HIT, MONSTER_DIE, MONSTER_END };
	enum BOSS_PTRN { BOSS_INTRO_WAIT, BOSS_INTRO, BOSS_IDLE, BOSS_MOVE, BOSS_ATTACK1, BOSS_ATTACK2, BOSS_ATTACK3, BOSS_ATTACK4, BOSS_DAMAGED, BOSS_HEALING, BOSS_DIE, BOSS_END };
	enum BOOM_PTRN { BOOM_IDLE, BOOM_INTRO, BOOM_MOVE, BOOM_ATTACK, BOOM, BOOM_DIE, BOOM_END };
}

using GIZMODESC = struct tagGizmoDesc
{
	ImGuizmo::MODE CurrentGizmoMode = ImGuizmo::WORLD;
	ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	bool bUseSnap = false;
	bool boundSizing = false;
	bool boundSizingSnap = false;
	float snap[3] = { 0.1f, 0.1f, 0.1f };
	float bounds[6] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	float boundsSnap[3] = { 0.1f, 0.1f, 0.1f };
};



extern HWND				g_hWnd;
extern HDC				g_HDC;
extern HINSTANCE		g_hInst;
extern bool				g_bCoinAuto;
extern int				g_bCoin;



#include "Client_Macro.h"

using namespace Client;