#include "QSPCH.h"
#include "Quelos/Core/Input.h"

#include "Quelos\Core\Application.h"

#include "GLFW\glfw3.h"

namespace Quelos
{
    struct KeyState
    {
        bool State = false;
        bool PrevState = false;
    };

    GLFWwindow* m_Window;
    static std::unordered_map<KeyCode, KeyState> m_KeysDown;
    static std::unordered_map<KeyCode, KeyState> m_KeysUp;

    void Input::Init()
    {
        QS_PROFILE_FUNCTION();

        m_Window = (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow();

        m_KeysDown.emplace(KeyCode::Space        , KeyState());
        m_KeysDown.emplace(KeyCode::Apostrophe   , KeyState());
        m_KeysDown.emplace(KeyCode::Comma        , KeyState());
        m_KeysDown.emplace(KeyCode::Minus        , KeyState());
        m_KeysDown.emplace(KeyCode::Period       , KeyState());
        m_KeysDown.emplace(KeyCode::Slash        , KeyState());
        m_KeysDown.emplace(KeyCode::Alpha0       , KeyState());
        m_KeysDown.emplace(KeyCode::Alpha1       , KeyState());
        m_KeysDown.emplace(KeyCode::Alpha2       , KeyState());
        m_KeysDown.emplace(KeyCode::Alpha3       , KeyState());
        m_KeysDown.emplace(KeyCode::Alpha4       , KeyState());
        m_KeysDown.emplace(KeyCode::Alpha5       , KeyState());
        m_KeysDown.emplace(KeyCode::Alpha6       , KeyState());
        m_KeysDown.emplace(KeyCode::Alpha7       , KeyState());
        m_KeysDown.emplace(KeyCode::Alpha8       , KeyState());
        m_KeysDown.emplace(KeyCode::Alpha9       , KeyState());
        m_KeysDown.emplace(KeyCode::Semicolon    , KeyState());
        m_KeysDown.emplace(KeyCode::Equal        , KeyState());
        m_KeysDown.emplace(KeyCode::A            , KeyState());
        m_KeysDown.emplace(KeyCode::B            , KeyState());
        m_KeysDown.emplace(KeyCode::C            , KeyState());
        m_KeysDown.emplace(KeyCode::D            , KeyState());
        m_KeysDown.emplace(KeyCode::E            , KeyState());
        m_KeysDown.emplace(KeyCode::F            , KeyState());
        m_KeysDown.emplace(KeyCode::G            , KeyState());
        m_KeysDown.emplace(KeyCode::H            , KeyState());
        m_KeysDown.emplace(KeyCode::I            , KeyState());
        m_KeysDown.emplace(KeyCode::J            , KeyState());
        m_KeysDown.emplace(KeyCode::K            , KeyState());
        m_KeysDown.emplace(KeyCode::L            , KeyState());
        m_KeysDown.emplace(KeyCode::M            , KeyState());
        m_KeysDown.emplace(KeyCode::N            , KeyState());
        m_KeysDown.emplace(KeyCode::O            , KeyState());
        m_KeysDown.emplace(KeyCode::P            , KeyState());
        m_KeysDown.emplace(KeyCode::Q            , KeyState());
        m_KeysDown.emplace(KeyCode::R            , KeyState());
        m_KeysDown.emplace(KeyCode::S            , KeyState());
        m_KeysDown.emplace(KeyCode::T            , KeyState());
        m_KeysDown.emplace(KeyCode::U            , KeyState());
        m_KeysDown.emplace(KeyCode::V            , KeyState());
        m_KeysDown.emplace(KeyCode::W            , KeyState());
        m_KeysDown.emplace(KeyCode::X            , KeyState());
        m_KeysDown.emplace(KeyCode::Y            , KeyState());
        m_KeysDown.emplace(KeyCode::Z            , KeyState());
        m_KeysDown.emplace(KeyCode::LeftBracket , KeyState());
        m_KeysDown.emplace(KeyCode::Backslash    , KeyState());
        m_KeysDown.emplace(KeyCode::RightBracket, KeyState());
        m_KeysDown.emplace(KeyCode::GraveAccent , KeyState());
        m_KeysDown.emplace(KeyCode::World_1      , KeyState());
        m_KeysDown.emplace(KeyCode::World_2      , KeyState());

        m_KeysDown.emplace(KeyCode::Escape       , KeyState());
        m_KeysDown.emplace(KeyCode::Enter        , KeyState());
        m_KeysDown.emplace(KeyCode::Tab          , KeyState());
        m_KeysDown.emplace(KeyCode::Backspace    , KeyState());
        m_KeysDown.emplace(KeyCode::Insert       , KeyState());
        m_KeysDown.emplace(KeyCode::Delete       , KeyState());
        m_KeysDown.emplace(KeyCode::Right        , KeyState());
        m_KeysDown.emplace(KeyCode::Left         , KeyState());
        m_KeysDown.emplace(KeyCode::Down         , KeyState());
        m_KeysDown.emplace(KeyCode::Up           , KeyState());
        m_KeysDown.emplace(KeyCode::Page_up      , KeyState());
        m_KeysDown.emplace(KeyCode::Page_down    , KeyState());
        m_KeysDown.emplace(KeyCode::Home         , KeyState());
        m_KeysDown.emplace(KeyCode::End          , KeyState());
        m_KeysDown.emplace(KeyCode::Caps_lock    , KeyState());
        m_KeysDown.emplace(KeyCode::Scroll_lock  , KeyState());
        m_KeysDown.emplace(KeyCode::Num_lock     , KeyState());
        m_KeysDown.emplace(KeyCode::Print_screen , KeyState());
        m_KeysDown.emplace(KeyCode::Pause        , KeyState());
        m_KeysDown.emplace(KeyCode::F1           , KeyState());
        m_KeysDown.emplace(KeyCode::F2           , KeyState());
        m_KeysDown.emplace(KeyCode::F3           , KeyState());
        m_KeysDown.emplace(KeyCode::F4           , KeyState());
        m_KeysDown.emplace(KeyCode::F5           , KeyState());
        m_KeysDown.emplace(KeyCode::F6           , KeyState());
        m_KeysDown.emplace(KeyCode::F7           , KeyState());
        m_KeysDown.emplace(KeyCode::F8           , KeyState());
        m_KeysDown.emplace(KeyCode::F9           , KeyState());
        m_KeysDown.emplace(KeyCode::F10          , KeyState());
        m_KeysDown.emplace(KeyCode::F11          , KeyState());
        m_KeysDown.emplace(KeyCode::F12          , KeyState());
        m_KeysDown.emplace(KeyCode::F13          , KeyState());
        m_KeysDown.emplace(KeyCode::F14          , KeyState());
        m_KeysDown.emplace(KeyCode::F15          , KeyState());
        m_KeysDown.emplace(KeyCode::F16          , KeyState());
        m_KeysDown.emplace(KeyCode::F17          , KeyState());
        m_KeysDown.emplace(KeyCode::F18          , KeyState());
        m_KeysDown.emplace(KeyCode::F19          , KeyState());
        m_KeysDown.emplace(KeyCode::F20          , KeyState());
        m_KeysDown.emplace(KeyCode::F21          , KeyState());
        m_KeysDown.emplace(KeyCode::F22          , KeyState());
        m_KeysDown.emplace(KeyCode::F23          , KeyState());
        m_KeysDown.emplace(KeyCode::F24          , KeyState());
        m_KeysDown.emplace(KeyCode::F25          , KeyState());
        m_KeysDown.emplace(KeyCode::Kp0          , KeyState());
        m_KeysDown.emplace(KeyCode::Kp1          , KeyState());
        m_KeysDown.emplace(KeyCode::Kp2          , KeyState());
        m_KeysDown.emplace(KeyCode::Kp3          , KeyState());
        m_KeysDown.emplace(KeyCode::Kp4          , KeyState());
        m_KeysDown.emplace(KeyCode::Kp5          , KeyState());
        m_KeysDown.emplace(KeyCode::Kp6          , KeyState());
        m_KeysDown.emplace(KeyCode::Kp7          , KeyState());
        m_KeysDown.emplace(KeyCode::Kp8          , KeyState());
        m_KeysDown.emplace(KeyCode::Kp9          , KeyState());
        m_KeysDown.emplace(KeyCode::KpDecimal    , KeyState());
        m_KeysDown.emplace(KeyCode::KpDivide     , KeyState());
        m_KeysDown.emplace(KeyCode::KpMultiply   , KeyState());
        m_KeysDown.emplace(KeyCode::KpSubtract   , KeyState());
        m_KeysDown.emplace(KeyCode::KpAdd        , KeyState());
        m_KeysDown.emplace(KeyCode::KpEnter      , KeyState());
        m_KeysDown.emplace(KeyCode::KpEqual      , KeyState());
        m_KeysDown.emplace(KeyCode::LeftShift    , KeyState());
        m_KeysDown.emplace(KeyCode::LeftControl  , KeyState());
        m_KeysDown.emplace(KeyCode::LeftAlt      , KeyState());
        m_KeysDown.emplace(KeyCode::LeftSuper    , KeyState());
        m_KeysDown.emplace(KeyCode::RightShift   , KeyState());
        m_KeysDown.emplace(KeyCode::RightControl , KeyState());
        m_KeysDown.emplace(KeyCode::RightAlt     , KeyState());
        m_KeysDown.emplace(KeyCode::RightSuper   , KeyState());
        m_KeysDown.emplace(KeyCode::Menu         , KeyState());


        m_KeysUp.emplace(KeyCode::Space, KeyState());
        m_KeysUp.emplace(KeyCode::Apostrophe, KeyState());
        m_KeysUp.emplace(KeyCode::Comma, KeyState());
        m_KeysUp.emplace(KeyCode::Minus, KeyState());
        m_KeysUp.emplace(KeyCode::Period, KeyState());
        m_KeysUp.emplace(KeyCode::Slash, KeyState());
        m_KeysUp.emplace(KeyCode::Alpha0, KeyState());
        m_KeysUp.emplace(KeyCode::Alpha1, KeyState());
        m_KeysUp.emplace(KeyCode::Alpha2, KeyState());
        m_KeysUp.emplace(KeyCode::Alpha3, KeyState());
        m_KeysUp.emplace(KeyCode::Alpha4, KeyState());
        m_KeysUp.emplace(KeyCode::Alpha5, KeyState());
        m_KeysUp.emplace(KeyCode::Alpha6, KeyState());
        m_KeysUp.emplace(KeyCode::Alpha7, KeyState());
        m_KeysUp.emplace(KeyCode::Alpha8, KeyState());
        m_KeysUp.emplace(KeyCode::Alpha9, KeyState());
        m_KeysUp.emplace(KeyCode::Semicolon, KeyState());
        m_KeysUp.emplace(KeyCode::Equal, KeyState());
        m_KeysUp.emplace(KeyCode::A, KeyState());
        m_KeysUp.emplace(KeyCode::B, KeyState());
        m_KeysUp.emplace(KeyCode::C, KeyState());
        m_KeysUp.emplace(KeyCode::D, KeyState());
        m_KeysUp.emplace(KeyCode::E, KeyState());
        m_KeysUp.emplace(KeyCode::F, KeyState());
        m_KeysUp.emplace(KeyCode::G, KeyState());
        m_KeysUp.emplace(KeyCode::H, KeyState());
        m_KeysUp.emplace(KeyCode::I, KeyState());
        m_KeysUp.emplace(KeyCode::J, KeyState());
        m_KeysUp.emplace(KeyCode::K, KeyState());
        m_KeysUp.emplace(KeyCode::L, KeyState());
        m_KeysUp.emplace(KeyCode::M, KeyState());
        m_KeysUp.emplace(KeyCode::N, KeyState());
        m_KeysUp.emplace(KeyCode::O, KeyState());
        m_KeysUp.emplace(KeyCode::P, KeyState());
        m_KeysUp.emplace(KeyCode::Q, KeyState());
        m_KeysUp.emplace(KeyCode::R, KeyState());
        m_KeysUp.emplace(KeyCode::S, KeyState());
        m_KeysUp.emplace(KeyCode::T, KeyState());
        m_KeysUp.emplace(KeyCode::U, KeyState());
        m_KeysUp.emplace(KeyCode::V, KeyState());
        m_KeysUp.emplace(KeyCode::W, KeyState());
        m_KeysUp.emplace(KeyCode::X, KeyState());
        m_KeysUp.emplace(KeyCode::Y, KeyState());
        m_KeysUp.emplace(KeyCode::Z, KeyState());
        m_KeysUp.emplace(KeyCode::LeftBracket, KeyState());
        m_KeysUp.emplace(KeyCode::Backslash, KeyState());
        m_KeysUp.emplace(KeyCode::RightBracket, KeyState());
        m_KeysUp.emplace(KeyCode::GraveAccent, KeyState());
        m_KeysUp.emplace(KeyCode::World_1, KeyState());
        m_KeysUp.emplace(KeyCode::World_2, KeyState());

        m_KeysUp.emplace(KeyCode::Escape, KeyState());
        m_KeysUp.emplace(KeyCode::Enter, KeyState());
        m_KeysUp.emplace(KeyCode::Tab, KeyState());
        m_KeysUp.emplace(KeyCode::Backspace, KeyState());
        m_KeysUp.emplace(KeyCode::Insert, KeyState());
        m_KeysUp.emplace(KeyCode::Delete, KeyState());
        m_KeysUp.emplace(KeyCode::Right, KeyState());
        m_KeysUp.emplace(KeyCode::Left, KeyState());
        m_KeysUp.emplace(KeyCode::Down, KeyState());
        m_KeysUp.emplace(KeyCode::Up, KeyState());
        m_KeysUp.emplace(KeyCode::Page_up, KeyState());
        m_KeysUp.emplace(KeyCode::Page_down, KeyState());
        m_KeysUp.emplace(KeyCode::Home, KeyState());
        m_KeysUp.emplace(KeyCode::End, KeyState());
        m_KeysUp.emplace(KeyCode::Caps_lock, KeyState());
        m_KeysUp.emplace(KeyCode::Scroll_lock, KeyState());
        m_KeysUp.emplace(KeyCode::Num_lock, KeyState());
        m_KeysUp.emplace(KeyCode::Print_screen, KeyState());
        m_KeysUp.emplace(KeyCode::Pause, KeyState());
        m_KeysUp.emplace(KeyCode::F1, KeyState());
        m_KeysUp.emplace(KeyCode::F2, KeyState());
        m_KeysUp.emplace(KeyCode::F3, KeyState());
        m_KeysUp.emplace(KeyCode::F4, KeyState());
        m_KeysUp.emplace(KeyCode::F5, KeyState());
        m_KeysUp.emplace(KeyCode::F6, KeyState());
        m_KeysUp.emplace(KeyCode::F7, KeyState());
        m_KeysUp.emplace(KeyCode::F8, KeyState());
        m_KeysUp.emplace(KeyCode::F9, KeyState());
        m_KeysUp.emplace(KeyCode::F10, KeyState());
        m_KeysUp.emplace(KeyCode::F11, KeyState());
        m_KeysUp.emplace(KeyCode::F12, KeyState());
        m_KeysUp.emplace(KeyCode::F13, KeyState());
        m_KeysUp.emplace(KeyCode::F14, KeyState());
        m_KeysUp.emplace(KeyCode::F15, KeyState());
        m_KeysUp.emplace(KeyCode::F16, KeyState());
        m_KeysUp.emplace(KeyCode::F17, KeyState());
        m_KeysUp.emplace(KeyCode::F18, KeyState());
        m_KeysUp.emplace(KeyCode::F19, KeyState());
        m_KeysUp.emplace(KeyCode::F20, KeyState());
        m_KeysUp.emplace(KeyCode::F21, KeyState());
        m_KeysUp.emplace(KeyCode::F22, KeyState());
        m_KeysUp.emplace(KeyCode::F23, KeyState());
        m_KeysUp.emplace(KeyCode::F24, KeyState());
        m_KeysUp.emplace(KeyCode::F25, KeyState());
        m_KeysUp.emplace(KeyCode::Kp0         , KeyState());
        m_KeysUp.emplace(KeyCode::Kp1         , KeyState());
        m_KeysUp.emplace(KeyCode::Kp2         , KeyState());
        m_KeysUp.emplace(KeyCode::Kp3         , KeyState());
        m_KeysUp.emplace(KeyCode::Kp4         , KeyState());
        m_KeysUp.emplace(KeyCode::Kp5         , KeyState());
        m_KeysUp.emplace(KeyCode::Kp6         , KeyState());
        m_KeysUp.emplace(KeyCode::Kp7         , KeyState());
        m_KeysUp.emplace(KeyCode::Kp8         , KeyState());
        m_KeysUp.emplace(KeyCode::Kp9         , KeyState());
        m_KeysUp.emplace(KeyCode::KpDecimal   , KeyState());
        m_KeysUp.emplace(KeyCode::KpDivide    , KeyState());
        m_KeysUp.emplace(KeyCode::KpMultiply  , KeyState());
        m_KeysUp.emplace(KeyCode::KpSubtract  , KeyState());
        m_KeysUp.emplace(KeyCode::KpAdd       , KeyState());
        m_KeysUp.emplace(KeyCode::KpEnter     , KeyState());
        m_KeysUp.emplace(KeyCode::KpEqual     , KeyState());
        m_KeysUp.emplace(KeyCode::LeftShift   , KeyState());
        m_KeysUp.emplace(KeyCode::LeftControl , KeyState());
        m_KeysUp.emplace(KeyCode::LeftAlt     , KeyState());
        m_KeysUp.emplace(KeyCode::LeftSuper   , KeyState());
        m_KeysUp.emplace(KeyCode::RightShift  , KeyState());
        m_KeysUp.emplace(KeyCode::RightControl, KeyState());
        m_KeysUp.emplace(KeyCode::RightAlt    , KeyState());
        m_KeysUp.emplace(KeyCode::RightSuper  , KeyState());
        m_KeysUp.emplace(KeyCode::Menu, KeyState());
    }

    bool Input::GetKey(KeyCode keycode)
    {
        auto state = glfwGetKey(m_Window, (int)keycode);
        return state == GLFW_PRESS;
    }

    bool Input::GetKeyDown(KeyCode keycode)
    {
        QS_PROFILE_FUNCTION();
        
        auto state = glfwGetKey(m_Window, (int)keycode);

        bool& kState = m_KeysDown[keycode].State;
        bool& kPrevState = m_KeysDown[keycode].PrevState;

        if (state == GLFW_RELEASE)
        {
            kPrevState = false;
            kState = false;
        }
        else if (state == GLFW_PRESS)
        {
            if (!kPrevState)
            {
                kState = true;
                kPrevState = true;
            }
            else if (kPrevState)
                kState = false;
        }

        return kState;
    }

    bool Input::GetKeyUp(KeyCode keycode)
    {
        QS_PROFILE_FUNCTION();

        auto state = glfwGetKey(m_Window, (int)keycode);

        bool& kState = m_KeysUp[keycode].State;
        bool& kPrevState = m_KeysUp[keycode].PrevState;

        if (state == GLFW_PRESS)
        {
            kPrevState = false;
            kState = false;
        }
        else if (state == GLFW_RELEASE)
        {
            if (!kPrevState)
            {
                kState = true;
                kPrevState = true;
            }
            else if (kPrevState)
                kState = false;
        }

        return kState;
    }

    bool Input::IsMouseButtonPressed(MouseButtonCode button)
    {
        auto state = glfwGetMouseButton(m_Window, (int)button);
        return state == GLFW_PRESS;
    }

    bool Input::IsMouseButtonReleased(MouseButtonCode button)
    {
        auto state = glfwGetMouseButton(m_Window, (int)button);
        return state == GLFW_RELEASE;
    }

    glm::vec2 Input::GetMousePosition()
    {
        double x, y;
        glfwGetCursorPos(m_Window, &x, &y);

        return {(float)x, (float)y};
    }
}
