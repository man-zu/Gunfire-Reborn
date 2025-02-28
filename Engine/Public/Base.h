#pragma once

#include "Engine_Defines.h"

/* 1. 앞으로 내가 정의하는 모든 클래스의 부모가 되는 클래스다.(SuperClass) */
/* 2. 레퍼런스카운트를 관리한다. */
/* 레퍼런스카운트 : 참조갯수 */

/* abstract 추상클래스 : 독립(혼자서)적으로 객체화 될 수 없는 클래스. 부모! */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;
public:
	unsigned int AddRef();

	/* 참조개수를 감소시킨다 or 삭제한다. */
	unsigned int Release();

private:
	unsigned int			m_iRefCnt = { 0 };

public:
	virtual void Free();
};

END