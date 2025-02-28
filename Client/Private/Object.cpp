#include "Object.h"

CObject::CObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CObject::CObject(const CObject& rhs)
	: CGameObject(rhs)
{
}

void CObject::Free()
{
	__super::Free();
}
