#include "Monster.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& rhs)
    : CGameObject(rhs)
{
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

CGameObject* CMonster::Clone(void* pArg)
{
    return nullptr;
}

void CMonster::Free()
{
    __super::Free();
}
