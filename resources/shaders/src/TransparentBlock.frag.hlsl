Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input {
    float4 Position : SV_Position;
    float4 BGColor : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct Output {
    float4 Color : SV_Target0;
    float4 Color2 : SV_Target1;
};

#include "constants.hlsl"

Output main(Input input) {
    Output output;

    // get the face pixel
    output.Color = Texture.Sample(Sampler, input.TexCoord);
    if (output.Color.a == 0.f) discard;

    output.Color.rgb *= 1 - input.BGColor.a;

    // add the data to the averager (+1 to the colors, +a to the alpha)
    output.Color2 = float4(SMOL_FLOAT,0.f,0.f,output.Color.a * SMOL_FLOAT) * RESCALE_DOWN;
    
    output.Color *= RESCALE_DOWN;

    return output;
}
