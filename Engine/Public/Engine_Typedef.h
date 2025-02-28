#pragma once

namespace Engine
{	
	typedef unsigned char			_ubyte;
	typedef signed char				_byte;
	typedef char					_char;
	typedef wchar_t					_tchar;
	typedef wstring					_wstring;

	typedef bool					_bool;

	typedef unsigned short			_ushort;
	typedef signed short			_short;

	typedef unsigned int			_uint;
	typedef signed int				_int;

	typedef unsigned long			_ulong;
	typedef signed long				_long;

	typedef float					_float;
	typedef double					_double;

	/* �ܼ� �����͸� �����ϱ����� Ÿ��. */
	/* ������ ���� ��ü�� ���������� �ʴ´�. => �� Ÿ�����δ� ������ ������! */
	typedef XMFLOAT2				_float2;
	typedef XMFLOAT3				_float3;
	typedef XMFLOAT4				_float4;
	typedef XMFLOAT4X4				_float4x4;

	/* ������ ���� ����Ÿ���� �������ش�. */
	/* SIMD ������ ���ؼ�. */


	typedef XMVECTOR				_vector;

	/* Parameters */
	typedef FXMVECTOR				_fvector;
	typedef GXMVECTOR				_gvector;
	typedef HXMVECTOR				_hvector;
	typedef CXMVECTOR				_cvector;

	typedef XMMATRIX				_matrix;	

	/* Parameters */
	typedef FXMMATRIX				_fmatrix;
	typedef CXMMATRIX				_cmatrix;

}

