Texture2D tex;
Texture2D fun;
SamplerState smp;

struct VS_INPUT {
    float2 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 uv : COLOR0;
};

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output = (PS_INPUT)0;

    output.position = float4(input.position, 0, 1);
    output.uv = input.uv;

    return output;
}

PS_OUTPUT PS(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.color = min(fun.Sample(smp, input.uv), tex.Sample(smp, input.uv));
    return output;
}
