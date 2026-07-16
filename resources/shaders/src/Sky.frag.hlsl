struct Input {
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
};

float4 main(Input input) : SV_Target0
{
    return float4(
        sin(input.TexCoord.x)/4.f,
        sin(input.TexCoord.y)/2.f,
        sin(input.TexCoord.x + input.TexCoord.y)/2.f,
        1.f
        );
}
