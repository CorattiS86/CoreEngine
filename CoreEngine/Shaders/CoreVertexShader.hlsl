// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

cbuffer InverseTransposeConstantBuffer : register(b1)
{
	matrix inverse;
	matrix transpose;
}

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 normal : NORMAL0;
	float3 color : COLOR0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL0;
	float3 color : COLOR0;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	matrix model_view_proj = mul(view, projection);
	model_view_proj = mul(model, model_view_proj);

	//pos = mul(pos, model);
	//pos = mul(pos, view);
	//pos = mul(pos, projection);

	output.pos = mul(pos, model_view_proj);

	float4 normal = float4(input.normal, 1.0f);
	
	output.normal = normal;

	// Pass the color through without modification.
	output.color = input.color;

	return output;
}
