Texture2D TransparencyTexture : register(t0, space2);
SamplerState TransparencySampler : register(s0, space2);

Texture2D AuxTexture : register(t1, space2);
SamplerState AuxSampler : register(s1, space2);

Texture2D AuxTexture2 : register(t2, space2);
SamplerState AuxSampler2 : register(s2, space2);


#include "constants.hlsl"

float4 main(float2 TexCoord : TEXCOORD0) : SV_Target0
{

    // get colors
    float4 color = float4(TransparencyTexture.Sample(TransparencySampler, TexCoord).rgba);
    float4 aux = float4(AuxTexture.Sample(AuxSampler, TexCoord).rgba);
    float4 aux2 = float4(AuxTexture2.Sample(AuxSampler2, TexCoord).rgba);
    
    if (color.a == 0.f) discard; // discarding 1.0f seems to reduce the amount of diagonals
    // combine results
    color = float4(float3(color.rgb / (aux.r / SMOL_FLOAT)),color.a * RESCALE_UP / SMOL_FLOAT / aux.a);
    return float4(float3(color.rgb * 0.80 + aux2.rgb * 0.20),color.a);
}
