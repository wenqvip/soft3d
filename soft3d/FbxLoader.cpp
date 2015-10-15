
#pragma comment(lib, "libfbxsdk.lib")
#include "FbxLoader.h"

namespace soft3d
{

	FbxLoader::FbxLoader()
	{
	}


	FbxLoader::~FbxLoader()
	{
		delete[] m_indexBuffer;
		delete[] m_vertexBuffer;
		delete[] m_normalBuffer;
	}


	int FbxLoader::LoadFbx(const char* fbxName)
	{
		FbxManager* lSdkManager = FbxManager::Create();
		FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);
		FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

		if (!lImporter->Initialize(fbxName, -1, lSdkManager->GetIOSettings())) {
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
			return -1;
		}

		FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
		lImporter->Import(lScene);
		lImporter->Destroy();

		FbxNode* lRootNode = lScene->GetRootNode();
		if (lRootNode) {
			for (int i = 0; i < lRootNode->GetChildCount(); i++)
				LoadNode(lRootNode->GetChild(i));
		}
		lSdkManager->Destroy();
		return 0;
	}

	void FbxLoader::LoadNode(FbxNode* pNode)
	{
		for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
			LoadAttribute(pNode->GetNodeAttributeByIndex(i));

		// Recursively print the children.
		for (int j = 0; j < pNode->GetChildCount(); j++)
			LoadNode(pNode->GetChild(j));
	}

	void FbxLoader::LoadAttribute(FbxNodeAttribute* pAttribute)
	{
		if (pAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh* pMesh = (FbxMesh*)pAttribute;

			FbxVector4* IControlPoints = pMesh->GetControlPoints();
			m_vertexCount = pMesh->GetControlPointsCount();
			m_vertexBuffer = new float[m_vertexCount * 4];
			for (int i = 0; i < m_vertexCount * 4; i+=4)
			{
				m_vertexBuffer[i]     = (float)IControlPoints[i / 4].mData[0];
				m_vertexBuffer[i + 1] = (float)IControlPoints[i / 4].mData[1];
				m_vertexBuffer[i + 2] = (float)IControlPoints[i / 4].mData[2];
				m_vertexBuffer[i + 3] = 1.0f;// IControlPoints[i / 4].mData[3];
			}

			int* pIndices = pMesh->GetPolygonVertices();
			m_indexCount = pMesh->GetPolygonVertexCount();
			m_indexBuffer = new uint32[m_indexCount];
			for (int i = 0; i < m_indexCount; ++i)
			{
				m_indexBuffer[i] = pIndices[i];
			}

			FbxLayer* pLayer = pMesh->GetLayer(0);
			if (pLayer != NULL)
			{
				FbxLayerElementNormal* pNormal = pLayer->GetNormals();
				m_normalCount = pNormal->mDirectArray->GetCount();
				m_normalBuffer = new float[m_normalCount * 3];
				for (int i = 0; i < m_normalCount * 3; i+=3)
				{
					m_normalBuffer[i]     = (float)(*pNormal->mDirectArray)[i / 3][0];
					m_normalBuffer[i + 1] = (float)(*pNormal->mDirectArray)[i / 3][1];
					m_normalBuffer[i + 2] = (float)(*pNormal->mDirectArray)[i / 3][2];
					//m_normalBuffer[i + 3] = (*pNormal->mDirectArray)[i / 4][3];
				}

				FbxLayerElementUV* pUV = pLayer->GetUVs();
				if (pUV->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
				{
					if (pUV->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						std::vector<int> index;
						std::map<int, std::pair<float, float>> direct;
						for (int i = 0; i < pMesh->GetPolygonCount(); i++)
						{
							for (int j = 0; j < pMesh->GetPolygonSize(i); j++)
							{
								int idx = pMesh->GetPolygonVertex(i, j);
								FbxVector2 uv = pUV->GetDirectArray().GetAt(idx);
								direct[idx] = std::pair<float, float>(uv[0], uv[1]);
								index.push_back(idx);
							}
						}
						m_uvCount = index.size();
						m_uvBuffer = new float[m_uvCount * 2];
						for (int i = 0; i < m_uvCount; i++)
						{
							m_uvBuffer[i * 2] = direct[index[i]].first;
							m_uvBuffer[i * 2 + 1] = direct[index[i]].second;
						}
					}
				}
				else
				{
					if (pUV->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						m_uvCount = pUV->mIndexArray->GetCount();
						m_uvBuffer = new float[m_uvCount * 2];
						for (int i = 0; i < m_uvCount * 2; i++)
						{
							m_uvBuffer[i] = (float)(*pUV->mDirectArray)[(*pUV->mIndexArray)[i / 2]][0];
							m_uvBuffer[i + 1] = (float)(*pUV->mDirectArray)[(*pUV->mIndexArray)[i / 2]][1];
						}
					}
					else
					{
						m_uvCount = pUV->mDirectArray->GetCount();
						m_uvBuffer = new float[m_uvCount * 2];
						for (int i = 0; i < m_uvCount * 2; i++)
						{
							m_uvBuffer[i] = (float)(*pUV->mDirectArray)[i / 2][0];
							m_uvBuffer[i + 1] = (float)(*pUV->mDirectArray)[i / 2][1];
						}
					}
				}
			}
		}
	}


}