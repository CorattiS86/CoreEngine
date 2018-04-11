// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL0;
	float3 color : COLOR0;
};

// A pass-through function for the (interpolated) color data.
float3 main(PixelShaderInput input) : SV_TARGET
{
	float3 lightColor		= float3( 1.0f, 1.0f, 1.0f);
	float3 lightDirection	= float3( 0.0f, 1.0f, 0.0f);
	
	float3 matDiffuse = float3(input.color);

	float Kd = max( dot(lightDirection, input.normal) , 0);
	
	float3 diffuseColor = Kd * lightColor * matDiffuse;

	return diffuseColor;
}
