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
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
    float4 BGColor : COLOR0;
};

Output main(Input input) {
    Output output;
    output.TexCoord = input.TexCoord;
    output.Position = mul(transform, float4(input.Position + translation, 1.0f));
    output.BGColor = input.BGColor;
    return output;
}
