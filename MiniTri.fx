struct VS_IN
{
	float4 pos : POSITION;
	float4 col : COLOR;
	float4 tex : TEXCOORD;
	float4 normal : NORMAL;
}; 

struct LINE_VS_IN
{
	float4 pos : POSITION;
	float4 col : COLOR;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float4 col : COLOR;
	float4 tex : TEXCOORD0;
	float4 worldPos : TEXCOORD1;
	float4 normal :TEXCOORD2;
}; 

struct LINE_PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

struct ConstantData
{
	float4x4 WorldViewProj;
	float4x4 World;
};

cbuffer ConstBuf:register(b0)
{
	ConstantData ConstData;
}

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

struct LightData
{
	float4 Direction;
	float4 Color;
	float4 ViewerPos;
};

cbuffer LightsBuf:register(b1)
{
	LightData Lights;
}

PS_IN VSMain( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	output.pos = mul(float4(input.pos.xyz, 1.0f), ConstData.WorldViewProj);
	output.worldPos = mul(float4(input.pos.xyz, 1.0f), ConstData.World);
	output.col = input.col;
	output.tex = input.tex;
	output.normal = mul(float4(input.normal.xyz, 1.0f), ConstData.World);;
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
#endif

	float3 LightDir = Lights.Direction.xyz;
	float3 normal = normalize(input.normal.xyz);

	float3 viewDir = normalize(Lights.ViewerPos.xyz - input.worldPos.xyz);
	float3 refVec = normalize(reflect(LightDir, normal));
	
	float3 ambient = color.xyz * 0.5;
	float3 diffuse = saturate(dot(LightDir, normal)) * color.xyz;
	float3 specular = pow(saturate(dot(-viewDir, refVec)), 0.7) * 0.3;

	return float4(Lights.Color.xyz * (ambient + diffuse + specular), 1.0f);
}

float4 LINE_PSMain(LINE_PS_IN input) : SV_Target
{
	float4 color = input.col;
	return color;
}
