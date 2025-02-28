#pragma once

#include "Base.h"

/* 1. 원형객체들을 보관한다. */
/* 2. 원형객체를 찾아서 복제하여 사본객체를 생성한다. */
/* 3. 위에서 생성한 사본객체를 보관한다. */
/* 4. 보유하고있는 레이어들의 Tick들을 호출한다. */
/* 5. 보유하고있는 레이어들의 Render (x : 그리는 순서를 관리해야 할 필요가 있어서. ) */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	void Clear(_uint iLevelIndex);
public:
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	class CComponent* Find_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);

private:
	map<const wstring, class CGameObject*>			m_Prototypes;
private:
	_uint											m_iNumLevels = { 0 };
	map<const wstring, class CLayer*>*				m_pLayers = { nullptr };
	typedef map<const wstring, class CLayer*>		LAYERS;

private:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);


public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END