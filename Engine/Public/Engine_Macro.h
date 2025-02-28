#pragma once

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))


#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define BEGIN(NAMESPACE) namespace NAMESPACE { 
#define END }

#define MAX_CHANNEL 128

#define MSG_BOX(message)								\
::MessageBox(nullptr, message, L"System Message", MB_OK)

#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static ClassName* Get_Instance();					\
	static void		  Destroy_Instance();				\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
void ClassName::Destroy_Instance()						\
{														\
	if(nullptr != m_pInstance)							\
	{													\
		delete m_pInstance;								\
		m_pInstance = nullptr;							\
	}													\
}




// 선언과 동시에 초기화 불가능, 먼저 선언된 전역변수와 자료형, 변수명까지 일치하는 상태에서만 사용 가능