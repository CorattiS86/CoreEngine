#pragma once
#include <DirectXMath.h>
#include <windows.h>

using namespace DirectX;
//-----------------------------------------------------------------------------
// Per-vertex data
//-----------------------------------------------------------------------------
typedef struct VERTEX_POSITION_COLOR
{
	XMFLOAT3 pos;
	XMFLOAT3 color;
} VertexPositionColor;

typedef struct VERTEX_POSITION_NORMAL_COLOR
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT3 color;
} VertexPositionNormalColor;

typedef struct VERTEX_POSITION_NORMAL_TEXCOORD
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT3 color;
	XMFLOAT2 texcoord;
} VertexPosNorColTex;
//-----------------------------------------------------------------------------
// Per-vertex data (extended)
//-----------------------------------------------------------------------------
typedef struct VERTEX_POSITION_NORMAL_TANGENT
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
} VertexPositionColorTangent;

//-----------------------------------------------------------------------------
// Variables for rendering the cube
//-----------------------------------------------------------------------------
typedef struct _constantBufferStruct {
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
} ConstantBufferStruct;

// Assert that the constant buffer remains 16-byte aligned.
static_assert((sizeof(ConstantBufferStruct) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

//-----------------------------------------------------------------------------
// Variables for rendering the cube
//-----------------------------------------------------------------------------
typedef struct _OtherconstantBufferStruct {
	XMFLOAT4 eye;
} OtherConstantBufferStruct;

// Assert that the constant buffer remains 16-byte aligned.
static_assert((sizeof(OtherConstantBufferStruct) % 16) == 0, "Constant Buffer size must be 16-byte aligned");