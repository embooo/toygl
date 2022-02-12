#pragma once

#include "stb_image.h"

struct TextureInfo
{
	int target;
	int width;
	int height;
	int format;		//  format of the pixel data : GL_RED, GL_RGB ... != internal format
	int unit;
	int mip_level;
	int type;		// type of the pixel data : GL_UNSIGNED_BYTE, GL_UNSIGNED_INT ...
	unsigned int id;
};

static int baseToSizedInternalFormat(int baseinternalFormat)
{
	switch (baseinternalFormat)
	{
		case GL_RED:  return GL_R8;
		case GL_RG:   return GL_RG8;
		case GL_RGB:  return GL_RGB8;
		case GL_RGBA: return GL_RGBA8;
		default: return GL_RGBA8;
	}
}

class GLTexture
{
public:
	GLTexture();
	unsigned int create(const char* filename, int target = GL_TEXTURE_2D);
	unsigned int create(int target, int width, int height, int format = GL_RGB8, unsigned char *data = nullptr, int unit = 0, bool genMipMap = true);
	const GLTexture& bind() const;
	unsigned int getId() const;
	int  width() { return m_info.width; };
	int  height() { return m_info.height; };
	void unbind();
	void release();

	inline static int s_Count;
private:
	TextureInfo m_info;
};