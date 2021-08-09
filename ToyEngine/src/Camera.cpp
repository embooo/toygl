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

    if (bMoveRight) move(Axis::X, deltaTime);
    if (bMoveLeft)  move(-Axis::X, deltaTime);
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

    switch (event.m_Key)
    {
        case GLFW_KEY_UP:   bMoveUp = false; break;
        case GLFW_KEY_DOWN: bMoveDown = false; break;
        case GLFW_KEY_LEFT:  bMoveLeft = false; break;
        case GLFW_KEY_RIGHT: bMoveRight = false; break;
    }
}

void Camera::onMouseClickEvent(MouseClick& event)
{
    if(event.m_Action == GLFW_PRESS && event.m_Mods == GLFW_MOD_ALT)
    {
        bLookAround = true;
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
    view = glm::lookAt(position, position + target, up);
}
