#include "stdafx.h"
#include "FbxLoader.h"


FbxLoader::FbxLoader()
{
	m_pfbxManager = FbxManager::Create();
	FbxIOSettings* pfbxIOSettings = FbxIOSettings::Create(m_pfbxManager, IOSROOT);
	m_pfbxManager->SetIOSettings(pfbxIOSettings);

	m_pfbxImporter = FbxImporter::Create(m_pfbxManager, "");
	m_pfbxScene = FbxScene::Create(m_pfbxManager, "");
}

FbxLoader::~FbxLoader()
{
	m_pfbxImporter->Destroy();
	m_pfbxManager->Destroy();
}

// CAnimateMesh 는 CStaticMesh에 대한 정보도 포함하기 때문에 CAnimateMesh에 정점 정보를 저장하여 가져온다
bool FbxLoader::LoadFbxMesh(const char* strMeshName, std::vector<CAnimateVertex>* vertices, std::vector<UINT>* indices,
	std::vector<Bone>* tempBoneHierarachy, std::vector<XMFLOAT4X4>* tempBoneOffsets, std::map<int, AnimationClip>* tempAnimations)
{
	// 파일 이름을 로딩
	bool bResult = m_pfbxImporter->Initialize(strMeshName, -1, m_pfbxManager->GetIOSettings());
	if (!bResult)
	{
#ifdef __DEBUG_MODE__
		std::cout << "Fail find '" << strMeshName << "'" << std::endl;
#endif	
		return false;
	}
	// Scene에 추가, Scene의 루트노드의 하위로 들어가게됨
	bResult = m_pfbxImporter->Import(m_pfbxScene);

	if (!bResult)
		return false;

	// 루트노드가 없으면 함수 종료
	FbxNode *pfbxRootNode = m_pfbxScene->GetRootNode();

	if (pfbxRootNode)
	{
		int ChildCount = m_pfbxScene->GetRootNode()->GetChildCount();

		// 자식들의 데이터 로딩
		for (int i = 0; i < ChildCount; i++)
		{
			// 자식노드들을 순차적으로 탐색
			GetChildinfo(pfbxRootNode->GetChild(i), vertices, indices, tempBoneHierarachy, tempBoneOffsets, tempAnimations);
			AnimationMatchBone(tempBoneHierarachy, tempAnimations);
		}

		// 노드들 제거
		for (int i = 0; i < m_pfbxScene->GetNodeCount(); i++)
		{
			FbxNode* temp = m_pfbxScene->GetNode(i);
			m_pfbxScene->RemoveNode(temp);
		}
		return true;
	}
	return false;
}

void FbxLoader::GetChildinfo(FbxNode* pChildNode, std::vector<CAnimateVertex> *pVertex, std::vector<UINT> *pIndex,
	std::vector<Bone>* tempBoneHierarachy, std::vector<XMFLOAT4X4>* tempBoneOffsets, std::map<int, AnimationClip>* tempAnimations)
{
	FbxNodeAttribute *nodeAttribute = pChildNode->GetNodeAttributeByIndex(0);
	if (nodeAttribute == NULL)
		return;

	FbxNodeAttribute::EType attributeType = nodeAttribute->GetAttributeType();
	FbxMesh* pMesh = (FbxMesh*)nodeAttribute;

	switch (attributeType)
	{
		case FbxNodeAttribute::eMesh:
		{
			// 메시 정점, 인덱스 정보 가져오기
			GetVerticesAndIndices(pMesh, pVertex, pIndex);
			// 영향 받는 본, 본오프셋, 본 계층구조 가져오기
			GetInfluenceWeightAndBoneOffsets(pMesh, pVertex, tempBoneOffsets, tempBoneHierarachy);
			break;
		}
		case FbxNodeAttribute::eSkeleton:
		{
			// 애니메이션 정보 가져오기, 본계층구조, 영향받는 본, 가중치, 프레임별 변환행렬 가져와야함
			*tempAnimations = LoadBoneInfomation(pChildNode);
			break;
		}
	}
}

std::map<int, AnimationClip> FbxLoader::LoadBoneInfomation(FbxNode* pNode)
{
	bool isAnimated = false;
	std::map<int, AnimationClip> tempAnimations;
	AnimationClip animation;
	std::string animName;

	// 애니메이션클립을 찾는 행위
	int numAnimations = m_pfbxScene->GetSrcObjectCount<FbxAnimStack>();
	for (int animIndex = 0; animIndex < numAnimations; animIndex++)
	{
		FbxAnimStack* animStack = m_pfbxScene->GetSrcObject<FbxAnimStack>(animIndex);
		animName = animStack->GetName();	// 이름

		LoadKeyframesByTime(animStack, pNode, &(animation.BoneAnimations));

		if (animation.BoneAnimations.size() != 0) {
			tempAnimations.insert({ animIndex, animation });
			animation.BoneAnimations.clear();
		}
	}	// end of animation for
	return tempAnimations;
}

void FbxLoader::LoadKeyframesByTime(FbxAnimStack *pAnimStack, FbxNode *pNode, std::vector<BoneAnimation>* pvAnimations)
{
	// 이미 정보를 가져온 적이 있으면 넘어간다
	for (int i = 0; i < pvAnimations->size(); i++)
	{
		if (pNode->GetName() == (*pvAnimations)[i].BoneName)
			return;
	}

	int stacks = pAnimStack->GetMemberCount<FbxAnimLayer>();
	for (int i = 0; i < stacks; i++)
	{
		FbxAnimLayer *pfbxAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(i);
		FbxAnimCurve *pfbxAnimCurve = pNode->LclRotation.GetCurve(pfbxAnimLayer, "X");

		if (!pfbxAnimCurve)
			continue;
		long long mtxSize = pfbxAnimCurve->KeyGetCount();

		if (mtxSize != 0)
		{
			std::vector<Keyframe> _mtx(mtxSize);
			BoneAnimation tempBoneAnim;

			float StartTime = (float)pfbxAnimCurve->KeyGetTime(0).GetSecondDouble();
			for (long long i = 0; i < mtxSize; i++)
			{
				FbxTime nTime = pfbxAnimCurve->KeyGetTime(i);

				if (pNode)
				{
					// 해당 본의 월드좌표계에서의 행렬
					_mtx[i].TimePos = (float)nTime.GetSecondDouble() - StartTime;		// 시간저장

					FbxAMatrix aniMatrix = pNode->EvaluateLocalTransform(nTime);
					FbxQuaternion Q = aniMatrix.GetQ();
					FbxVector4 S = aniMatrix.GetS();
					FbxVector4 T = aniMatrix.GetT();

					_mtx[i].RotationQuat.x = (float)Q[0]; _mtx[i].RotationQuat.y = (float)Q[1]; _mtx[i].RotationQuat.z = (float)Q[2]; _mtx[i].RotationQuat.w = (float)Q[3];
					_mtx[i].Scale.x = (float)S[0]; _mtx[i].Scale.y = (float)S[1]; _mtx[i].Scale.z = (float)S[2];
					_mtx[i].Translation.x = (float)T[0]; _mtx[i].Translation.y = (float)T[1]; _mtx[i].Translation.z = (float)T[2];
				}
			}

			// 한 순간의 애니메이션 매트릭스
			tempBoneAnim.Keyframes = _mtx;
			tempBoneAnim.BoneName = pNode->GetName();
			tempBoneAnim.PerentBoneName = pNode->GetParent()->GetName();

			pvAnimations->push_back(tempBoneAnim);
		}
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
		LoadKeyframesByTime(pAnimStack, pNode->GetChild(i), pvAnimations);
}

void FbxLoader::AnimationMatchBone(std::vector<Bone>* tempBoneHierarachy, std::map<int, AnimationClip>* tempAnimations)
{
	// 본계층구조나 애니메이션이 비었으면 함수 종료
	if (!tempBoneHierarachy || !tempAnimations)
		return;
	if (tempBoneHierarachy->empty() || tempAnimations->empty())
		return;

	for (int i = 0; i < tempBoneHierarachy->size(); i++)
	{
		for (int j = 0; j < tempAnimations->size(); j++)
		{
			for (int k = 0; k < (*tempAnimations)[j].BoneAnimations.size(); k++)
			{
				if ((*tempBoneHierarachy)[i].boneName == (*tempAnimations)[j].BoneAnimations[k].BoneName)
					(*tempAnimations)[j].BoneAnimations[k].BoneIndex = i;
			}
		}
	}
}

void FbxLoader::GetInfluenceWeightAndBoneOffsets(FbxMesh *pMesh, std::vector<CAnimateVertex> *pVertices, std::vector<XMFLOAT4X4> *pBoneOffsets, std::vector<Bone>* tempBoneHierarachy)
{
	// 본 계층구조, 오프셋변환, 가중치, 영향받는 정점 을 구해야함
	int numDeformers = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	for (int deformerCount = 0; deformerCount < numDeformers; deformerCount++)
	{
		FbxSkin *skin = (FbxSkin*)pMesh->GetDeformer(deformerCount, FbxDeformer::eSkin);
	
		if (!skin)
			continue;

		int boneCount = skin->GetClusterCount();		// 본의 개수	
		for (int i = 0; i < boneCount; i++)
		{
			FbxCluster *cluster = skin->GetCluster(i);
			FbxNode *bone = cluster->GetLink();
			if (strcmp(bone->GetParent()->GetName(), "RootNode") == 0)
			{
				GetBoneHierarachy(bone, tempBoneHierarachy);
				break;
			}
		}
		XMFLOAT4X4 identity;
		XMStoreFloat4x4(&identity, XMMatrixIdentity());
		pBoneOffsets->resize(tempBoneHierarachy->size(), identity);

		// 본의 정보를 얻어오는 행위
		for (int boneIndex = 0; boneIndex < boneCount; boneIndex++)
		{
			// cluster == bone
			FbxCluster *cluster = skin->GetCluster(boneIndex);
			FbxNode *bone = cluster->GetLink();		// 본과 연결되어있는 노드, 부모노드가 아님

			int boneIdx = -1;
			for (int i = 0; i < tempBoneHierarachy->size(); i++)
			{
				if (bone->GetName() == tempBoneHierarachy->data()[i].boneName)
				{
					boneIdx = i;
					break;
				}
			}

			if (boneIdx == -1)
				continue;

			FbxAMatrix LinkBoneMatrix;
			FbxAMatrix TransboneMatrix;
			FbxAMatrix ResultMatrix;

			cluster->GetTransformLinkMatrix(LinkBoneMatrix);
			cluster->GetTransformMatrix(TransboneMatrix);
			ResultMatrix = LinkBoneMatrix.Inverse()  *TransboneMatrix;

			XMFLOAT4X4 tempOffsetMatl;
			tempOffsetMatl._11 = ResultMatrix.mData[0].mData[0]; tempOffsetMatl._12 = ResultMatrix.mData[0].mData[1];
			tempOffsetMatl._13 = ResultMatrix.mData[0].mData[2]; tempOffsetMatl._14 = ResultMatrix.mData[0].mData[3];
			tempOffsetMatl._21 = ResultMatrix.mData[1].mData[0]; tempOffsetMatl._22 = ResultMatrix.mData[1].mData[1];
			tempOffsetMatl._23 = ResultMatrix.mData[1].mData[2]; tempOffsetMatl._24 = ResultMatrix.mData[1].mData[3];
			tempOffsetMatl._31 = ResultMatrix.mData[2].mData[0]; tempOffsetMatl._32 = ResultMatrix.mData[2].mData[1];
			tempOffsetMatl._33 = ResultMatrix.mData[2].mData[2]; tempOffsetMatl._34 = ResultMatrix.mData[2].mData[3];
			tempOffsetMatl._41 = ResultMatrix.mData[3].mData[0]; tempOffsetMatl._42 = ResultMatrix.mData[3].mData[1];
			tempOffsetMatl._43 = ResultMatrix.mData[3].mData[2]; tempOffsetMatl._44 = ResultMatrix.mData[3].mData[3];

			(*pBoneOffsets)[boneIdx] = tempOffsetMatl;

			double *boneVertexWeights = cluster->GetControlPointWeights();				// 해당 본에 의한 정점의 가중치
			int *boneVertexIndices = cluster->GetControlPointIndices();					// 해당 본에 영향을 받는 정점들
																						// 해당 본에 영향을 받는 모든 정점을 하나씩 가져옴	
			int numBoneVertexIndices = cluster->GetControlPointIndicesCount();
			
			for (int i = 0; i < numBoneVertexIndices; i++)
			{
				float boneWeight = (float)boneVertexWeights[i];
				int idx = boneVertexIndices[i];

				// 가중치 중 x가 0이면 첫번째 인덱스
				if ((*pVertices)[idx].m_weights.x == 0)
				{
					(*pVertices)[idx].m_weights.x = boneWeight;
					(*pVertices)[idx].m_boneIndices.x = (float)boneIdx;
				}
				// 가중치 중 x가 0이 아니고 y가 0이면 두번째 인덱스
				else if ((*pVertices)[idx].m_weights.x != 0 && (*pVertices)[idx].m_weights.y == 0)
				{
					(*pVertices)[idx].m_weights.y = boneWeight;
					(*pVertices)[idx].m_boneIndices.y = (float)boneIdx;
				}
				// 가중치 중 x가 0이 아니고 y가 0이 아니고 z가 0이면 세번째 인덱스
				else if ((*pVertices)[idx].m_weights.x != 0 && (*pVertices)[idx].m_weights.y != 0 && (*pVertices)[idx].m_weights.z == 0)
				{
					(*pVertices)[idx].m_weights.z = boneWeight;
					(*pVertices)[idx].m_boneIndices.z = (float)boneIdx;
				}
				// 모두 0이 아니면 4번째 인덱스, 가중치는 1에서 xyz 빼면 나머지 구할 수 있음
				else if ((*pVertices)[idx].m_weights.x != 0 && (*pVertices)[idx].m_weights.y != 0 && (*pVertices)[idx].m_weights.z != 0)
				{
					(*pVertices)[idx].m_boneIndices.w = (float)boneIdx;
				}
			}
		}	// end of bonecount for

		// uv 중복 좌표의 가중치와 영향받는 본 추가
		int vertexCount = pMesh->GetControlPointsCount();
		for (int i = vertexCount; i < pVertices->size(); i++)
		{
			if ((*pVertices)[i].m_weights.x == 0)
			{
				for (int j = 0; j < vertexCount; j++)
				{
					if ((*pVertices)[i].m_position.x == (*pVertices)[j].m_position.x &&
						(*pVertices)[i].m_position.y == (*pVertices)[j].m_position.y &&
						(*pVertices)[i].m_position.z == (*pVertices)[j].m_position.z)
					{
						(*pVertices)[i].m_boneIndices = (*pVertices)[j].m_boneIndices;
						(*pVertices)[i].m_weights = (*pVertices)[j].m_weights;
						break;
					}
				}
			}
		}
	}
}

void FbxLoader::GetBoneHierarachy(FbxNode *pNode, std::vector<Bone> *pBoneHierarchy)
{
	Bone tempBone;

	int parentIdx = 0;

	if (pBoneHierarchy->size() == 0)
	{
		tempBone.boneName = pNode->GetName();
		tempBone.parentBoneName = pNode->GetParent()->GetName();
		tempBone.parentIdx = parentIdx;
		pBoneHierarchy->push_back(tempBone);
	}
	
	for (int i = 0; i < pBoneHierarchy->size(); i++)
	{
		if ((*pBoneHierarchy)[i].boneName == pNode->GetName())
		{
			parentIdx = i;
			break;
		}
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		tempBone.boneName = pNode->GetChild(i)->GetName();
		tempBone.parentBoneName = pNode->GetName();
		tempBone.parentIdx = parentIdx;
		pBoneHierarchy->push_back(tempBone);

		GetBoneHierarachy(pNode->GetChild(i), pBoneHierarchy);
	}
}	

void FbxLoader::GetVerticesAndIndices(FbxMesh* pMesh, std::vector<CAnimateVertex> *pVertex, std::vector<UINT> *pIndex)
{
	// ControlPoints ( 정점 )
	FbxVector4* pVertices = pMesh->GetControlPoints();
	FbxVector4 fbxNormal;
	FbxVector4 fbxTangent;
	FbxVector4 fbxBinormal;
	FbxVector2 uv;

	int vertexCount = pMesh->GetControlPointsCount();
	pVertex->resize(vertexCount);

	// polygons
	int countPolys = pMesh->GetPolygonCount();

	int vertexID = 0;
	for (int i = 0; i < countPolys; i++)
	{
		int countPolyVerts = pMesh->GetPolygonSize(i);

		//for (int j = countPolyVerts - 1; j >= 0; j--)
		for(int j=0; j<countPolyVerts; j++)
		{
			CAnimateVertex vertex;

			int cp_index = pMesh->GetPolygonVertex(i, j);
		
			//---------------------------------------------------------
			// position
			//---------------------------------------------------------
			vertex.m_position.x = (float)pVertices[cp_index].mData[0];
			vertex.m_position.y = (float)pVertices[cp_index].mData[1];
			vertex.m_position.z = (float)pVertices[cp_index].mData[2];

			//---------------------------------------------------------
			// normal
			//---------------------------------------------------------
			FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(0);
			if (leNormal == nullptr)
			{
#ifdef __DEBUG_MODE__
				std::cout << "Don't have Normal info" << std::endl;
#endif	
			}
			else
			{
				FbxLayerElement::EMappingMode normals_MapMode = leNormal->GetMappingMode();
				if (normals_MapMode == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
						case FbxGeometryElement::eDirect:
							fbxNormal = leNormal->GetDirectArray().GetAt(vertexID);
							break;
						case FbxGeometryElement::eIndexToDirect:
						{
							int id = leNormal->GetIndexArray().GetAt(vertexID);
							fbxNormal = leNormal->GetDirectArray().GetAt(id);
						}
						break;
						default:
							break;
					}
				}
				else if (normals_MapMode == FbxGeometryElement::eByControlPoint)
				{
					switch (leNormal->GetReferenceMode())
					{
						case FbxGeometryElement::eDirect:
							fbxNormal = leNormal->GetDirectArray().GetAt(cp_index);
							break;
						case FbxGeometryElement::eIndexToDirect:
						{
							int id = leNormal->GetIndexArray().GetAt(cp_index);
							fbxNormal = leNormal->GetDirectArray().GetAt(id);
						}
						break;
						default:
							break;
					}
				}
				vertex.m_normal.x = fbxNormal.mData[0];
				vertex.m_normal.y = fbxNormal.mData[1];
				vertex.m_normal.z = fbxNormal.mData[2];
			}

			//---------------------------------------------------------
			// uv
			//---------------------------------------------------------
			FbxGeometryElementUV* leUV = pMesh->GetElementUV(0);
			if (leUV == nullptr)
			{
#ifdef __DEBUG_MODE__
				std::cout << "Don't have UV info" << std::endl;
#endif	
			}
			else
			{
				switch (leUV->GetMappingMode())
				{
					case FbxGeometryElement::eByControlPoint:
					{
						switch (leUV->GetReferenceMode())
						{
							case FbxGeometryElement::eDirect:
								uv = leUV->GetDirectArray().GetAt(cp_index);
								break;
							case FbxGeometryElement::eIndexToDirect:
							{
								int id = leUV->GetIndexArray().GetAt(cp_index);
								uv = leUV->GetDirectArray().GetAt(id);
							}
							break;
							default:
								break;
						}
					}
					break;

					case FbxGeometryElement::eByPolygonVertex:
					{
						int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
						switch (leUV->GetReferenceMode())
						{
							case FbxGeometryElement::eDirect:
							case FbxGeometryElement::eIndexToDirect:
							{
								uv = leUV->GetDirectArray().GetAt(lTextureUVIndex);
							}
							break;
							default:
								break;
						}
					}
					default:
						break;
				}
				vertex.m_textureUV.x = (float)uv.mData[0];
				vertex.m_textureUV.y = 1.0f - (float)uv.mData[1];
			}

//			//---------------------------------------------------------
//			// tangent
//			//---------------------------------------------------------
//			FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(0);
//			if (leTangent == nullptr)
//			{
//#ifdef __DEBUG_MODE__
//				std::cout << "Don't have Tangent info" << std::endl;
//#endif	
//			}
//			else
//			{
//				FbxLayerElement::EMappingMode tangent_MapMode = leTangent->GetMappingMode();
//				if (tangent_MapMode == FbxGeometryElement::eByPolygonVertex)
//				{
//					switch (leTangent->GetReferenceMode())
//					{
//						case FbxGeometryElement::eDirect:
//							fbxTangent = leTangent->GetDirectArray().GetAt(vertexID);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = leTangent->GetIndexArray().GetAt(vertexID);
//							fbxTangent = leTangent->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//							break;
//					}
//				}
//				else if (tangent_MapMode == FbxGeometryElement::eByControlPoint)
//				{
//					switch (leTangent->GetReferenceMode())
//					{
//						case FbxGeometryElement::eDirect:
//							fbxTangent = leTangent->GetDirectArray().GetAt(cp_index);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = leTangent->GetIndexArray().GetAt(cp_index);
//							fbxTangent = leTangent->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//							break;
//					}
//				}
//				vertex.m_tangent.x = fbxTangent.mData[0];
//				vertex.m_tangent.y = fbxTangent.mData[1];
//				vertex.m_tangent.z = fbxTangent.mData[2];
//			}
//
//			//---------------------------------------------------------
//			// binormal
//			//---------------------------------------------------------
//			FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal(0);
//			if (leBinormal == nullptr)
//			{
//#ifdef __DEBUG_MODE__
//				std::cout << "Don't have Binormal info" << std::endl;
//#endif	
//			}
//			else
//			{
//				FbxLayerElement::EMappingMode binormal_MapMode = leTangent->GetMappingMode();
//				if (binormal_MapMode == FbxGeometryElement::eByPolygonVertex)
//				{
//					switch (leBinormal->GetReferenceMode())
//					{
//						case FbxGeometryElement::eDirect:
//							fbxBinormal = leBinormal->GetDirectArray().GetAt(vertexID);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = leBinormal->GetIndexArray().GetAt(vertexID);
//							fbxBinormal = leBinormal->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//							break;
//					}
//				}
//				else if (binormal_MapMode == FbxGeometryElement::eByControlPoint)
//				{
//					switch (leBinormal->GetReferenceMode())
//					{
//						case FbxGeometryElement::eDirect:
//							fbxBinormal = leBinormal->GetDirectArray().GetAt(cp_index);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = leBinormal->GetIndexArray().GetAt(cp_index);
//							fbxBinormal = leBinormal->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//							break;
//					}
//				}
//				vertex.m_binormal.x = fbxBinormal.mData[0];
//				vertex.m_binormal.y = fbxBinormal.mData[1];
//				vertex.m_binormal.z = fbxBinormal.mData[2];
//			}
			++vertexID;

			if ((*pVertex)[cp_index].m_position.x == vertex.m_position.x &&
				(*pVertex)[cp_index].m_position.y == vertex.m_position.y &&
				(*pVertex)[cp_index].m_position.z == vertex.m_position.z &&
				(*pVertex)[cp_index].m_textureUV.x != vertex.m_textureUV.x
				&& (*pVertex)[cp_index].m_textureUV.y != vertex.m_textureUV.y )
			{
				cp_index = pVertex->size();
				pVertex->push_back(vertex);
			}
			else
				(*pVertex)[cp_index] = vertex;

			pIndex->push_back(cp_index);
		}
	}
}
