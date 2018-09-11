/*********************************************
	Based on the particlesgs sample in the
	DirectX SDK (June 2010), as well as 
	Chapter 20 of Frank D. Luna's book,
	"Introduction to 3D Game Programming with
	DirectX 11"
**********************************************/


cbuffer perDraw
{
	float4x4 gWorldViewProj;
	float gGlobalTime;
	float gElapsedTime;
	float4 gGravity;
	float gBloodParticleSize = .02f;
};

// contains the blocks
Texture2DArray gDiffuseMaps;

struct VSParticleIn
{
	float3 pos			: POSITION;
	float2 vel			: VELOCITY;
	float2 size			: SIZE;
	float timer			: TIMER;
	float emitInterval	: EMITTER_INTERVAL;
	uint type			: TYPE;
	uint etype			: ETYPE;
	uint texIndex		: TEXINDEX;
};

struct VSParticleDrawOut
{
	float4 pos			: POSITION;
	float2 size			: SIZE;
	float2 texCoords	: TEXCOORD;
	uint texIndex		: TEXINDEX;
	float4 color		: COLOR;
};

struct PSInput
{
	// the Z member of pos is used to determine if we use the texture or not
	// Z = 0.0f, use the texture
	// Z = 1.0f, dont use the texture
	float4 pos			: SV_POSITION;
	float2 tex			: TEXTURE;
	float texIndex		: TEXINDEX;
	float4 color		: COLOR;
};

Texture1D gtexRandom;
SamplerState gSamPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	//AddressV = Wrap;
};

BlendState AdditiveBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = ONE;
	BlendOp = ADD;
	SrcBlendAlpha = ZERO;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

RasterizerState rastCullNone
{
	CULLMODE = NONE;
};

#define USE_TEXTURE 0
#define USE_COLOR_ONLY 1

#define PT_EMITTER	10
#define PT_BLOOD	1
#define PT_RAIN		2
#define PT_SMOKE	3
#define PT_EMBER	4
#define PT_BLOCK	5

#define PT_DEATHGORE 6
#define PT_HOLYSHOWER 7
#define PT_EXPBURST 8

cbuffer cFixed {
	float gLifeTimes[9] =
	{
		0.0f,	// unused
		1.0f,	// blood life time
		3.0f,	// rain life time
		2.0f,	// smoke life time
		1.0f,	// ember life time
		1.0f,	// block life time
		1.0f,	// gore life time
		0.8f,	// Holy shower life time
		0.8f,	// EXP Burst life time
	};
	float3 gPositions[4] =
	{
		float3(-1, 1, 0),
		float3(1, 1, 0),
		float3(-1, -1, 0),
		float3(1, -1, 0),
	};
	float2 gTexCoords[4] =
	{
		float2(0, 0),
		float2(1, 0),
		float2(0, 1),
		float2(1, 1),
	};
};


float3 RandomDir(float offset)
{
	float tCoord = (gGlobalTime + offset) / 300.0;
	return gtexRandom.SampleLevel(gSamPoint, tCoord, 0);
}

/**************************
	VERTEX SHADERS
***************************/

VSParticleDrawOut VSDrawMain(VSParticleIn input)
{
	VSParticleDrawOut output = (VSParticleDrawOut)0;

	output.size = input.size;

	if (input.type == PT_EMITTER)
	{
		output.pos = float4(input.pos.xy, USE_COLOR_ONLY, 0);
		output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else if (input.type == PT_BLOOD)
	{
		output.pos = float4(input.pos.xy, USE_COLOR_ONLY, 0);
		// red
		output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (input.type == PT_RAIN)
	{
		output.pos = float4(input.pos.xy, USE_COLOR_ONLY, 0);
		// blue
		output.color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else if (input.type == PT_SMOKE)
	{
		output.pos = float4(input.pos.xy, USE_COLOR_ONLY, 0);
		// gray
		output.color = float4(0.25f, 0.25f, 0.25f, .75f);
	}
	else if (input.type == PT_EMBER)
	{
		output.pos = float4(input.pos.xy, USE_COLOR_ONLY, 0);
		// yellow
		output.color = float4(1.0f, 1.0f, 0.0f, .75f);
	}
	else if (input.type == PT_BLOCK)
	{
		output.pos = float4(input.pos.xy, USE_TEXTURE, 0);
		output.texCoords = float2(0.75f, 0.75f);
		output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else if (input.type == PT_DEATHGORE)
	{
		output.pos = float4(input.pos.xy, USE_COLOR_ONLY, 0);
		// red
		output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (input.type == PT_HOLYSHOWER)
	{
		output.pos = float4(input.pos.xy, USE_COLOR_ONLY, 0);
		// blue
		output.color = float4(0.0f, 1.0f, 0.3f, 1.0f);
	}
	else if (input.type == PT_EXPBURST)
	{
		output.pos = float4(input.pos.xy, USE_COLOR_ONLY, 0);
		// blue
		output.color = float4(0.93f, .79f, 0.0f, 1.0f);
	}
	output.texIndex = input.texIndex;
	return output;
}

VSParticleIn VSUpdateMain(VSParticleIn input)
{
	return input;
}

/**************************
	GEOMETRY SHADERS
***************************/

void GSEmitterHandler(VSParticleIn input, uint primID, inout PointStream<VSParticleIn> ParticleOutputStream)
{
	if (input.timer <= 0)
	{
		if (input.etype == PT_BLOCK)
		{
			for (int i = 0; i < 8; i++){
				float3 vRandom = normalize(RandomDir(input.type + i + primID * (input.etype - PT_BLOOD)));
				//output a new particle of the emit type
				VSParticleIn output;
				output.pos = input.pos + (vRandom * 0.5f);
				output.vel = vRandom*3.5 + float3(input.vel*gElapsedTime, 0);
				output.size = input.size;
				output.timer = gLifeTimes[input.etype] + vRandom.y*0.5f;
				output.emitInterval = 1.0f;
				output.type = input.etype;
				output.etype = input.etype;
				output.texIndex = input.texIndex;

				//reset emit timer
				//input.timer = input.emitInterval;

				//ParticleOutputStream.Append(input);
				ParticleOutputStream.Append(output);
			}
		}
		else if (input.etype == PT_DEATHGORE)
		{
			for (int i = 0; i < 32; i++){
				float3 vRandom = normalize(RandomDir(input.type + i + primID * (input.etype - PT_BLOOD)));
					//output a new particle of the emit type
					VSParticleIn output;
				output.pos = input.pos + (vRandom * 0.5f);
				output.vel = vRandom*3.5 + float3(input.vel*gElapsedTime, 0);
				output.size = input.size;
				output.timer = gLifeTimes[input.etype] + vRandom.y*0.5f;
				output.emitInterval = 1.0f;
				output.type = input.etype;
				output.etype = input.etype;
				output.texIndex = input.texIndex;

				//reset emit timer
				//input.timer = input.emitInterval;

				//ParticleOutputStream.Append(input);
				ParticleOutputStream.Append(output);
			}
		}
		else if(input.etype == PT_HOLYSHOWER)
		{
			for (int i = 0; i < 32; i++){
				float3 vRandom = normalize(RandomDir(input.type + i + primID));
					//output a new particle of the emit type
					VSParticleIn output;
				output.pos = input.pos + (vRandom * 0.5f);
				output.vel = vRandom*3.5 + float3(input.vel*gElapsedTime, 0);
				output.size = input.size;
				output.timer = gLifeTimes[input.etype] + vRandom.y*0.5f;
				output.emitInterval = 1.0f;
				output.type = input.etype;
				output.etype = input.etype;
				output.texIndex = input.texIndex;

				//reset emit timer
				//input.timer = input.emitInterval;

				//ParticleOutputStream.Append(input);
				ParticleOutputStream.Append(output);
			}
		}
		else if(input.etype == PT_EXPBURST)
		{
			for (int i = 0; i < 32; i++){
				float3 vRandom = normalize(RandomDir(input.type + i + primID));
					//output a new particle of the emit type
					VSParticleIn output;
				output.pos = input.pos + (vRandom * 0.5f);
				output.vel = vRandom*3.5 + float3(input.vel*gElapsedTime, 0);
				output.size = input.size;
				output.timer = gLifeTimes[input.etype] + vRandom.y*0.5f;
				output.emitInterval = 1.0f;
				output.type = input.etype;
				output.etype = input.etype;
				output.texIndex = input.texIndex;

				//reset emit timer
				//input.timer = input.emitInterval;

				//ParticleOutputStream.Append(input);
				ParticleOutputStream.Append(output);
			}
		}
		else
		{
			float3 vRandom = normalize(RandomDir(input.type + primID * (input.etype - PT_BLOOD)));
				//output a new particle of the emit type
				VSParticleIn output;
			output.pos = input.pos;
			output.vel = float3(input.vel*gElapsedTime, 0) + vRandom*3.5;
			output.size = input.size;
			output.timer = gLifeTimes[input.etype] + vRandom.y*0.5f;
			output.emitInterval = 1.0f;
			output.type = input.etype;
			output.etype = input.etype;
			output.texIndex = input.texIndex;

			//reset emit timer
			//input.timer = input.emitInterval;

			//ParticleOutputStream.Append(input);
			ParticleOutputStream.Append(output);
		}

	}
}

void GSBloodHandler(VSParticleIn input, uint primID, inout PointStream<VSParticleIn> ParticleOutputStream)
{
	if (input.timer > 0){
		input.pos += float3(input.vel*gElapsedTime, input.pos.z);
		input.vel += gGravity.xy * gElapsedTime;
		input.size = float2(gBloodParticleSize, gBloodParticleSize);
		input.timer -= gElapsedTime;

		ParticleOutputStream.Append(input);
	}
}

void GSRainHandler(VSParticleIn input, uint primID, inout PointStream<VSParticleIn> ParticleOutputStream)
{
	if (input.timer > 0){
		input.pos += float3(input.vel*gElapsedTime, input.pos.z);
		input.vel += gGravity.xy * gElapsedTime;
		input.timer -= gElapsedTime;

		ParticleOutputStream.Append(input);
	}
}

void GSSmokeHandler(VSParticleIn input, uint primID, inout PointStream<VSParticleIn> ParticleOutputStream)
{
	if (input.timer > 0){
		input.pos += float3(input.vel*gElapsedTime, input.pos.z);
		input.vel -= (gGravity.xy / 1.5f) * gElapsedTime;
		input.timer -= gElapsedTime;

		ParticleOutputStream.Append(input);
	}
}

void GSEmberHandler(VSParticleIn input, uint primID, inout PointStream<VSParticleIn> ParticleOutputStream)
{
	if (input.timer > 0){
		input.pos += float3(input.vel*gElapsedTime, input.pos.z);
		input.vel -= gGravity.xy * gElapsedTime;
		input.timer -= gElapsedTime;

		ParticleOutputStream.Append(input);
	}
}

void GSBlockHandler(VSParticleIn input, uint primID, inout PointStream<VSParticleIn> ParticleOutputStream)
{
	if (input.timer > 0){
		input.pos += float3(input.vel*gElapsedTime, input.pos.z);
		input.vel += gGravity.xy * gElapsedTime;
		input.timer -= gElapsedTime;

		ParticleOutputStream.Append(input);
	}
}

void GSDeathHandler(VSParticleIn input, uint primID, inout PointStream<VSParticleIn> ParticleOutputStream)
{
	if (input.timer > 0){
		input.pos += float3(input.vel*gElapsedTime, input.pos.z);
		input.vel += gGravity.xy * gElapsedTime;
		input.timer -= gElapsedTime;

		ParticleOutputStream.Append(input);
	}
}

[maxvertexcount(32)]
void GSUpdateParticles(point VSParticleIn input[1], uint primID : SV_PrimitiveID, inout PointStream<VSParticleIn> ParticleOutputStream)
{
	// this dynamic branching hurts me, but the DirectX June 2010 sample that does this still gets
	// like 1900 fps on my gpu so... Don't optimize if you don't have to I suppose.
	if (input[0].type == PT_EMITTER)
		GSEmitterHandler(input[0], primID, ParticleOutputStream);
	else if (input[0].type == PT_BLOOD)
		GSBloodHandler(input[0], primID, ParticleOutputStream);
	else if (input[0].type == PT_RAIN)
		GSRainHandler(input[0], primID, ParticleOutputStream);
	else if (input[0].type == PT_SMOKE)
		GSSmokeHandler(input[0], primID, ParticleOutputStream);
	else if (input[0].type == PT_EMBER)
		GSEmberHandler(input[0], primID, ParticleOutputStream);
	else if (input[0].type == PT_BLOCK)
		GSBlockHandler(input[0], primID, ParticleOutputStream);
	else if (input[0].type == PT_DEATHGORE)
		GSDeathHandler(input[0], primID, ParticleOutputStream);
	else if (input[0].type == PT_HOLYSHOWER)
		GSSmokeHandler(input[0], primID, ParticleOutputStream);
	else if (input[0].type == PT_EXPBURST)
		GSSmokeHandler(input[0], primID, ParticleOutputStream);
}

[maxvertexcount(4)]
void GSDrawMain(point VSParticleDrawOut input[1], inout TriangleStream<PSInput> SpriteStream)
{
	PSInput output;
	
	for (int i = 0; i < 4; i++)
	{
		float3 position = float3(gPositions[i].xy * input[0].size, 0);
		position = position + input[0].pos;
		output.pos = mul(float4(position, 1.0), gWorldViewProj);
		output.pos.z = input[0].pos.z;

		output.texIndex = input[0].texIndex;
		output.color = input[0].color;
		output.tex = input[0].texCoords*gTexCoords[i];
		SpriteStream.Append(output);
	}
	SpriteStream.RestartStrip();
}

/***********************
	PIXEL SHADERS
************************/

float4 PSDrawMain(PSInput input) : SV_Target
{
	// if the z value of input is 0, the shader will use the texture,
	// just the color will be used otherwise
	float4 zVect = float4(input.pos.z, input.pos.z, input.pos.z, input.pos.z);
	return max(zVect, gDiffuseMaps.Sample(gSamPoint, float3(input.tex, input.texIndex))) * input.color;
}

/***********************
	TECHNIQUES
************************/

GeometryShader gsStreamOut = ConstructGSWithSO(CompileShader(gs_4_0, GSUpdateParticles()), "POSITION.xyz; VELOCITY.xy; SIZE.xy; TIMER.x; EMITTER_INTERVAL.x; TYPE.x; ETYPE.x; TEXINDEX.x");
technique11 UpdateParticles
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, VSUpdateMain()));
		SetGeometryShader(gsStreamOut);
		SetPixelShader(NULL);

		SetDepthStencilState(DisableDepth, 0);
	}
}

technique11 DrawParticles
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, VSDrawMain()));
		SetGeometryShader(CompileShader(gs_4_0, GSDrawMain()));
		SetPixelShader(CompileShader(ps_4_0, PSDrawMain()));

		//SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rastCullNone);
	}
}