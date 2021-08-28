#include "Camera.h"

Camera::Camera()
    : name("DefaultCamera"), viewFrustum(ViewFrustum()), position(glm::vec3(0.0f,0.0f,0.0f)), target(glm::vec3(0.0f, 0.0f, -1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), 
    view(glm::lookAt(position, position + target, up)),
    projection(glm::perspective(viewFrustum.fieldOfView, viewFrustum.aspectRatio, viewFrustum.zNear, viewFrustum.zFar)), m_BaseSpeed(0.5f), bLookAround(false)
{
    bMoveUp= bMoveDown= bMoveLeft= bMoveRight = false;
}

Camera::Camera(const std::string& name, const ViewFrustum& frustum, const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
    : name(name), position(pos), target(target), up(up), viewFrustum(frustum), 
    view(glm::lookAt(pos, pos + target, up)),
    projection(glm::perspective(frustum.fieldOfView, frustum.aspectRatio, frustum.zNear, frustum.zFar)), m_BaseSpeed(0.5f), bLookAround(false)
{
    bMoveUp= bMoveDown= bMoveLeft= bMoveRight = false;
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
    if (bMoveUp)    move(-Axis::Z, deltaTime);
    if (bMoveDown)  move(Axis::Z, deltaTime);

    if (bMoveUp)       move(up,  deltaTime);
    if (bMoveDown)     move(-up,  deltaTime);

    if (bMoveFront)    move(front,  deltaTime);
    if (bMoveBack)     move(-front, deltaTime);

    if (bMoveRight) move(right, deltaTime);
    if (bMoveLeft)  move(-right, deltaTime);
}
    
    if(bOutdatedViewMatrix)
    {
        updateViewMatrix();
        bOutdatedViewMatrix = false;
    }
}
    
    if(bOutdatedViewMatrix)
    {
        updateViewMatrix();
        bOutdatedViewMatrix = false;
    }
}
    
    if(bOutdatedViewMatrix)
    {
        updateViewMatrix();
        bOutdatedViewMatrix = false;
    }
}
    
    if(bOutdatedViewMatrix)
    {
        updateViewMatrix();
        bOutdatedViewMatrix = false;
    }
}
        lookAroundState = true;
        m_WindowHandle->setCursorVisibility(false);
        m_WindowHandle->setCursorVisibility(false);
        updateViewMatrix();
        bOutdatedViewMatrix = false;
    }
}
    
    if(bOutdatedViewMatrix)
    {
        updateViewMatrix();
        bOutdatedViewMatrix = false;
    }
}

void Camera::onKeyPressed(KeyEvent& event)
{
    switch (event.m_Key)
    {
        case GLFW_KEY_UP:    bMoveUp = true; break;
        case GLFW_KEY_DOWN:  bMoveDown = true; break;
        case GLFW_KEY_LEFT:  bMoveLeft = true; break;
        case GLFW_KEY_RIGHT: bMoveRight = true; break;
    }
}

void Camera::onKeyReleased(KeyEvent& event)
{

    view = glm::lookAt(position, position + front, up);
}
    
void Camera::rotate(float deltaTime)
{
    front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    front = glm::normalize(front);

    updateViewMatrix();
    {
        firstClick = true;
        lookAroundState = true;
        m_WindowHandle->setCursorVisibility(false);
    }
    else if(event.m_Action == GLFW_RELEASE && bLookAround)
    {
        bLookAround = false;
    }
}

void Camera::onMouseMove(MouseMove& event)
{
    if(bLookAround)
    {
        //
    }
}

void Camera::onMouseScroll(MouseScroll& event)
{

}


void Camera::move(const glm::vec3& dir, float deltaTime)
{
    // Update camera's position
    position += dir * m_BaseSpeed * deltaTime;

    // Update view matrix
    view = glm::lookAt(position, position + front, up);
}
    
void Camera::rotate(float deltaTime)
{
    front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    front = glm::normalize(front);

    bOutdatedViewMatrix = true; 
}
