#include "PCH.h"
#include "OpenGLFramebuffer.h"

GLFramebuffer::GLFramebuffer()
{
	s_Count = 0;
}

void GLFramebuffer::create(int width, int height, int format_color, int format_depth)
{
	m_info = { width, height, 0 };

	glCreateFramebuffers(1, &m_info.id);

	// Setup color and depth attachments to the framebuffer
	if (format_color != 0)
	{
		m_color = std::make_unique<GLTexture>();
		m_color->create(GL_TEXTURE_2D, width, height, format_color);
		glTextureParameteri(m_color->getId(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_color->getId(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glNamedFramebufferTexture(m_info.id, GL_COLOR_ATTACHMENT0, m_color->getId(), 0);
	}

	if (format_depth != 0)
	{
		m_depth = std::make_unique<GLTexture>();
		m_depth->create(GL_TEXTURE_2D, width, height, format_depth);
		glNamedFramebufferTexture(m_info.id, GL_DEPTH_ATTACHMENT, m_depth->getId(), 0);
	}

	auto status = glCheckNamedFramebufferStatus(m_info.id, GL_FRAMEBUFFER);
	//assert(status == GL_FRAMEBUFFER_COMPLETE);
}

void GLFramebuffer::release()
{
	glDeleteFramebuffers(1, &m_info.id);
	m_color.reset();
	m_depth.reset();
}

void GLFramebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_info.id);
	glViewport(0, 0, m_info.width, m_info.height);
}

void GLFramebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const GLTexture& GLFramebuffer::get(const FBAttach& attachment)
{
	switch (attachment)
	{
	case FBAttach::Color:
		return *m_color.get();
	case FBAttach::Depth:
		return *m_depth.get();

	default:
		printf("Unknown framebuffer attachment.");
	}
}
