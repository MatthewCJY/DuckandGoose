/*!*************************************************************************
****
\file			VertexArray.h
\author			Huang Xin Xiang
\par DP email:	h.xinxiang@digipen.edu
\par Course:	Gam200
\section		A
\date			02-11-2022
\brief			This file contain VertexBuffer binding and unbind set the buffer
				layout to tell house our buffer data will be read by our gpu
****************************************************************************
***/
#pragma once
#include "Buffer.h"
namespace EM {

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();
		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexbuffer);
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffers; }
		static MultiRefs<VertexArray> Create() { return CreateMultiRefs<VertexArray>(); }
	private:
		unsigned int m_RenderID;
		unsigned int m_VBIndex = 0;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffers;
	};
}