#pragma once

#include <string>

#include "Event.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"
#include "IObserver.h"
#include "Window.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

struct ViewFrustum
{
    ViewFrustum()
        : fieldOfView(45.0f), windowWidth(1280), windowHeight(720), aspectRatio(16.0f/9.0f), zNear(0.1f), zFar(100.0f) {};
    ViewFrustum(float fov, float width, float height, float zNear = 0.1f, float zFar = 100.0f)
        : fieldOfView(fov), windowWidth(width), windowHeight(height), aspectRatio(width/height), zNear(zNear), zFar(zFar) {};

    // Data to build a projection matrix 
    // Angle (in radians) between the side or top/bottom planes of the frustum
    float fieldOfView;  
    // Aspect ratio of the viewing window
    float aspectRatio;
    // Near depth limit - Distance in the Z-axis between the camera and near plane
    float zNear; 
    // Far depth limit - Distance in the Z-axis between the camera and far plane
    float zFar;  

    float windowWidth, windowHeight;
};

namespace Axis
{
    static const glm::vec3 X = glm::vec3(1.0f, 0.0f, 0.0f);
    static const glm::vec3 Y = glm::vec3(0.0f, 1.0f, 0.0f);
    static const glm::vec3 Z = glm::vec3(0.0f, 0.0f, 1.0f);
};

namespace Mouse
{
    static glm::vec2 LastPos = glm::vec2(0.0f, 0.0f);
    static glm::vec2 LastClickPos = glm::vec2(0.0f, 0.0f);
    static glm::vec2 Delta = glm::vec2(0.0f, 0.0f);;
}

class Camera : public IObserver
{
public:
    Camera();
    Camera(const std::string& name, Window* window, const ViewFrustum& frustum,
    const glm::vec3& pos = glm::vec3(0.0f, 0.0f, 0.0f),
    const glm::vec3& target = glm::vec3(0.0f, 0.0f, -1.0f),
    const glm::vec3& up  = glm::vec3(0.0f, 1.0f, 0.0f) );

    const glm::mat4& getViewMat() const;
    const glm::mat4& getProjectionMat() const;

    void move(const glm::vec3& dir, float deltaTime);
    void rotate(float deltaTime);

    virtual void onUpdate(Event& event) override;
    void update(const float dt = 0.1f) ;
    
    

    // Mouse/Keyboard events handler
    virtual void onKeyPressed(KeyEvent& event);
    virtual void onKeyReleased(KeyEvent& event);
    virtual void onMouseClickEvent(MouseClick& event);
    virtual void onMouseMove(MouseMove& event);
    virtual void onMouseScroll(MouseScroll& event);
    
private:
    Window* m_WindowHandle;
    std::string name;

    // Viewing volume - everything inside is visible
    ViewFrustum viewFrustum;

    // Matrices to feed to shaders
    glm::mat4 view;
    glm::mat4 projection;

    // Default position of the camera - apex of the view frustum
    glm::vec3 position;
    // LookAt direction
    glm::vec3 front; // aka target
    // Normalized vector - camera's up axis
    glm::vec3 up;

    glm::vec3 right;

    void updateViewMatrix();

    float m_MoveSpeed;
    float m_LookSpeed;

    float rotYawAmount = 0.0f, rotPitchAmount = 0.0f;
    float horizontalSensitivity = 0.1f, verticalSensitivity = 0.1f;

    float m_deltaTime;
    float pitch = 0.0f;
    float yaw = -90.0f;
    bool bOutdatedViewMatrix = true;
    bool firstClick = true;
    bool bMouseMoved = false;
    bool bMoveUp = false, bMoveDown = false, bMoveFront =false, bMoveBack = false, bMoveLeft = false, bMoveRight = false;
    bool lookAroundState = false;

};