Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input {
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
    float4 BGColor : COLOR0;
};

struct Output {
    float4 FrontColor : SV_Target0;
    float4 OutlineColor : SV_Target1;
};

Output main(Input input)
{
    Output output;
    output.FrontColor = Texture.Sample(Sampler, input.TexCoord);
    output.OutlineColor = input.BGColor;

    return output;
}
