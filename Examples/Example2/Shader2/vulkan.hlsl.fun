[[vk::push_constant]]
cbuffer vulkanConstants {
	matrix vert_worldMatrix;
	matrix vert_viewMatrix;
	matrix vert_projectionMatrix;
}


struct VS_INPUT {
    float4 position  : POSITION0;
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

    output.position = input.position;
    output.position = mul(vert_worldMatrix, output.position);
    output.position = mul(vert_viewMatrix, output.position);
    output.position = mul(vert_projectionMatrix, output.position);
    output.color = input.color;

    return output;
}

PS_OUTPUT PS(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.color = input.color;
    //output.color.g = input.position.z;
    return output;
}

