#define MAX_LIGHTS			8 
#define DIRECTIONAL_LIGHT 0.0f
#define POINT_LIGHT 1.0f
#define SPOT_LIGHT 2.0f

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES

struct MATERIAL
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular; //a = power
	float4 m_cEmissive;
};

struct LIGHT
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;
	float3 m_vPosition;
	float m_fRange;
	float3 m_vDirection;
	float m_nType;
	float3 m_vAttenuation;
	float m_fFalloff;
	float m_fTheta; //cos(m_fTheta)
	float m_fPhi; //cos(m_fPhi)
	float m_bEnable;
	float padding;
};

float4 g_vDirLight = ( float4 )0.f;

struct LIGHTEDCOLOR
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;
};

cbuffer cbTransformMatrix : register( b0 )
{
	matrix gmtxWorld : packoffset( c0 );
	matrix gmtxView : packoffset( c4 );
	matrix gmtxProjection : packoffset( c8 );
}

cbuffer cbLights : register( b1 )
{
	LIGHT gLights[MAX_LIGHTS];
	float4 gcLightGlobalAmbient;
	float4 gvCameraPosition;

	float m_fogStart;
	float m_fogRange;
	float4 m_fogColor;
	float2 padding;
};

cbuffer cbMaterial : register( b2 )
{
	MATERIAL gMaterial;
}

cbuffer cbSkinned : register(b3)
{
	// 한 케릭터의 최대 뼈의 개수 96개
	matrix gBoneTransforms[96];
};


Texture2D gtexDiffuse : register( t0 );
SamplerState gssDiffuse : register( s0 );

struct VS_INPUT
{
	float3 positionL : POSITION;
	float2 uvTexCoord : TEXCOORD0;
	float3 normalL : NORMAL;
	float3 binormalL : BINORMAL;
	float3 tangentL : TANGENT;
};

struct VS_OUTPUT
{
	float3 positionW : POSITION;
	float4 positionH : SV_POSITION;
	float2 uvTexCoord : TEXCOORD0;
	float3 normalW : NORMAL;
	float3 binormalW : BINORMAL;
	float3 tangentW : TANGENT;
};

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

struct SkinnedVertexIn
{
	float3 positionL : POSITION;
	float2 uvTexCoord : TEXCOORD0;
	float3 normalL : NORMAL;
	float3 binormalL : BINORMAL;
	float3 tangentL : TANGENT;
	float3 weights : WEIGHTS;
	float4 boneIndices : BONEINDICES;
};

struct SkinnedVertexOut
{
	float3 positionW : POSITION;
	float4 positionH : SV_POSITION;
	float2 uvTexCoord : TEXCOORD0;
	float3 normalW : NORMAL;
	float4 binormalW : BINORMAL;
	float4 tangentW : TANGENT;
	float4 color : TEXCOORD1;
};

VS_TEXTURED_OUTPUT VSTexturedColor( VS_TEXTURED_INPUT input )
{
	VS_TEXTURED_OUTPUT output = (VS_TEXTURED_OUTPUT)0;
	output.position = mul( mul( mul( float4( input.position, 1.0f ), gmtxWorld ), gmtxView ), gmtxProjection );
	output.texCoord = input.texCoord;

	return( output );
}

float4 PSTexturedColor( VS_TEXTURED_OUTPUT input ) : SV_Target
{
	float4 cColor = gtexDiffuse.Sample( gssDiffuse, input.texCoord );

	clip( cColor.a - 0.1f );
	return( cColor );
}

LIGHTEDCOLOR DirectionalLight( int i, float3 vNormal, float3 vToCamera )
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = -gLights[i].m_vDirection;
		float fDiffuseFactor = dot( vToLight, vNormal );
	if (fDiffuseFactor > 0.0f)
	{
		if (gMaterial.m_cSpecular.a != 0.0f)
		{
#ifdef _WITH_REFLECT
			float3 vReflect = reflect( -vToLight, vNormal );
				float fSpecularFactor = pow( max( dot( vReflect, vToCamera ), 0.0f ), gMaterial.m_cSpecular.a );
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
			float3 vHalf = normalize( vToCamera + vToLight );
#else
			float3 vHalf = float3( 0.0f, 1.0f, 0.0f );
#endif
				float fSpecularFactor = pow( max( dot( vHalf, vNormal ), 0.0f ), gMaterial.m_cSpecular.a );
#endif
			output.m_cSpecular = gMaterial.m_cSpecular * ( gLights[i].m_cSpecular * fSpecularFactor );
		}
		output.m_cDiffuse = gMaterial.m_cDiffuse * ( gLights[i].m_cDiffuse * fDiffuseFactor );
	}
	output.m_cAmbient = gMaterial.m_cAmbient * gLights[i].m_cAmbient;
	return( output );
}

LIGHTEDCOLOR PointLight( int i, float3 vPosition, float3 vNormal, float3 vToCamera )
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = gLights[i].m_vPosition - vPosition;
		float fDistance = length( vToLight );
	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;
		float fDiffuseFactor = dot( vToLight, vNormal );
		if (fDiffuseFactor > 0.0f)
		{
			if (gMaterial.m_cSpecular.a != 0.0f)
			{
#ifdef _WITH_REFLECT
				float3 vReflect = reflect( -vToLight, vNormal );
					float fSpecularFactor = pow( max( dot( vReflect, vToCamera ), 0.0f ), gMaterial.m_cSpecular.a );
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
				float3 vHalf = normalize( vToCamera + vToLight );
#else
				float3 vHalf = float3( 0.0f, 1.0f, 0.0f );
#endif
					float fSpecularFactor = pow( max( dot( vHalf, vNormal ), 0.0f ), gMaterial.m_cSpecular.a );
#endif
				output.m_cSpecular = gMaterial.m_cSpecular * ( gLights[i].m_cSpecular * fSpecularFactor );
			}
			output.m_cDiffuse = gMaterial.m_cDiffuse * ( gLights[i].m_cDiffuse * fDiffuseFactor );
		}
		float fAttenuationFactor = 1.0f / dot( gLights[i].m_vAttenuation, float3( 1.0f, fDistance, fDistance*fDistance ) );
		output.m_cAmbient = gMaterial.m_cAmbient * ( gLights[i].m_cAmbient * fAttenuationFactor );
		output.m_cDiffuse *= fAttenuationFactor;
		output.m_cSpecular *= fAttenuationFactor;
	}
	return( output );
}

LIGHTEDCOLOR SpotLight( int i, float3 vPosition, float3 vNormal, float3 vToCamera )
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;
	float3 vToLight = gLights[i].m_vPosition - vPosition;
		float fDistance = length( vToLight );
	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;
		float fDiffuseFactor = dot( vToLight, vNormal );
		if (fDiffuseFactor > 0.0f)
		{
			if (gMaterial.m_cSpecular.a != 0.0f)
			{
#ifdef _WITH_REFLECT
				float3 vReflect = reflect( -vToLight, vNormal );
					float fSpecularFactor = pow( max( dot( vReflect, vToCamera ), 0.0f ), gMaterial.m_cSpecular.a );
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
				float3 vHalf = normalize( vToCamera + vToLight );
#else
				float3 vHalf = float3( 0.0f, 1.0f, 0.0f );
#endif
					float fSpecularFactor = pow( max( dot( vHalf, vNormal ), 0.0f ), gMaterial.m_cSpecular.a );
#endif
				output.m_cSpecular = gMaterial.m_cSpecular * ( gLights[i].m_cSpecular * fSpecularFactor );
			}
			output.m_cDiffuse = gMaterial.m_cDiffuse * ( gLights[i].m_cDiffuse * fDiffuseFactor );
		}
#ifdef _WITH_THETA_PHI_CONES
		float fAlpha = max( dot( -vToLight, gLights[i].m_vDirection ), 0.0f );
		float fSpotFactor = pow( max( ( ( fAlpha - gLights[i].m_fPhi ) / ( gLights[i].m_fTheta - gLights[i].m_fPhi ) ), 0.0f ), gLights[i].m_fFalloff );
#else
		float fSpotFactor = pow( max( dot( -vToLight, gLights[i].m_vDirection ), 0.0f ), gLights[i].m_fFalloff );
#endif
		float fAttenuationFactor = 1.0f / dot( gLights[i].m_vAttenuation, float3( 1.0f, fDistance, fDistance*fDistance ) );
		output.m_cAmbient = gMaterial.m_cAmbient * ( gLights[i].m_cAmbient * fAttenuationFactor * fSpotFactor );
		output.m_cDiffuse *= fAttenuationFactor * fSpotFactor;
		output.m_cSpecular *= fAttenuationFactor * fSpotFactor;
	}
	return( output );
}

float4 Lighting( float3 vPosition, float3 vNormal )
{
	int i;
	float3 vCameraPosition = float3( gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z );
		float3 vToCamera = normalize( vCameraPosition - vPosition );
		LIGHTEDCOLOR LightedColor = (LIGHTEDCOLOR)0;
	float4 cColor = float4( 0.0f, 0.0f, 0.0f, 0.0f );
		for (i = 0; i < MAX_LIGHTS; i++)
		{
			if (gLights[i].m_bEnable == 1.0f)
			{
				if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
				{
					LightedColor = DirectionalLight( i, vNormal, vToCamera );
					cColor += ( LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular );
				}
				if (gLights[i].m_nType == POINT_LIGHT)
				{
					LightedColor = PointLight( i, vPosition, vNormal, vToCamera );
					cColor += ( LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular );
				}
				if (gLights[i].m_nType == SPOT_LIGHT)
				{
					LightedColor = SpotLight( i, vPosition, vNormal, vToCamera );
					cColor += ( LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular );
				}
			}
		}
	cColor += ( gcLightGlobalAmbient * gMaterial.m_cAmbient );
	cColor.a = gMaterial.m_cDiffuse.a;
	return( cColor );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
VS_OUTPUT VSmain( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.positionW = mul( input.positionL, ( float3x3 )gmtxWorld );
	output.positionH = mul( mul( float4( output.positionW, 1.0f ), gmtxView ), gmtxProjection );
	output.normalW = mul( input.normalL, ( float3x3 )gmtxWorld );
	output.tangentW = mul( input.tangentL, ( float3x3 )gmtxWorld );
	output.binormalW = mul( input.binormalL, ( float3x3 )gmtxWorld );

	return (output);
}

float4 PSmain( VS_OUTPUT input ) : SV_TARGET
{
	float3 N = normalize( input.normalW );

	float4 cIllumination = Lighting( input.positionW, N );
	float4 cColor = gtexDiffuse.Sample( gssDiffuse, input.uvTexCoord );

	return cIllumination * cColor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
SkinnedVertexOut SkinnedVS(SkinnedVertexIn vin)
{
	SkinnedVertexOut vout = (SkinnedVertexOut)0;

	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vin.weights.x;
	weights[1] = vin.weights.y;
	weights[2] = vin.weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = (float3)0;
	float3 normalL = (float3)0;
	float3 tangentL = (float3)0;
	float3 binormalL = (float3)0;

	vout.color.x = 0.0;
	vout.color.y = 0.0;
	vout.color.z = 0.0;
	vout.color.w = 1.0;

	if (vin.boneIndices[0] == 7 && vin.boneIndices[1] == 0)
		vout.color.x = 255.0;

	if (vin.boneIndices[0] == 7 && vin.boneIndices[1] == 6)
		vout.color.y = 255.0;

	if (vin.boneIndices[0] == 7 && vin.boneIndices[1] == 20)
		vout.color.z = 255.0;

	if (weights[0] == 0)
		posL = vin.positionL;
	else {
		for (int i = 0; i < 4; ++i)
		{
			posL += weights[i] * mul(float4(vin.positionL, 1.0f), gBoneTransforms[vin.boneIndices[i]]).xyz;
			normalL += weights[i] * mul(vin.normalL, (float3x3)gBoneTransforms[vin.boneIndices[i]]);
			tangentL += weights[i] * mul(vin.tangentL, (float3x3)gBoneTransforms[vin.boneIndices[i]]);
			binormalL += weights[i] * mul(vin.binormalL, (float3x3)gBoneTransforms[vin.boneIndices[i]]);
		}
	}
	vout.positionW = mul(float4(posL, 1.0f), gmtxWorld).xyz;
//	vout.normalW = mul(float4(normalL, 1.0f), gmtxWorld).xyz;
	vout.normalW = float4(mul(normalL, (float3x3)gmtxWorld), 0.0f);
	vout.tangentW = float4(mul(tangentL, (float3x3)gmtxWorld), 0.0f);
	vout.binormalW = float4(mul(binormalL, (float3x3)gmtxWorld), 0.0f);

	vout.positionH = mul(float4(posL, 1.0f), gmtxWorld);
	vout.positionH = mul(vout.positionH, gmtxView);
	vout.positionH = mul(vout.positionH, gmtxProjection);

	vout.uvTexCoord = vin.uvTexCoord;

	return vout;
}

float4 SkinnedPS(SkinnedVertexOut input) : SV_Target
{
	float3 N = normalize(input.normalW);

	float4 cIllumination = Lighting(input.positionW, N);
	float4 cColor = gtexDiffuse.Sample(gssDiffuse, input.uvTexCoord);

	return cIllumination * cColor;
	//return input.color;
}