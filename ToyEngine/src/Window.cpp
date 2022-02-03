#include "PCH.h"

#include "Window.h"

// Note : VSync is enabled by default
Window::Window()
    : m_Props({ "DefaultWindow", 1280, 720, 4, 3, 1, true })
{
    init();
}

Window::Window(const char* title, const int width, const int height, const int glMajor, const int glMinor, int swapInterval, bool vsync)
    : m_Props({ title, width, height, glMajor, glMinor, swapInterval, vsync, } )
{
    init();
}

static void errorCallback(int error, const char *msg)
{
    std::cerr << "GLFW Error " << error << ": " << msg << std::endl;
}

void Window::init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW.\n";
    }
    
    // Set error callback
    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_Props.GLProps.major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_Props.GLProps.minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG_BUILD
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Enables OpenGL debug output
#endif
    m_pWindow = glfwCreateWindow(m_Props.width, m_Props.height, m_Props.title, nullptr, nullptr);

    if (!m_pWindow)
    {
        std::cerr << "Failed to create GLFW window.\n";
    }

    glfwMakeContextCurrent(m_pWindow);

    // Load OpenGL functions
    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize glad.\n";
    }

    glfwSetWindowUserPointer(m_pWindow, this);

    // Set callbacks for window/mouse/keyboard events
    glfwSetKeyCallback(m_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
    {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->notify( KeyEvent(key, scancode, action, mods) );
    });

    // Mouse callbacks ----------------------------------------------------------------------------------
    glfwSetMouseButtonCallback(m_pWindow, [](GLFWwindow* window, int button, int action, int mods)
    {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->notify( MouseClick(button, action, mods) );
    });

    glfwSetCursorPosCallback(m_pWindow, [](GLFWwindow* window, double xpos, double ypos)
    {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->notify( MouseMove(xpos, ypos) );
    });

    glfwSetScrollCallback(m_pWindow, [](GLFWwindow* window, double xoffset, double yoffset)
    {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->notify( MouseScroll(xoffset, yoffset) );
    });

    // Window callbacks ----------------------------------------------------------------------------------
    glfwSetWindowSizeCallback(m_pWindow, [](GLFWwindow* window, int width, int height)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        win->m_Props.width = width;
        win->m_Props.height = height;
        win->notify( WindowResizeEvent(width, height) );
    });

    glfwSetWindowIconifyCallback(m_pWindow, [](GLFWwindow* window, int minimized)
    {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->notify(WindowMinimizeEvent(minimized));
    });

    glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* window) 
    {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->notify(WindowCloseEvent());
    });


    // User interface
    m_UserInterface.init(*this);

}

void Window::swapBuffers()
{
    glfwSwapInterval(m_Props.swapInterval);
    glfwSwapBuffers(m_pWindow);
}

bool Window::isClosed() const
{
    return glfwWindowShouldClose(m_pWindow);
}

GLFWwindow* Window::getRawPtr() const
{
    return m_pWindow;
}

void Window::notify(Event& event)
{
	for (IObserver* observer : m_Observers)
	{
		observer->onUpdate(event);
	}
}

void Window::setCursorVisibility(bool visible)
{
    int value = visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN;
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, value);
}

bool Window::isVsyncActive() const
{
    return m_Props.vsyncEnabled;
}

void Window::attach(IObserver* observer)
{
    m_Observers.push_back(observer);
}

void Window::detach(IObserver* observer)
{
    auto pos = std::find(m_Observers.begin(), m_Observers.end(), observer);
    if(pos != m_Observers.end())
    {
        m_Observers.erase(pos);
    }
}


int Window::height()
{
    return m_Props.height;
}

int Window::width()
{
    return m_Props.width;
}

void Window::toggleVsync()
{
    m_Props.vsyncEnabled = !m_Props.vsyncEnabled;
    m_Props.swapInterval = m_Props.vsyncEnabled ? 1 : 0;
}

UserInterface& Window::getUI()
{
    return m_UserInterface;
}

void Window::update()
{
}

void Window::terminate()
{
    glfwTerminate();
}

Window::~Window()
{
    std::cerr << "Destructor :  Window\n";
    terminate();
}
