#pragma once

#include <string>

#include "Event.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"
#include "IObserver.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

class Window;

struct Matrices
{
    glm::mat4 view;
    glm::mat4 projection;
};

struct CamVectors
{
    glm::vec3 pos; // Default position of the camera - apex of the view frustum
    glm::vec3 front;    // LookAt direction aka target
    glm::vec3 up;       // Normalized vector - camera's up axis
    glm::vec3 right;
};

struct State
{
    float pitch;
    float yaw;
    int   currentMove;
    float moveSpeed;
    float rotSpeed;
    float hSens;
    float vSens;
};

struct ViewFrustum
{
    ViewFrustum()
        : fov(45.0f), windowWidth(1280), windowHeight(720), aspect(16.0f/9.0f), znear(0.1f), zfar(100.0f) {};
    ViewFrustum(float fov, float width, float height, float zNear = 0.1f, float zFar = 100.0f)
        : fov(fov), windowWidth(width), windowHeight(height), aspect(width/height), znear(zNear), zfar(zFar) {};

    // Data to build a projection matrix 
    // Angle (in radians) between the side or top/bottom planes of the frustum
    float fov;  
    // Aspect ratio of the viewing window
    float aspect;
    // Near depth limit - Distance in the Z-axis between the camera and near plane
    float znear; 
    // Far depth limit - Distance in the Z-axis between the camera and far plane
    float zfar;  

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
    Camera(Window* window,
           const std::string& name, 
           const ViewFrustum& frustum,
           const glm::vec3&   pos    = { 0.0f, 0.0f,  0.0f },
           const glm::vec3&   front  = { 0.0f, 0.0f, -1.0f },
           const glm::vec3&   up     = { 0.0f, 1.0f,  0.0f }
    );

    virtual void onUpdate(Event& event) override;
    void update(const float dt = 0.1f);

    // Getters
    const glm::mat4& viewMat() const;
    const glm::mat4& projMat() const;
    const glm::vec3& pos()     const;
    glm::vec3& pos();

    // Navigation
    void move(const glm::vec3& dir, float deltaTime);
    void rotate(float deltaTime);

    // Setters
    void DisableRotation() { IsRotating = false; }
    void updateAspectRatio(float aspectRatio);

    // Mouse/Keyboard events handler
    virtual void onKeyPressed(KeyEvent& event);
    virtual void onKeyReleased(KeyEvent& event);
    virtual void onMouseClickEvent(MouseClick& event);
    virtual void onMouseMove(MouseMove& event);
    virtual void onMouseScroll(MouseScroll& event);
    
private:
    Window*     m_WindowHandle;
    std::string m_Name;
    ViewFrustum m_VF; 
    CamVectors  m_CamVecs;
    Matrices    m_CamMats;
    State m_State;

    void updateViewMat();
    void updateProjMat();

    inline static float m_deltaTime;

    enum class MoveStates
    {
        NONE      = 0,
        MoveUp    = 1,
        MoveDown  = 2,
        MoveFront = 4,
        MoveBack  = 8,
        MoveLeft  = 16,
        MoveRight = 32
    };

    bool  IsRotating = false;
};