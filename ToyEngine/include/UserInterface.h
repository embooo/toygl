#pragma once

#include "IObserver.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Light.h"
#include "OpenGLRenderer.h"
#include "OpenGLTexture.h"
#include "OpenGLFramebuffer.h"


class Window;

struct Resources
{
	std::vector<GLTexture> textures;
	GLFramebuffer framebuffer;
};

class UserInterface : public IObserver
{
public:
	UserInterface();
	virtual void onUpdate(Event& event) override;
	virtual void init(Window& window);
	virtual void beginFrame(std::vector<GLuint>& tex, Light& light, const Camera& camera,  OpenGLRenderer& renderer);
	virtual void render();
	virtual bool wantCaptureMouse() const;
	virtual bool wantCaptureKeyboard() const;
	virtual void terminate();

	virtual ~UserInterface();

private:
	bool show_demo_window    = false;
	bool show_another_window = false;
	bool enable_vsync        = true;

	Resources resources;

	ImVec4 clear_color       = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGuiIO* io;
	Window* m_pWindow;
};