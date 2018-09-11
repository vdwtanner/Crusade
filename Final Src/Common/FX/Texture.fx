
cbuffer perObject
{
	float4x4 gWorldViewProj;
	float4x4 gWorld;
	float4 gColorCorrect;
};

cbuffer cbFixed
{
	float2 gTexCoords[4] = 
	{
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f)
	};

};

Texture2DArray gDiffuseMaps;

struct VertexIn
{
	float3 PosL		: POSITION;
	float2 Size		: SIZE;
	float Rotation	: ROTATION;
	uint TexIndex	: TEXINDEX;
	float4 Color	: COLOR;

};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float2 Tex		: TEXCOORD;
	uint TexIndex	: TEXINDEX;
	float4 Color	: COLOR;
};

struct TilePointOut
{
	float3 PosL		: POSITION;
	float2 Size		: SIZE;
	float Rotation	: ROTATION;
	uint TexIndex	: TEXINDEX;
	float4 Color	: COLOR;
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = CLAMP;
};

RasterizerState rastCullNone
{
	CULLMODE = NONE;
};

DepthStencilState depthDisable
{
	DepthEnable = false;
	DepthWriteMask = Zero;
	
	StencilEnable = false;
	StencilReadMask = 0xFF;
	StencilWriteMask = 0xFF;

	FrontFaceStencilFunc = Always;
	FrontFaceStencilPass = Incr;
	FrontFaceStencilFail = Keep;

	BackFaceStencilFunc = Always;
	BackFaceStencilPass = Incr;
	BackFaceStencilFail = Keep;

};

TilePointOut VS(VertexIn vin){
	TilePointOut tout;

	tout.PosL = vin.PosL;
	tout.Size = vin.Size;
	tout.Rotation = vin.Rotation;
	tout.TexIndex = vin.TexIndex;
	tout.Color = vin.Color;

	return tout;
}

// expands a single point into a 1 by 1 quad, used for tile-based 2d-maps
[maxvertexcount(4)]
void GS(point TilePointOut gin[1], inout TriangleStream<VertexOut> triStream){

	float a = gin[0].Rotation;

	float4 v[4];
	v[0] = float4(gin[0].PosL.x, gin[0].PosL.y + gin[0].Size.y, gin[0].PosL.z, 1.0f);			// Top Left
	v[1] = float4(gin[0].PosL.x + gin[0].Size.x, gin[0].PosL.y + gin[0].Size.y, gin[0].PosL.z, 1.0f);	// Top Right
	v[2] = float4(gin[0].PosL, 1.0f);											// Bottom Left
	v[3] = float4(gin[0].PosL.x + gin[0].Size.x, gin[0].PosL.y, gin[0].PosL.z, 1.0f);		// Bottom Right

	float2 c0 = float2(v[0].x + (gin[0].Size.x / 2), v[0].y - (gin[0].Size.y / 2));
	float2 c1 = float2(v[1].x - (gin[0].Size.x / 2), v[1].y - (gin[0].Size.y / 2));
	float2 c2 = float2(v[2].x + (gin[0].Size.x / 2), v[2].y + (gin[0].Size.y / 2));
	float2 c3 = float2(v[3].x - (gin[0].Size.x / 2), v[3].y + (gin[0].Size.y / 2));

	float2 temp0 = float2(v[0].x - c0.x, v[0].y - c0.y);
	float2 temp1 = float2(v[1].x - c1.x, v[1].y - c1.y);
	float2 temp2 = float2(v[2].x - c2.x, v[2].y - c2.y);
	float2 temp3 = float2(v[3].x - c3.x, v[3].y - c3.y);

	v[0] = float4(temp0.x * cos(a) - temp0.y * sin(a), temp0.x * sin(a) + temp0.y * cos(a), v[0].zw);
	v[1] = float4(temp1.x * cos(a) - temp1.y * sin(a), temp1.x * sin(a) + temp1.y * cos(a), v[1].zw);
	v[2] = float4(temp2.x * cos(a) - temp2.y * sin(a), temp2.x * sin(a) + temp2.y * cos(a), v[2].zw);
	v[3] = float4(temp3.x * cos(a) - temp3.y * sin(a), temp3.x * sin(a) + temp3.y * cos(a), v[3].zw);

	v[0] = float4(v[0].x + c0.x, v[0].y + c0.y, v[0].zw);
	v[1] = float4(v[1].x + c1.x, v[1].y + c1.y, v[1].zw);
	v[2] = float4(v[2].x + c2.x, v[2].y + c2.y, v[2].zw);
	v[3] = float4(v[3].x + c3.x, v[3].y + c3.y, v[3].zw);


	VertexOut vout;
	[unroll]
	for (int i = 0; i < 4; i++){
		vout.PosH = mul(v[i], gWorldViewProj);
		vout.Tex = gTexCoords[i];
		vout.TexIndex = gin[0].TexIndex;
		vout.Color = gin[0].Color;

		triStream.Append(vout);
	}

}

float4 PS(VertexOut pin) : SV_Target
{
	return gDiffuseMaps.Sample(samPoint, float3(pin.Tex, pin.TexIndex)) * pin.Color * gColorCorrect;
}

float4 NoTexPS(VertexOut pin) : SV_Target
{
	return pin.Color * gColorCorrect;
}

technique11 SpriteTech
{
	pass P0
	{
		SetRasterizerState(rastCullNone);
		SetDepthStencilState(depthDisable, 0);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

technique11 ColorBoxTech
{
	pass P0
	{
		SetRasterizerState(rastCullNone);
		SetDepthStencilState(depthDisable, 0);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetPixelShader(CompileShader(ps_5_0, NoTexPS()));
	}

}