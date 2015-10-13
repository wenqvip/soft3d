#pragma once
#include "soft3d.h"
#include <fbxsdk.h>

namespace soft3d
{

	class FbxLoader
	{
	public:
		FbxLoader();
		~FbxLoader();

		int LoadFbx(const char* fbxName);

		const float* GetVertexBuffer() const
		{
			return m_vertexBuffer;
		}
		uint32 GetVertexCount()
		{
			return m_vertexCount;
		}

		const uint32* GetIndexBuffer() const
		{
			return m_indexBuffer;
		}
		uint32 GetIndexCount()
		{
			return m_indexCount;
		}

		const float* GetNormalBuffer() const
		{
			return m_normalBuffer;
		}
		uint32 GetNormalCount()
		{
			return m_normalCount;
		}

	private:
		void LoadNode(FbxNode* node);
		void LoadAttribute(FbxNodeAttribute* pAttribute);

	private:
		float* m_vertexBuffer;
		uint32 m_vertexCount;

		uint32* m_indexBuffer;
		uint32 m_indexCount;

		float* m_normalBuffer;
		uint32 m_normalCount;
	};

}