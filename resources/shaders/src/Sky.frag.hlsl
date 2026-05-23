struct Input {
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
};

float4 main(Input input) : SV_Target0
{
    return float4(sin(input.TexCoord.x),sin(input.TexCoord.y),sin(input.TexCoord.x + input.TexCoord.y),0.5f);
}
