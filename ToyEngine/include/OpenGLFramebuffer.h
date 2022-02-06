#pragma once

#include "OpenGLTexture.h"

struct FBInfo
{
	int width;
	int height;
	unsigned int id;
};

enum class FBAttach
{
	None = 0, Color = 1, Depth = 2
};

class GLFramebuffer
{
public:
	GLFramebuffer();
	void create(int width, int height, int format_color, int format_depth);
	void release();
	void bind();
	void unbind();

	const GLTexture& get(const FBAttach& attachment);

private:
	inline static int s_Count;
	FBInfo     m_info;
	std::unique_ptr<GLTexture> m_color;
	std::unique_ptr<GLTexture> m_depth;
};