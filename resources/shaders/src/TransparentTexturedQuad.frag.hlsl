Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input {
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
};

struct Output {
    float4 Color : TEXCOORD0;
};

float4 main(Input input) : SV_Target0
{
    Output output;
    output.Color = Texture.Sample(Sampler, input.TexCoord);

    if (output.Color.a < 1) discard;
    //output.Color = output.Color.bgra;
    return output.Color;
}
