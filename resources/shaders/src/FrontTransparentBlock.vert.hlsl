cbuffer UBO : register(b0, space1)
{
    float4x4 transform : packoffset(c0);
};

cbuffer UBO2 : register(b1, space1)
{
    float3 translation : packoffset(c0);
};

struct Input {
    float3 Position : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
    float4 BGColor : COLOR0;
};

struct Output {
    float4 Position : SV_Position;
    float4 BGColor : COLOR0;
    float2 TexCoord : TEXCOORD0;
    float2 ScrCoord : TEXCOORD1;
};

Output main(Input input) {
    Output output;

    output.Position = mul(transform, float4(input.Position + translation, 1.0f));

    output.BGColor = input.BGColor;
    output.TexCoord = input.TexCoord;
    output.ScrCoord = (output.Position.rg)/ output.Position.a * 0.5f + 0.5f;

    output.ScrCoord.g = 1.f -output.ScrCoord.g;

    return output;
}
