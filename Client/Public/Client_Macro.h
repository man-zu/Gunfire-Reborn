#pragma once

#define CAMERA_FAR				3000.f;

// ATTACK DAMAGE
#define BOSS_JUMPSTONE_DAMAGE	90.f;
#define BOSS_THROWSTONE_DAMAGE	30.f;
#define BOSS_SHOT_DAMAGE		20.f;
#define BOSS_PUNCH_DAMAGE		100.f;
#define MONSTER_ARROW_DAMAGE	30.f;
#define PLAYER_WEAPON_BASIC		10.f;
#define PLAYER_WEAPON_BLUEEYE	25.f;


#define GET_LIST(Level_Index,LayerTag)			m_pGameInstance->Get_List(m_pGameInstance->Find_Layer(Level_Index, LayerTag))
#define GET_OBJ(Level_Index, LayerTag, iIndex)	m_pGameInstance->Find_GameObject(Level_Index, LayerTag, iIndex)
#define GET_CURLEVEL			(LEVEL)m_pGameInstance->Get_CurLevel()->Get_LevelID()

#define KEY_NONE(KEY)		CInput_Device::NONE == m_pGameInstance->Get_DIKeyState(KEY)
#define KEY_PUSH(KEY)		CInput_Device::PUSH == m_pGameInstance->Get_DIKeyState(KEY)
#define KEY_HOLD(KEY)		CInput_Device::HOLD == m_pGameInstance->Get_DIKeyState(KEY)
#define KEY_AWAY(KEY)		CInput_Device::AWAY == m_pGameInstance->Get_DIKeyState(KEY)

#define MOUSE_NONE(BOTTON)	CInput_Device::NONE == m_pGameInstance->Get_DIMKeyState(BOTTON)
#define MOUSE_PUSH(BOTTON)	CInput_Device::PUSH == m_pGameInstance->Get_DIMKeyState(BOTTON)
#define MOUSE_HOLD(BOTTON)	CInput_Device::HOLD == m_pGameInstance->Get_DIMKeyState(BOTTON)
#define MOUSE_AWAY(BOTTON)	CInput_Device::AWAY == m_pGameInstance->Get_DIMKeyState(BOTTON)

#define MOUSE_MOVE_X(Mouse) Mouse = m_pGameInstance->Get_DIMouseMove(DIMM_X)
#define MOUSE_MOVE_Y(Mouse) Mouse = m_pGameInstance->Get_DIMouseMove(DIMM_Y)

#define GET_PIPELINE(eState)	*(m_pGameInstance->Get_Transform_Float4x4(eState))


#define ADD_SOUND(SoundTag, FilePath) m_pGameInstance->Add_Sound(SoundTag,FilePath)
#define PLAY(SoundTag,Loop) m_pGameInstance->Play(SoundTag,Loop)
#define PLAYONLY(SoundTag) m_pGameInstance->PlayOnly(SoundTag)
#define PLAYTOBGM(SoundTag,Loop) m_pGameInstance->Play(SoundTag,Loop)
#define STOP(SoundTag)	m_pGameInstance->Stop(SoundTag)
#define VOLUME(SoundTag, Volume) m_pGameInstance->SetVolume(SoundTag,Volume)


