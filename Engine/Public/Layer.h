#pragma once

#include "Base.h"

/* 사본객체들을 모아서 저장한다. */
/* 4. 보유하고있는 사본객체들의 Tick들을 호출한다. */
/* 5. 보유하고있는 사본객체들의 Render (x : 그리는 순서를 관리해야 할 필요가 있어서. ) */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	class CComponent* Find_Component(const wstring& strComponentTag, _uint iIndex);
	class CGameObject* Find_GameObject(_uint iIndex);
	list <class CGameObject*>* Get_List() { return &m_GameObjects; }

private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END