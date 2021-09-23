struct VS_INPUT {
    float2 position  : POSITION0;
    float4 color     : COLOR0;
};

struct PS_INPUT {
    float4 position  : SV_POSITION;
    float4 color     : COLOR0;
};

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output = (PS_INPUT)0;

    output.position = float4(input.position, 0, 1);
    output.color = input.color;

    return output;
}

PS_OUTPUT PS(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.color = input.color;
    return output;
}
