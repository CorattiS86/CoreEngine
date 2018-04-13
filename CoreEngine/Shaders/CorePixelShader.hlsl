// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 posWVP	: SV_POSITION;
	float3 posW		: POSITION0;
	float4 normal	: NORMAL0;
	float3 color	: COLOR0;
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

struct Material
{
	float3	diffuseColor;
	float3  specularColor;
	float	specularPower;
};

// A pass-through function for the (interpolated) color data.
float3 main(PixelShaderInput input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	Material mat;
	mat.diffuseColor	= float3(input.color);
	mat.specularColor	= float3(1.0f, 1.0f, 1.0f);
	mat.specularPower	= 4;

	float3	D = float3(0.0f, 0.0f, 0.0f);
	float3	S = float3(0.0f, 0.0f, 0.0f);

	// Computation for Directional Light
	{
		DirectionalLight l_Directional;
		l_Directional.diffuseColor		= float3(1.0f, 1.0f, 1.0f);
		l_Directional.specularColor		= float3(1.0f, 1.0f, 1.0f);
		l_Directional.lightDirection	= normalize(float3(1.0f, 1.0f, 0.0f)); //all vector must be normalized

		float cos_LN = dot(l_Directional.lightDirection, input.normal);

		float Kd = max(cos_LN, 0);
		//D += Kd * l_Directional.diffuseColor * mat.diffuseColor;

		[flatten]
		if(cos_LN > 0.0f)
		{
			float3	V	= normalize( float3(0.0, 2.0, 5.0) );
			float3  R	= reflect(-l_Directional.lightDirection, input.normal); // reflect require the sign minus for direction
			float	Ks  = pow(max(dot(R, V), 0), mat.specularPower);

			//S += Ks * l_Directional.specularColor * mat.specularColor;
		}
	}
	///////////////////////////////////////

	// Computation for Point Light
	{
		PointLight	l_Point;
		l_Point.diffuseColor	= float3(1.0f, 1.0f, 1.0f);
		l_Point.specularColor	= float3(1.0f, 1.0f, 1.0f);
		l_Point.lightPosition	= float3(0.0f, 1.0f, -8.0f);

		float3 direction = normalize(float3(
			l_Point.lightPosition.x - input.posW.x,
			l_Point.lightPosition.y - input.posW.y,
			l_Point.lightPosition.z - input.posW.z
			)
		);

		float  cos_LN		= dot( direction, input.normal);
		float  d			= length(l_Point.lightPosition - float3(input.posW.x, input.posW.y, input.posW.z));

		float  Kd = max(cos_LN, 0) / d;

		D += ( Kd * l_Point.diffuseColor * mat.diffuseColor);
	}
	{
		PointLight	l_Point;
		l_Point.diffuseColor = float3(1.0f, 1.0f, 1.0f);
		l_Point.specularColor = float3(1.0f, 1.0f, 1.0f);
		l_Point.lightPosition = float3(5.0f, 1.0f, -2.0f);

		float3 direction = normalize(float3(
			l_Point.lightPosition.x - input.posW.x,
			l_Point.lightPosition.y - input.posW.y,
			l_Point.lightPosition.z - input.posW.z
			)
		);

		float  cos_LN = dot(direction, input.normal);
		float  d = length(l_Point.lightPosition - float3(input.posW.x, input.posW.y, input.posW.z));

		float  Kd = max(cos_LN, 0) / d;

		D += (Kd * l_Point.diffuseColor * mat.diffuseColor);
	}
	
	///////////////////////////////////////
	return D + S;
}
