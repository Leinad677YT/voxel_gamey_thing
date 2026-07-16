Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input {
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
    float4 BGColor : COLOR0;
};

struct Output {
    float4 Color : SV_Target0;
    float4 Color2 : SV_Target1;
};

#include "constants.hlsl"

Output main(Input input)
{
    Output output;

    // get the face pixel
    output.Color = Texture.Sample(Sampler, input.TexCoord);
    if (output.Color.a == 0.f) discard;

    output.Color.rgb *= 1 - input.BGColor.a;

    // add the data to the averager (+1 to the colors, +a to the alpha)
    output.Color2 = float4(SMOL_FLOAT,0.f,0.f,output.Color.a) * RESCALE_DOWN;

    // somehow this made it so that they almost dont converge to 1
    // output.Color = float4(float3(output.Color.rgb * (1.f-pow(output.Color.a,2)+output.Color.a/3)),output.Color.a) * RESCALE_DOWN;
    
    output.Color *= RESCALE_DOWN;

    
    // output.Color  = float4(0.f,0.f,input.Position.b,1.f);
    // output.Color2 = float4(0.f,0.f,input.Position.b,1.f);
    
    return output;
}
