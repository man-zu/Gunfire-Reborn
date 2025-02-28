#include "UI.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
{
}

void CUI::Free()
{
	__super::Free();
}
