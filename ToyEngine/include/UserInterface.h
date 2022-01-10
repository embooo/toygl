#pragma once

#include "IObserver.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Light.h"
#include "OpenGLRenderer.h"
class Window;

class UserInterface : public IObserver
{
public:
	UserInterface();
	virtual void onUpdate(Event& event) override;
	virtual void init(const Window& window);
	virtual void beginFrame(Light& light, const Camera& camera,  OpenGLRenderer& renderer);
	virtual void render();
	virtual bool wantCaptureMouse() const;
	virtual bool wantCaptureKeyboard() const;
	virtual ~UserInterface();

private:
	bool show_demo_window    = true;
	bool show_another_window = false;
	ImVec4 clear_color       = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGuiIO* io;
	virtual void terminate();

};