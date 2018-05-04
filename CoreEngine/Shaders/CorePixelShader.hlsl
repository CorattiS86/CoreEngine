cbuffer constantBufferPerFrame : register(b0)
{
	float3 eyePosition; float empty;
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

struct DirectionalLight
{
	float3	diffuseColor;
	float3	specularColor;
	float3	lightDirection;
};

struct PointLight
{
	float3	diffuseColor;
	float3	specularColor;

	float3	lightPosition;
	float	range;
};

struct SpotLight
{
	float3	diffuseColor;
	float3	specularColor;

	float3	lightPosition;
	float	range;

	float3  lightDirection;
	float	maxSpot;
};

struct Material
{
	float3 diffuseColor;  float empty;
	float3 specularColor; float specularPower;
};

SamplerState samplerState		: register(s0);
SamplerState samplerAnisotropic : register(s1);


texture2D diffuseMap : register(t0);
texture2D secondMap  : register(t1);




// A pass-through function for the (interpolated) color data.
float3 main(PixelShaderInput input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	Material mat;

	float3 diffuse1 = diffuseMap.Sample(samplerState, input.texcoord);
	float3 diffuse2 = secondMap.Sample(samplerState, input.texcoord);

	mat.diffuseColor = diffuse1;
	//mat.diffuseColor	= float3(input.color);
	mat.specularColor	= float3(1.0f, 1.0f, 1.0f);
	mat.specularPower	= 64;

	float3	D = float3(0.0f, 0.0f, 0.0f);
	float3	S = float3(0.0f, 0.0f, 0.0f);

	// Computation for Directional Light
	{
		DirectionalLight l_Directional;
		l_Directional.diffuseColor		= float3(1.0f, 1.0f, 1.0f);
		l_Directional.specularColor		= float3(1.0f, 1.0f, 1.0f);
		l_Directional.lightDirection	= normalize(float3(0.0f, 1.0f, 0.0f)); //all vector must be normalized

		float cos_LN = dot(l_Directional.lightDirection, input.normal.xyz);

		float Kd = max(cos_LN, 0);
		D += Kd * l_Directional.diffuseColor * mat.diffuseColor;

		[flatten]
		if(cos_LN > 0.0f)
		{
			float3	V	= normalize(eyePosition);
			float3  R	= reflect(-l_Directional.lightDirection, input.normal.xyz); // reflect require the sign minus for direction
			float	Ks  = pow(max(dot(R, V), 0), mat.specularPower);

			S += Ks * l_Directional.specularColor * mat.specularColor;
		}
	}
	///////////////////////////////////////

	// Computation for Point Light
	{
		PointLight	l_Point;
		l_Point.diffuseColor	= float3(1.0f, 1.0f, 1.0f);
		l_Point.specularColor	= float3(1.0f, 1.0f, 1.0f);
		l_Point.lightPosition	= float3(1.0f, 1.0f, -5.0f);
		l_Point.range = 7.0f;

		float3 direction = normalize(
			float3(	l_Point.lightPosition - input.posW )
		);

		float  cos_LN		= dot( direction, input.normal.xyz);
		float  d			= length(l_Point.lightPosition - float3(input.posW.x, input.posW.y, input.posW.z));

		float  Kd = 0.0f;

		if( d < l_Point.range)
			Kd = max(cos_LN, 0) / d;
		
		//D += ( Kd * l_Point.diffuseColor * mat.diffuseColor);

		[flatten]
		if (cos_LN > 0.0f)
		{
			float3	V = normalize(eyePosition);
			float3  R = reflect(-direction, input.normal.xyz); // reflect require the sign minus for direction
			float	Ks = pow(max(dot(R, V), 0), mat.specularPower) / d;

			//S += Ks * l_Point.specularColor * mat.specularColor;
		}
	}

	{
		SpotLight	l_Spot;
		l_Spot.diffuseColor   = float3(1.0f, 1.0f, 1.0f);
		l_Spot.specularColor  = float3(1.0f, 1.0f, 1.0f);
		l_Spot.lightPosition  = float3(0.0f, 3.0f, -5.0f);
		l_Spot.lightDirection = normalize(float3(0.0f, -3.0f, 5.0f));
		l_Spot.maxSpot		  = 0.98f;

		float3 direction = normalize(
			float3(	l_Spot.lightPosition - input.posW )
		);

		float d = length( float3(l_Spot.lightPosition - input.posW) );
		float Kd = 0.0f;

		float cos_LD = dot(direction, -l_Spot.lightDirection);
	
		d = 1;
		if (cos_LD > l_Spot.maxSpot)
		{
			Kd = 1.0f / d;

			//D += Kd * l_Spot.diffuseColor * mat.diffuseColor;
	
			float3	V = normalize(eyePosition);
			float3  R = reflect(-direction, input.normal.xyz); // reflect require the sign inus for direction
			float	Ks = pow(max(dot(R, V), 0), mat.specularPower) / d;

			//S += Ks * l_Spot.specularColor * mat.specularColor;
			
		}
	}
	///////////////////////////////////////
	return  D + S;

	
}
