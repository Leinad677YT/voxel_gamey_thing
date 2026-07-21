Texture2D DepthTexture : register(t0, space2);
SamplerState DepthSampler : register(s0, space2);

Texture2D BlockColorTexture : register(t1, space2);
SamplerState BlockColorSampler : register(s1, space2);

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

float4 main(float2 TexCoord : TEXCOORD0) : SV_Target0
{


    // get our color & depth value
    float depth = DepthTexture.Sample(DepthSampler, TexCoord).r;
    
    float edge;
    float edge2;
    
    float diff2 = GetDifference(depth, TexCoord, 2.0f);

    float3 res;

    // get the difference between the edges at 2px and 4px away
    if (diff2 < 2E-10) discard;
    
    float diff4 = GetDifference(depth, TexCoord, 4.0f);
    if (abs(2*diff2 - diff4) > diff4*0.875) {
        res = BlockColorTexture.Sample(BlockColorSampler,TexCoord).rgb;
    }
    else discard;
    // combine results
    return float4(res, 1.0f);
}
