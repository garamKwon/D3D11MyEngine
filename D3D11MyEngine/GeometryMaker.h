#pragma once
#include "Mesh.h"
#include "FbxLoader.h"

// 간단 메시를 만들기 위한 클래스, 사각형, 구, 원기둥,캡슐 등등
class GeometryMaker
{
public:
	GeometryMaker( );
	~GeometryMaker( );

	CStaticMesh* MakeCube( ID3D11Device* pd3dDevice, std::string strName, float fWidth = 5.0f, float fHeight = 5.0f, float fDepth = 5.0f );
	CStaticMesh* MakeCircle( ID3D11Device* pd3dDevice, std::string strName);
	CStaticMesh* MakeCylinder( ID3D11Device* pd3dDevice, std::string strName);
	CStaticMesh* MakeCapsule( ID3D11Device* pd3dDevice, std::string strName);
	CSkyboxMesh* MakeSkyBox( ID3D11Device* pd3dDevice, std::string strName, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f );
	CStaticMesh* MakeStaticMeshByFbx(ID3D11Device* pd3dDevice, FbxLoader* fbxLoader, std::string strName, const char* strFileName);
	CAnimateMesh* MakeAnimateMeshByFbx(ID3D11Device* pd3dDevice, FbxLoader* fbxLoader, std::string strName, const char* strFileName);

	void SetAverageVertexNormal( std::vector<CAnimateVertex> &vertices, std::vector<UINT>& indices, int nPrimitives, int nOffset, bool bStrip );
	XMFLOAT3 CalculateTriAngleNormal( std::vector<CAnimateVertex>& vertices, UINT nIndex0, UINT nIndex1, UINT nIndex2 );
};