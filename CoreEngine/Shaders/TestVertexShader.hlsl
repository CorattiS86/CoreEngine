// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 pos      : POSITION;
    float4 normal   : NORMAL0;
    float3 color    : COLOR0;
    float2 texcoord : TEXCOORD0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos      : SV_POSITION;
    float3 color    : COLOR0;
};

texture2D texture0 : register(t0);

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    output.pos      = float4(input.pos, 1.0f);
    output.color    = input.color;
    
    return output;
}