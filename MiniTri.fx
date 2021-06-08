struct VS_IN
{
	float4 pos : POSITION0;
	float4 col : COLOR0;
	float4 tex : TEXCOORD0;
}; 

struct LINE_VS_IN
{
	float4 pos : POSITION0;
	float4 col : COLOR0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float4 col : COLOR;
	float4 tex : TEXCOORD;
}; 

struct LINE_PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

struct ConstantData
{
	float4x4 WorldViewProj;
};

cbuffer ConstBuf:register(b0)
{
	ConstantData ConstData;
}

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

PS_IN VSMain( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	output.pos = mul(float4(input.pos.xyz, 1.0f), ConstData.WorldViewProj);
	output.col = input.col;
	output.tex = input.tex;
	return output;
}

PS_IN LINE_VSMain(LINE_VS_IN input)
{
	PS_IN output = (PS_IN)0;
	output.pos = mul(float4(input.pos.xyz, 1.0f), ConstData.WorldViewProj);
	output.col = input.col;
	return output;
}

float4 PSMain( PS_IN input ) : SV_Target
{
	float4 color = input.col;
#ifdef HASTEXTURE
	color = DiffuseMap.Sample(Sampler, float2(input.tex.x, input.tex.y));
	//color = float4(input.tex.x, input.tex.y, 0.0f, 1.0f);
#endif
	return color;
}

float4 LINE_PSMain(LINE_PS_IN input) : SV_Target
{
	float4 color = input.col;
	return color;
}
