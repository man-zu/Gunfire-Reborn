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

	/* 단순 데이터를 저장하기위한 타입. */
	/* 심플한 연산 자체도 제공해주지 않는다. => 이 타입으로는 연산을 하지마! */
	typedef XMFLOAT2				_float2;
	typedef XMFLOAT3				_float3;
	typedef XMFLOAT4				_float4;
	typedef XMFLOAT4X4				_float4x4;

	/* 연산을 위한 벡터타입을 제공해준다. */
	/* SIMD 연산을 위해서. */


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

