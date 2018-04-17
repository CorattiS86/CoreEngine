// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer constantBufferPerObject : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos		: POSITION;
	float3 normal	: NORMAL0;
	float3 color	: COLOR0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 posWVP	: SV_POSITION;
	float3 posW		: POSITION0;
	float4 normal	: NORMAL0;
	float3 color	: COLOR0;
	float2 texcoord : TEXCOORD0;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	 
	//matrix <float, 4, 4> MyView = {
	// 0.0, 0.0, -1.0, 0.0, // column 1
	// 0.0, 1.0,  0.0, 0.0, // column 2
	// 1.0, 0.0,  0.0, 0.0, // column 3
	// 5.0, 0.0,  0.0, 1.0  // column 4
	//};

	//matrix <float, 4, 4> MyViewInverse = { 
	//	0.0, 0.0,  1.0, 0.0, // column 1
	//	0.0, 1.0,  0.0, 0.0, // column 2
	// -1.0, 0.0,  0.0, 0.0, // column 3
	//	0.0, 0.0, -5.0, 1.0  // column 4
	//};

	// Transform the vertex position into projected space.
	{
		float4 pos = float4(input.pos, 1.0f);

		pos = mul(pos, world);
		pos = mul(pos, view);
		pos = mul(pos, projection);

		output.posWVP = pos;
	}

	// Transform normal vector
	{
		float4 normal = float4(input.normal, 0.0f);

		normal = mul(normal, world);
		output.normal = normal;
	}

	// Pass the position in world space
	{
		float4 pos = float4(input.pos, 1.0f);

		pos = mul(pos, world);

		output.posW = pos;
	}

	// Pass the color through without modification.
	output.color = input.color;

	return output;
}
