#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CNavigation;
class CVIBuffer_Terrain;
END


BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	using TERRAIN_DESC = struct TagTerrainDesc
	{
		LEVEL eLevel;
	};

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

	_bool m_bShowNavi = { false };
	_bool m_bShowTexture = { false };
	LEVEL m_eLevel = {LEVEL_END};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	void GetPickPos(_float3* vPickPos);
	CNavigation* GetNaviUsingTool() { return m_pNavigationCom; }

	void NaviOnOff(_bool _b) { m_bShowNavi = _b; }

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END