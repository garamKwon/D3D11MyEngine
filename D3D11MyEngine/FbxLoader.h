#pragma once
#include <fbxsdk.h>
#include "Mesh.h"

class FbxLoader
{
	FbxManager* m_pfbxManager;
	FbxImporter* m_pfbxImporter;
	FbxScene* m_pfbxScene;

	// fbxLoader 내부에서만 사용할 함수
private:
	void GetChildinfo(FbxNode* pChildNode, std::vector<CAnimateVertex> *pVertex, std::vector<UINT> *pIndex, 
		std::vector<Bone>* tempBoneHierarachy = nullptr, std::vector<XMFLOAT4X4>* tempBoneOffsets = nullptr, std::map<int, AnimationClip>* tempAnimations = nullptr );
	void GetVerticesAndIndices(FbxMesh* pMesh, std::vector<CAnimateVertex> *pVertex, std::vector<UINT> *pIndex);
	void GetInfluenceWeightAndBoneOffsets(FbxMesh *pMesh, std::vector<CAnimateVertex> *pVertices, std::vector<XMFLOAT4X4> *pBoneOffsets, std::vector<Bone>* tempBoneHierarachy);
	std::map<int, AnimationClip> LoadBoneInfomation(FbxNode* pNode);
	void LoadKeyframesByTime(FbxAnimStack *pAnimStack, FbxNode *pNode, std::vector<BoneAnimation>* pvAnimations);
	void AnimationMatchBone(std::vector<Bone>* tempBoneHierarachy, std::map<int, AnimationClip>* tempAnimations);
	void GetBoneHierarachy(FbxNode *pNode, std::vector<Bone> *pBoneHierarchy);

public:
	FbxLoader( );
	~FbxLoader( );

	bool LoadFbxMesh(const char* strMeshName, std::vector<CAnimateVertex>* vertices, std::vector<UINT>* indices, 
		std::vector<Bone>* tempBoneHierarachy = nullptr, std::vector<XMFLOAT4X4>* tempBoneOffsets = nullptr, std::map<int, AnimationClip>* tempAnimations = nullptr);
};

