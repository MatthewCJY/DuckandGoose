/*!*************************************************************************
****
\file			VertexArray.cpp
\author			Huang Xin Xiang
\par DP email:	h.xinxiang@digipen.edu
\par Course:	Gam200
\section		A
\date			02-11-2022
\brief			This file contain VertexBuffer binding and unbind set the buffer
				layout to tell house our buffer data will be read by our gpu
****************************************************************************
***/
#include "empch.h"
#include "VertexArray.h"

namespace EM {

	////////////Vertex Array///////////
	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_RenderID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(0, &m_RenderID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RenderID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexbuffer)
	{
		glBindVertexArray(m_RenderID);
		vertexbuffer->Bind();

		const auto& layout = vertexbuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(m_VBIndex);
			glVertexAttribPointer(m_VBIndex, element.GetComponentCount(),
				GL_FLOAT, element.Normalized, layout.GetStride(), (const void*) element.Offset);

			m_VBIndex++;
		}

		m_VertexBuffers.push_back(vertexbuffer);
	}
	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RenderID);
		indexBuffer->Bind();

		m_IndexBuffers = indexBuffer;
	}
}