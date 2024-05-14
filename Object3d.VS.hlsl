struct VertexShaderOutput
{
    float4 position : SV_Position;
};

struct VertexShaderInput
{
    float4 position : Position;
};

VertexShaderOutput main( VertexShaderInput input )
{
    VertexShaderOutput output;
    output.position = input.position;
	return output;
}