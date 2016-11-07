#include "stdafx.h"
#include "GeometryMaker.h"


GeometryMaker::GeometryMaker( )
{
}


GeometryMaker::~GeometryMaker( )
{
}

CStaticMesh* GeometryMaker::MakeCube( ID3D11Device* pd3dDevice, std::string strName, float fWidth, float fHeight, float fDepth )
{
	// 정점 정보 세팅
	std::vector<CAnimateVertex> vertices;
	float fx = fWidth, fy = fHeight, fz = fDepth;

	vertices.resize( 8 );
	vertices[0].m_position = XMFLOAT3( -fx, +fy, -fz );
//	vertices[0].m_textureUV = XMFLOAT2( 1, 0 );
	vertices[1].m_position = XMFLOAT3( +fx, +fy, -fz );
//	vertices[1].m_textureUV = XMFLOAT2( 0, 0 );
	vertices[2].m_position = XMFLOAT3( +fx, +fy, +fz );
//	vertices[2].m_textureUV = XMFLOAT2( 0, 1 );
	vertices[3].m_position = XMFLOAT3( -fx, +fy, +fz );
//	vertices[3].m_textureUV = XMFLOAT2( 0, 0 );
	vertices[4].m_position = XMFLOAT3( -fx, -fy, -fz );
//	vertices[4].m_textureUV = XMFLOAT2( 1, 0 );
	vertices[5].m_position = XMFLOAT3( +fx, -fy, -fz );
//	vertices[5].m_textureUV = XMFLOAT2( 0, 0 );
	vertices[6].m_position = XMFLOAT3( +fx, -fy, +fz );
//	vertices[6].m_textureUV = XMFLOAT2( 0, 1 );
	vertices[7].m_position = XMFLOAT3( -fx, -fy, +fz );
//	vertices[7].m_textureUV = XMFLOAT2( 0, 0 );

	// 인덱스 정보 세팅
	std::vector<UINT> indices;
	indices.resize( 18 );
	indices[0] = 5; //5,6,4 - cw
	indices[1] = 6; //6,4,7 - ccw
	indices[2] = 4; //4,7,0 - cw
	indices[3] = 7; //7,0,3 - ccw
	indices[4] = 0; //0,3,1 - cw
	indices[5] = 3; //3,1,2 - ccw
	indices[6] = 1; //1,2,2 - cw 
	indices[7] = 2; //2,2,3 - ccw
	indices[8] = 2; //2,3,3 - cw  - Degenerated Index(2)
	indices[9] = 3; //3,3,7 - ccw - Degenerated Index(3)
	indices[10] = 3;//3,7,2 - cw  - Degenerated Index(3)
	indices[11] = 7;//7,2,6 - ccw
	indices[12] = 2;//2,6,1 - cw
	indices[13] = 6;//6,1,5 - ccw
	indices[14] = 1;//1,5,0 - cw
	indices[15] = 5;//5,0,4 - ccw
	indices[16] = 0;
	indices[17] = 4;

	SetAverageVertexNormal( vertices, indices, ( indices.size( ) ) ? ( indices.size( ) - 2 ) : ( vertices.size( ) - 2 ), 1, true );

	// 메시 이름 세팅 및 메시 생성
	CStaticMesh* Cube = new CStaticMesh( pd3dDevice, vertices, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, strName, ObjectLayer::LAYER_SCENE );
	return Cube;
}

CStaticMesh* GeometryMaker::MakeQuad(ID3D11Device* pd3dDevice, std::string strName, float fWidth, float fHeight)
{
	std::vector<CAnimateVertex> vertices;
	float fx = fWidth, fy = fHeight;

	vertices.resize(4);
	vertices[0].m_position = XMFLOAT3(-fx, +fy, 0);
	vertices[0].m_textureUV = XMFLOAT2(0, 0);
	vertices[1].m_position = XMFLOAT3(+fx, +fy, 0);
	vertices[1].m_textureUV = XMFLOAT2(1, 0);
	vertices[2].m_position = XMFLOAT3(+fx, -fy, 0);
	vertices[2].m_textureUV = XMFLOAT2(1, 1);
	vertices[3].m_position = XMFLOAT3(-fx, -fy, 0);
	vertices[3].m_textureUV = XMFLOAT2(0, 1);

	std::vector<UINT> indices;
	indices.resize(6);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;

	CStaticMesh* Quad = new CStaticMesh(pd3dDevice, vertices, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, strName, ObjectLayer::LAYER_SCENE);
	return Quad;
}

CStaticMesh* GeometryMaker::MakeCircle( ID3D11Device* pd3dDevice, std::string strName)
{
	// 정점 정보 세팅
	std::vector<CAnimateVertex> vertices;

	// 인덱스 정보 세팅
	std::vector<UINT> indices;

	// 메시 이름 세팅 및 메시 생성
	CStaticMesh* Circle = new CStaticMesh( pd3dDevice, vertices, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, strName, ObjectLayer::LAYER_SCENE );
	return Circle;
}

CStaticMesh* GeometryMaker::MakeCylinder( ID3D11Device* pd3dDevice, std::string strName)
{
	// 정점 정보 세팅
	std::vector<CAnimateVertex> vertices;

	// 인덱스 정보 세팅
	std::vector<UINT> indices;

	// 메시 이름 세팅 및 메시 생성
	CStaticMesh* Cylinder = new CStaticMesh( pd3dDevice, vertices, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, strName, ObjectLayer::LAYER_SCENE );
	return Cylinder;
}

CStaticMesh* GeometryMaker::MakeCapsule( ID3D11Device* pd3dDevice, std::string strName)
{
	// 정점 정보 세팅
	std::vector<CAnimateVertex> vertices;

	// 인덱스 정보 세팅
	std::vector<UINT> indices;

	// 메시 이름 세팅 및 메시 생성
	CStaticMesh* Capsule = new CStaticMesh( pd3dDevice, vertices, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, strName, ObjectLayer::LAYER_SCENE );
	return Capsule;
}

CSkyboxMesh* GeometryMaker::MakeSkyBox( ID3D11Device* pd3dDevice, std::string strName, float fWidth, float fHeight, float fDepth )
{
	std::vector<CStaticVertex> vertices;
	vertices.resize( 24 );

	int i = 0;
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
	// Front Quad 
	vertices[i].m_position = XMFLOAT3( -fx, +fx, +fx );
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( +fx, +fx, +fx );
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( +fx, -fx, +fx );
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 1.0f );
	vertices[i].m_position = XMFLOAT3( -fx, -fx, +fx );
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 1.0f );
	// Back Quad
	vertices[i].m_position = XMFLOAT3( +fx, +fx, -fx );
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( -fx, +fx, -fx );
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( -fx, -fx, -fx );
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 1.0f );
	vertices[i].m_position = XMFLOAT3( +fx, -fx, -fx );
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 1.0f );
	// Left Quad
	vertices[i].m_position = XMFLOAT3( -fx, +fx, -fx );
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( -fx, +fx, +fx );
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( -fx, -fx, +fx );
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 1.0f );
	vertices[i].m_position = XMFLOAT3( -fx, -fx, -fx );
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 1.0f );
	// Right Quad
	vertices[i].m_position = XMFLOAT3( +fx, +fx, +fx );
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( +fx, +fx, -fx );
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( +fx, -fx, -fx );
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 1.0f );
	vertices[i].m_position = XMFLOAT3( +fx, -fx, +fx );
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 1.0f );
	// Top Quad
	vertices[i].m_position = XMFLOAT3( -fx, +fx, -fx );
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( +fx, +fx, -fx );
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( +fx, +fx, +fx );
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 1.0f );
	vertices[i].m_position = XMFLOAT3( -fx, +fx, +fx );
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 1.0f );
	// Bottom Quad
	vertices[i].m_position = XMFLOAT3( -fx, -fx, +fx );	//0
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( +fx, -fx, +fx ); //1
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 1.0f );
	vertices[i].m_position = XMFLOAT3( +fx, -fx, -fx ); //2
	vertices[i++].m_textureUV = XMFLOAT2( 1.0f, 0.0f );
	vertices[i].m_position = XMFLOAT3( -fx, -fx, -fx );	//3
	vertices[i++].m_textureUV = XMFLOAT2( 0.0f, 0.0f );

	// 인덱스 정보 세팅
	std::vector<UINT> indices;
	indices.resize( 4 );

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 3;
	indices[3] = 2;

	// 메시 이름 세팅 및 메시 생성
	CSkyboxMesh* skybox = new CSkyboxMesh( pd3dDevice, vertices, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, strName, ObjectLayer::LAYER_SCENE, ObjectType::TYPE_SKYBOX );
	return skybox;
}

CStaticMesh* GeometryMaker::MakeStaticMeshByFbx(ID3D11Device* pd3dDevice, FbxLoader* fbxLoader, std::string strName, const char* strFileName)
{
	std::vector<CAnimateVertex> vertices;
	std::vector<UINT> indices;

	fbxLoader->LoadFbxMesh(strFileName, &vertices, &indices);

	CStaticMesh* pMesh = new CStaticMesh(pd3dDevice, vertices, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, strName, ObjectLayer::LAYER_SCENE);
	return pMesh;
}

CAnimateMesh* GeometryMaker::MakeAnimateMeshByFbx(ID3D11Device* pd3dDevice, FbxLoader* fbxLoader, std::string strName, const char* strFileName)
{
	std::vector<CAnimateVertex> vertices;
	std::vector<UINT> indices;
	std::vector<XMFLOAT4X4> boneOffsets;
	std::vector<Bone> bones;
	std::map<int, AnimationClip> animClips;

	fbxLoader->LoadFbxMesh(strFileName, &vertices, &indices, &bones, &boneOffsets, &animClips);

	SkinnedData animData;
	animData.Set(bones, boneOffsets, animClips);

	CAnimateMesh* pMesh = new CAnimateMesh(pd3dDevice, vertices, indices, animData, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, strName, ObjectLayer::LAYER_SCENE);
	return pMesh;
}

void GeometryMaker::SetAverageVertexNormal( std::vector<CAnimateVertex> &vertices, std::vector<UINT>& indices, int nPrimitives, int nOffset, bool bStrip )
{
	XMVECTOR normal;
	XMFLOAT3 d3dxvSumOfNormal;

	for (int j = 0; j < vertices.size(); j++)
	{
		normal = XMLoadFloat3( &XMFLOAT3( 0.0f, 0.0f, 0.0f ));
		for (int i = 0; i < nPrimitives; i++)
		{
			UINT nIndex0 = ( bStrip ) ? ( ( ( i % 2 ) == 0 ) ? ( i*nOffset + 0 ) : ( i*nOffset + 1 ) ) : ( i*nOffset + 0 );
			if (indices.size( ) != 0)
				nIndex0 = indices[nIndex0];

			UINT nIndex1 = ( bStrip ) ? ( ( ( i % 2 ) == 0 ) ? ( i*nOffset + 1 ) : ( i*nOffset + 0 ) ) : ( i*nOffset + 1 );
			if (indices.size( ) != 0) 
				nIndex1 = indices[nIndex1];

			UINT nIndex2 = ( indices.size( ) != 0 ) ? indices[i*nOffset + 2] : ( i*nOffset + 2 );
			if (( nIndex0 == j ) || ( nIndex1 == j ) || ( nIndex2 == j ))
			{
				XMVECTOR tryNormal = XMLoadFloat3( &CalculateTriAngleNormal( vertices, nIndex0, nIndex1, nIndex2 ) );
				normal = normal + tryNormal;
			}
		}

		normal = XMVector3Normalize( normal );
		XMStoreFloat3( &d3dxvSumOfNormal, normal );
		vertices[j].m_normal = d3dxvSumOfNormal;
	}
}

XMFLOAT3 GeometryMaker::CalculateTriAngleNormal( std::vector<CAnimateVertex>& vertices, UINT nIndex0, UINT nIndex1, UINT nIndex2 )
{
	XMFLOAT3 vNormal;
	XMVECTOR vP0 = XMLoadFloat3( &vertices[nIndex0].m_position );
	XMVECTOR vP1 = XMLoadFloat3( &vertices[nIndex1].m_position );
	XMVECTOR vP2 = XMLoadFloat3( &vertices[nIndex2].m_position );

	XMVECTOR vEdge1 = vP1 - vP0;
	XMVECTOR vEdge2 = vP2 - vP0;
	XMVECTOR vEdge3 = XMVector3Cross( vEdge1, vEdge2 );
	vEdge3 = XMVector3Normalize( vEdge3 );

	XMStoreFloat3( &vNormal, vEdge3 );
	return vNormal;
}

