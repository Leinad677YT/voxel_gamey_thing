Texture2D TransparencyTexture : register(t0, space2);
SamplerState TransparencySampler : register(s0, space2);

Texture2D AuxTexture : register(t1, space2);
SamplerState AuxSampler : register(s1, space2);


#include "constants.hlsl"

float4 main(float2 TexCoord : TEXCOORD0) : SV_Target0
{

    // get colors
    float4 color = float4(TransparencyTexture.Sample(TransparencySampler, TexCoord).rgba);
    float4 aux = float4(AuxTexture.Sample(AuxSampler, TexCoord).rgba);
    
    if (color.a == 0.f) discard;
    // combine results
    return float4(float3(color.rgb / (aux.r / SMOL_FLOAT)),color.a * RESCALE_UP / aux.a / SMOL_FLOAT) ;
}
