// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos		: SV_POSITION;
	float4 normal	: NORMAL0;
	float3 color	: COLOR0;
};

struct DirectionalLight
{
	float3	diffuseColor;
	float3	specularColor;
	float3	lightDirection;
};


struct Material
{
	float3	diffuseColor;
	float3  specularColor;
};

// A pass-through function for the (interpolated) color data.
float3 main(PixelShaderInput input) : SV_TARGET
{
	DirectionalLight l_Directional;
	l_Directional.diffuseColor		= float3(1.0f, 1.0f, 1.0f);
	l_Directional.specularColor		= float3(1.0f, 1.0f, 1.0f);
	l_Directional.lightDirection	= normalize( float3(0.0f, -5.0f, 5.0f) ); //all vector must be normalized
	
	//l_Directional.lightDirection = reflect(-l_Directional.lightDirection, float3(0.0f, 1.0f, 0.0f));

	Material mat;
	mat.diffuseColor	= float3(input.color);
	mat.specularColor	= float3(1.0f, 1.0f, 1.0f);

	float3	D = float3(0.0f, 0.0f, 0.0f);
	float3	S = float3(0.0f, 0.0f, 0.0f);

	float cos_LN = dot(l_Directional.lightDirection, input.normal);
	
	float Kd = max(cos_LN, 0);
	D += Kd * l_Directional.diffuseColor * mat.diffuseColor;

	[flatten]
	if(cos_LN > 0.0f)
	{
		float3	V	= normalize( float3(0.0, 5.0, 5.0) );
		float3  R	= reflect(-l_Directional.lightDirection, input.normal); // reflect require the sign minus for direction
		float	Ks  = pow(max(dot(R, V), 0), 16);

		S += Ks * l_Directional.specularColor * mat.specularColor;
	}
	
	return D + S;
}
