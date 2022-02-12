#include "PCH.h"

#include "OpenGLTexture.h"

GLTexture::GLTexture()
{
	s_Count = 0;
}

unsigned int GLTexture::create(const char* filename, int target)
{
	int width, height, channels;
	unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);

	int baseInternalFormat;
	switch (channels)
	{
		case 1:  baseInternalFormat = GL_RED;  break;
		case 2:  baseInternalFormat = GL_RG;   break;
		case 3:  baseInternalFormat = GL_RGB;  break;
		case 4:  baseInternalFormat = GL_RGBA; break;
		default: baseInternalFormat = GL_RGBA;
	}

	create(target, width, height, baseInternalFormat, data);

	stbi_image_free(data);

	return m_info.id;
}

unsigned int GLTexture::create(int target, int width, int height, int format, unsigned char* data, int unit, bool genMipMap)
{
	m_info = { target, width, height, format, unit, 0, GL_UNSIGNED_BYTE };

	// Direct State Access
	glCreateTextures(target, 1, &m_info.id);
	glBindTextureUnit(m_info.unit, m_info.id);

	glTextureParameteri(m_info.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(m_info.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_info.id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_info.id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTextureParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);

	glTextureStorage2D(m_info.id,  1, baseToSizedInternalFormat(format), width, height);
	glTextureSubImage2D(m_info.id, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

	if (genMipMap)
	{
		glGenerateTextureMipmap(m_info.id);
	}

	s_Count++;
	return m_info.id;
}

const GLTexture& GLTexture::bind() const
{
	//glBindTexture(m_info.target, m_info.id);
	glBindTextureUnit(m_info.unit, m_info.id);

	return *this;
}

unsigned int GLTexture::getId() const
{
	return m_info.id;
}


void GLTexture::unbind()
{
	glBindTexture(m_info.target, 0);
}

void GLTexture::release()
{
	printf("Destructor : GLTexture \n");
	glDeleteTextures(1, &m_info.id);
}
