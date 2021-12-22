#include "Camera.h"

Camera::Camera()
    : name("DefaultCamera"), 
    viewFrustum(ViewFrustum()), 
    position(glm::vec3(0.0f,0.0f, 5.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), 
    view(glm::lookAt(position, position + front, up)),
    projection(glm::perspective(viewFrustum.fieldOfView, viewFrustum.aspectRatio, viewFrustum.zNear, viewFrustum.zFar)), 
    m_MoveSpeed(2.0f), m_LookSpeed (0.25f)
{
    right = glm::normalize(glm::cross(front, up));
}

Camera::Camera(const std::string& name, Window* window, const ViewFrustum& frustum, const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up)
    : name(name), position(pos), front(glm::normalize(front)), up(glm::normalize(up)), viewFrustum(frustum), 
    view(glm::lookAt(pos, glm::normalize(pos + front), up)),
    projection(glm::perspective(frustum.fieldOfView, frustum.aspectRatio, frustum.zNear, frustum.zFar)), m_MoveSpeed(2.0f),m_LookSpeed (0.25f), m_WindowHandle(window)
{
    right = glm::cross(front, up);
}

const glm::mat4& Camera::getViewMat() const
{
    return view;
}

const glm::mat4& Camera::getProjectionMat() const
{
    return projection;
}


void Camera::onUpdate(Event& event)
{
    switch(event.getType())
    {
        case EventType::KeyPressed:
            onKeyPressed(static_cast<KeyEvent&>(event));
            break;

        case EventType::KeyReleased:
            onKeyReleased(static_cast<KeyEvent&>(event));
            break;

        case EventType::MouseButtonPressed:
            onMouseClickEvent(static_cast<MouseClick&>(event));
            break;

        case EventType::MouseMove:
            onMouseMove(static_cast<MouseMove&>(event));
            break;

        case EventType::MouseScroll:
            onMouseScroll(static_cast<MouseScroll&>(event));
            break;
    }
}

void Camera::update(float deltaTime)
{
    m_deltaTime = deltaTime;

    if (bMoveUp)    move( up,    deltaTime);
    if (bMoveDown)  move(-up,    deltaTime);

    if (bMoveFront) move( front, deltaTime);
    if (bMoveBack)  move(-front, deltaTime);

    if (bMoveRight) move( right, deltaTime);
    if (bMoveLeft)  move(-right, deltaTime);
}

void Camera::onKeyPressed(KeyEvent& event)
{
    switch (event.m_Key)
    {
        case GLFW_KEY_SPACE :       bMoveUp      = true; break;
        case GLFW_KEY_LEFT_SHIFT :  m_MoveSpeed *= 2.0f; break;
                                                         
        case GLFW_KEY_UP:           bMoveFront   = true; break;
        case GLFW_KEY_W:            bMoveFront   = true; break;
                                                         
        case GLFW_KEY_DOWN:         bMoveBack    = true; break;
        case GLFW_KEY_S:            bMoveBack    = true; break;
                                                         
        case GLFW_KEY_LEFT:         bMoveLeft    = true; break;
        case GLFW_KEY_A:            bMoveLeft    = true; break;
                                                         
        case GLFW_KEY_RIGHT:        bMoveRight   = true; break;
        case GLFW_KEY_D:            bMoveRight   = true; break;
    }
}

void Camera::onKeyReleased(KeyEvent& event)
{
    switch (event.m_Key)
    {
        case GLFW_KEY_SPACE:        bMoveUp     = false; break;
        case GLFW_KEY_LEFT_SHIFT :  m_MoveSpeed = 2.0f;  break;

        case GLFW_KEY_UP:           bMoveFront  = false; break;
        case GLFW_KEY_W:            bMoveFront  = false; break;

        case GLFW_KEY_DOWN:         bMoveBack   = false; break;
        case GLFW_KEY_S:            bMoveBack   = false; break;

        case GLFW_KEY_LEFT:         bMoveLeft   = false; break;
        case GLFW_KEY_A:            bMoveLeft   = false; break;

        case GLFW_KEY_RIGHT:        bMoveRight  = false; break;
        case GLFW_KEY_D:            bMoveRight  = false; break;
    }
}

void Camera::onMouseClickEvent(MouseClick& event)
{
    
    if(event.m_Button == GLFW_MOUSE_BUTTON_LEFT && event.m_Action == GLFW_PRESS )
    {
        lookAroundState = true;
        m_WindowHandle->setCursorVisibility(false);
    }
    else if(event.m_Button == GLFW_MOUSE_BUTTON_LEFT && event.m_Action == GLFW_RELEASE)
    {
        lookAroundState = false;
        m_WindowHandle->setCursorVisibility(true);
    }
}

void Camera::onMouseMove(MouseMove& event)
{
    glm::vec2 current(event.getX(), event.getY());

    Mouse::Delta = current - Mouse::LastPos;

    if(lookAroundState && Mouse::Delta != glm::vec2(0.0f))
    {
        yaw   +=  Mouse::Delta.x * 0.3f ;
        pitch -=  Mouse::Delta.y * 0.3f ;

        pitch = glm::clamp<float>(pitch, -89.0, 89.0);

        rotate(m_deltaTime);
    }

    Mouse::LastPos = current;
} 

void Camera::onMouseScroll(MouseScroll& event)
{
    move(front * (float)event.getYOffset() * m_MoveSpeed * 10.0f, m_deltaTime);
}

void Camera::updateViewMatrix()
{
    view = glm::lookAt(position, position + front, up);
}

void Camera::move(const glm::vec3& dir, float deltaTime)
{
    // Update 
    right = glm::normalize(glm::cross(front, up));

    // Update camera's position
    position += (dir * m_MoveSpeed * deltaTime);

    // Update view matrix
    view = glm::lookAt(position, position + front, up);
}
    
void Camera::rotate(float deltaTime)
{
    front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    front = glm::normalize(front);

    updateViewMatrix();
}

void Camera::updateAspectRatio(float aspectRatio)
{
    projection = glm::perspective(viewFrustum.fieldOfView, aspectRatio, viewFrustum.zNear, viewFrustum.zFar);
}