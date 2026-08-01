Texture2D DepthTexture : register(t0, space2);
SamplerState DepthSampler : register(s0, space2);

Texture2D BlockColorTexture : register(t1, space2);
SamplerState BlockColorSampler : register(s1, space2);

Texture2D TransparencyTexture : register(t2, space2);
SamplerState TransparencySampler : register(s2, space2);

#include "constants.hlsl"

// Gets the difference between a depth value and adjacent depth pixels
// This is used to detect "edges", where the depth falls off.
float GetDifference(float depth, float2 TexCoord, float distance)
{
    float w, h;
    DepthTexture.GetDimensions(w, h);
    
    return
        max(DepthTexture.Sample(DepthSampler, TexCoord + float2(1.0 / w, 0) * distance).r - depth,
         max(DepthTexture.Sample(DepthSampler, TexCoord + float2(-1.0 / w, 0) * distance).r - depth,
          max(
            DepthTexture.Sample(DepthSampler, TexCoord + float2(0, 1.0 / h) * distance).r - depth,
            DepthTexture.Sample(DepthSampler, TexCoord + float2(0, -1.0 / h) * distance).r - depth
          )
         )
        );
}

float4 DrawTransparency(float2 TexCoord) {
    float4 output = TransparencyTexture.Sample(TransparencySampler,TexCoord);
    if (output.a == 0) discard;
    output.a *= 1.2f;
    return output;
}

float4 main(float2 TexCoord : TEXCOORD0) : SV_Target0
{
    // get our color & depth value
    float depth = DepthTexture.Sample(DepthSampler, TexCoord).r;

    if (depth < 1.0f - 2*SMOL_FLOAT) return DrawTransparency(TexCoord);

    float diff2 = GetDifference(depth, TexCoord, 2.0f);

    // get the difference between the edges at 2px and 4px away
    if (diff2 < 2E-10) return DrawTransparency(TexCoord);

    float3 res;
    float diff4 = GetDifference(depth, TexCoord, 4.0f);
    if (abs(8*diff2 - 4*diff4) > max(diff4*diff2*0.875,0.0001f) && abs(8*diff2 - 4*diff4) > 2* diff2) {
        res = BlockColorTexture.Sample(BlockColorSampler,TexCoord).rgb;
    }
    else return DrawTransparency(TexCoord);
    // combine results
    return float4(res, 0.8f);
}
