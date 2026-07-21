Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input {
    float4 Position : SV_Position;
    float4 BGColor : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct Output {
    float4 TxColor : SV_Target0;
    float4 BgColor : SV_Target1;
};

Output main(Input input) {
    Output output;

    output.TxColor = Texture.Sample(Sampler, input.TexCoord);
    if (output.TxColor.a < 1.0f) discard;

    output.TxColor.rgb *= 1.0f - input.BGColor.a;
    output.BgColor = float4(input.BGColor.rgb,1.0f);

    return output;
}
