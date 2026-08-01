Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler0 : register(s0, space2);

Texture2D<float4> TransparencyColor : register(t1, space2);
SamplerState Sampler1 : register(s1, space2);

Texture2D<float4> TransparencyAmount : register(t2, space2);
SamplerState Sampler2 : register(s2, space2);

struct Input {
    float4 Position : SV_Position;
    float4 BGColor : COLOR0;
    float2 TexCoord : TEXCOORD0;
    float2 ScrCoord : TEXCOORD1;
};

struct Output {
    float4 FrontColor : SV_Target0;
    float4 OutlineColor : SV_Target1;
};

#include "constants.hlsl"

Output main(Input input)
{
    Output output;
    
    // get colors
    float4 front_color = Texture.Sample(Sampler0, input.TexCoord);
    if (front_color.a == 0.0f || front_color.a == 1.0f) discard;

    // sample from average textures
    float w,h; TransparencyColor.GetDimensions(w,h);
    float4 avg_color = TransparencyColor.Sample(Sampler1, input.Position.rg / float2(w,h));
    float4 avg_amount = TransparencyAmount.Sample(Sampler2, input.Position.rg / float2(w,h));
    
    // combine results
    output.FrontColor =
          // avg
        float4((0.8f * (avg_color.rgb / (avg_amount.r / SMOL_FLOAT))), 0.3f * avg_color.a * RESCALE_UP / SMOL_FLOAT / avg_amount.a)
        + // front
        float4(0.2f * front_color.rgb, 0.8f* front_color.a)
    ;

    output.FrontColor.a = front_color.a;
    output.OutlineColor = input.BGColor;




    return output;
}
