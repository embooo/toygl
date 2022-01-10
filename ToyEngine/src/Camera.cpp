#include "Camera.h"
#include "Window.h"

Camera::Camera()
        
    : m_WindowHandle(nullptr),
      m_Name("DefaultCamera"), 
      m_VF(ViewFrustum()), 
      m_CamVecs({ glm::vec3(0.0f,0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) }),
      m_CamMats({ glm::lookAt(m_CamVecs.pos, m_CamVecs.pos + m_CamVecs.front, m_CamVecs.up), 
                  glm::perspective(m_VF.fov, m_VF.aspect, m_VF.znear, m_VF.zfar) }),
      m_State({0.0f, -90.0f, 0, 2.0f, 0.25f, 0.1f, 0.1f })
{
    m_CamVecs.right = glm::normalize(glm::cross(m_CamVecs.front, m_CamVecs.up));
}

Camera::Camera(Window* win, const std::string& name, const ViewFrustum& vf, const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up)
    : m_WindowHandle(win),
      m_Name(name), 
      m_VF(vf), 
      m_CamVecs({ pos, front, up }),
      m_CamMats({ glm::lookAt(m_CamVecs.pos, m_CamVecs.pos + m_CamVecs.front, m_CamVecs.up),
                  glm::perspective(vf.fov, vf.aspect, vf.znear, vf.zfar) }),
    m_State({0.0f, -90.0f, 0, 2.0f, 0.25f, 0.1f, 0.1f })
{
    m_CamVecs.right = glm::normalize(glm::cross(front,up));
}

const glm::mat4& Camera::viewMat() const
{
    return m_CamMats.view;
}

const glm::mat4& Camera::projMat() const
{
    return m_CamMats.projection;
}

const glm::vec3& Camera::pos() const
{
    return m_CamVecs.pos;
}

glm::vec3& Camera::pos()
{
    return m_CamVecs.pos;
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

    if ((m_State.currentMove & (int)MoveStates::MoveUp)   == (int)MoveStates::MoveUp)    
        move( m_CamVecs.up, deltaTime);

    if ((m_State.currentMove & (int)MoveStates::MoveDown) == (int)MoveStates::MoveDown)
        move(-m_CamVecs.up, deltaTime);

    if ((m_State.currentMove & (int)MoveStates::MoveFront) == (int)MoveStates::MoveFront)
        move(m_CamVecs.front, deltaTime);

    if ((m_State.currentMove & (int)MoveStates::MoveBack) == (int)MoveStates::MoveBack)
        move(-m_CamVecs.front, deltaTime);

    if ((m_State.currentMove & (int)MoveStates::MoveRight) == (int)MoveStates::MoveRight)
        move(m_CamVecs.right, deltaTime);

    if ((m_State.currentMove & (int)MoveStates::MoveLeft) == (int)MoveStates::MoveLeft)
        move(-m_CamVecs.right, deltaTime);

}

void Camera::onKeyPressed(KeyEvent& event)
{
    switch (event.m_Key)
    {
        
        case GLFW_KEY_SPACE :       m_State.currentMove |= (int)MoveStates::MoveUp;    break;
        case GLFW_KEY_LEFT_SHIFT :  m_State.moveSpeed   *= 2.0f;                       break;
                                                         
        case GLFW_KEY_UP:           m_State.currentMove |= (int)MoveStates::MoveFront; break;
        case GLFW_KEY_W:            m_State.currentMove |= (int)MoveStates::MoveFront; break;
                                                         
        case GLFW_KEY_DOWN:         m_State.currentMove |= (int)MoveStates::MoveBack;  break;
        case GLFW_KEY_S:            m_State.currentMove |= (int)MoveStates::MoveBack;  break;
                                                         
        case GLFW_KEY_LEFT:         m_State.currentMove |= (int)MoveStates::MoveLeft;  break;
        case GLFW_KEY_A:            m_State.currentMove |= (int)MoveStates::MoveLeft;  break;
                                                         
        case GLFW_KEY_RIGHT:        m_State.currentMove |= (int)MoveStates::MoveRight; break;
        case GLFW_KEY_D:            m_State.currentMove |= (int)MoveStates::MoveRight; break;
    }
}

void Camera::onKeyReleased(KeyEvent& event)
{
    switch (event.m_Key)
    {
        case GLFW_KEY_SPACE :       m_State.currentMove &= ~(int)MoveStates::MoveUp;    break;
        case GLFW_KEY_LEFT_SHIFT :  m_State.moveSpeed    = 2.0f;                        break;
                                                            
        case GLFW_KEY_UP:           m_State.currentMove &= ~(int)MoveStates::MoveFront; break;
        case GLFW_KEY_W:            m_State.currentMove &= ~(int)MoveStates::MoveFront; break;
                                                         
        case GLFW_KEY_DOWN:         m_State.currentMove &= ~(int)MoveStates::MoveBack;  break;
        case GLFW_KEY_S:            m_State.currentMove &= ~(int)MoveStates::MoveBack;  break;
                                                        
        case GLFW_KEY_LEFT:         m_State.currentMove &= ~(int)MoveStates::MoveLeft;  break;
        case GLFW_KEY_A:            m_State.currentMove &= ~(int)MoveStates::MoveLeft;  break;
                                                        
        case GLFW_KEY_RIGHT:        m_State.currentMove &= ~(int)MoveStates::MoveRight; break;
        case GLFW_KEY_D:            m_State.currentMove &= ~(int)MoveStates::MoveRight; break;
    }
}

void Camera::onMouseClickEvent(MouseClick& event)
{
    if(event.m_Button == GLFW_MOUSE_BUTTON_LEFT && event.m_Action == GLFW_PRESS )
    {
        IsRotating = true;
        m_WindowHandle->setCursorVisibility(false);
    }
    else if(event.m_Button == GLFW_MOUSE_BUTTON_LEFT && event.m_Action == GLFW_RELEASE)
    {
        IsRotating = false;
        m_WindowHandle->setCursorVisibility(true);
    }
}

void Camera::onMouseMove(MouseMove& event)
{
    glm::vec2 current(event.getX(), event.getY());

    Mouse::Delta = current - Mouse::LastPos;

    if(IsRotating && Mouse::Delta != glm::vec2(0.0f))
    {
        m_State.yaw   +=  Mouse::Delta.x * 0.3f ;
        m_State.pitch -=  Mouse::Delta.y * 0.3f ;

        m_State.pitch = glm::clamp<float>(m_State.pitch, -89.0, 89.0);

        rotate(m_deltaTime);
    }

    Mouse::LastPos = current;
} 

void Camera::onMouseScroll(MouseScroll& event)
{
    move(m_CamVecs.front * (float)event.getYOffset() * m_State.moveSpeed, m_deltaTime);
}

void Camera::move(const glm::vec3& dir, float deltaTime)
{
    // Update camera's vectors
    m_CamVecs.right       = glm::normalize(glm::cross(m_CamVecs.front, m_CamVecs.up));
    m_CamVecs.pos   += dir * m_State.moveSpeed * deltaTime;
    
    // Update view matrix
    m_CamMats.view = glm::lookAt(m_CamVecs.pos, m_CamVecs.pos + m_CamVecs.front, m_CamVecs.up);
}

void Camera::updateViewMat()
{
    m_CamMats.view = glm::lookAt(m_CamVecs.pos, m_CamVecs.pos + m_CamVecs.front, m_CamVecs.up);
}

void Camera::updateProjMat()
{
    m_CamMats.projection = glm::perspective(m_VF.fov, m_VF.aspect, m_VF.znear, m_VF.zfar);
}

void Camera::updateAspectRatio(float aspectRatio)
{
    m_VF.aspect = aspectRatio;
}
    
void Camera::rotate(float deltaTime)
{
    m_CamVecs.front = { glm::cos(glm::radians(m_State.yaw)) * glm::cos(glm::radians(m_State.pitch)),
                        glm::sin(glm::radians(m_State.pitch)),
                        glm::sin(glm::radians(m_State.yaw))* glm::cos(glm::radians(m_State.pitch)) };

    m_CamVecs.front = glm::normalize(m_CamVecs.front);

    updateViewMat();
}

