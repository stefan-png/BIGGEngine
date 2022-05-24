#pragma once
#include "InputEnums.hpp"
#include "Events.hpp"

#include <map>

namespace BIGGEngine::Debug {

const std::map<KeyEnum, const char*> g_keyEnumDebugStrings = {
    {KeyEnum::Unknown,        "Unknown" },    /* The unknown key */
    {KeyEnum::Space,          "Space" }, /* Printable keys */
    {KeyEnum::Apostrophe,     "Apostrophe" },  /* ' */
    {KeyEnum::Comma,          "Comma" },  /* , */
    {KeyEnum::Minus,          "Minus" },  /* - */
    {KeyEnum::Period,         "Period" },  /* . */
    {KeyEnum::Slash,          "Slash" },  /* / */
    {KeyEnum::Zero,           "Zero" },
    {KeyEnum::One,            "One" },
    {KeyEnum::Two,            "Two" },
    {KeyEnum::Three,          "Three" },
    {KeyEnum::Four,           "Four" },
    {KeyEnum::Five,           "Five" },
    {KeyEnum::Six,            "Six" },
    {KeyEnum::Seven,          "Seven" },
    {KeyEnum::Eight,          "Eight" },
    {KeyEnum::Nine,           "Nine" },
    {KeyEnum::Semicolon,      "Semicolon" },  /* ; */
    {KeyEnum::Equal,          "Equal" },  /* = */
    {KeyEnum::A,              "A" },
    {KeyEnum::B,              "B" },
    {KeyEnum::C,              "C" },
    {KeyEnum::D,              "D" },
    {KeyEnum::E,              "E" },
    {KeyEnum::F,              "F" },
    {KeyEnum::G,              "G" },
    {KeyEnum::H,              "H" },
    {KeyEnum::I,              "I" },
    {KeyEnum::J,              "J" },
    {KeyEnum::K,              "K" },
    {KeyEnum::L,              "L" },
    {KeyEnum::M,              "M" },
    {KeyEnum::N,              "N" },
    {KeyEnum::O,              "O" },
    {KeyEnum::P,              "P" },
    {KeyEnum::Q,              "Q" },
    {KeyEnum::R,              "R" },
    {KeyEnum::S,              "S" },
    {KeyEnum::T,              "T" },
    {KeyEnum::U,              "U" },
    {KeyEnum::V,              "V" },
    {KeyEnum::W,              "W" },
    {KeyEnum::X,              "X" },
    {KeyEnum::Y,              "Y" },
    {KeyEnum::Z,              "Z" },
    {KeyEnum::LeftBracket,    "LeftBracket" },  /* [ */
    {KeyEnum::Backslash,      "Backslash" },  /* \ */
    {KeyEnum::RightBracket,   "RightBracket" },  /* ] */
    {KeyEnum::GraveAccent,    "GraveAccent" },  /* ` */
    {KeyEnum::World1,         "World1" }, /* non-US #1 */
    {KeyEnum::World2,         "World2" }, /* non-US #2 */
    {KeyEnum::Escape,         "Escape" }, /* Function keys */
    {KeyEnum::Enter,          "Enter" },
    {KeyEnum::Tab,            "Tab" },
    {KeyEnum::Backspace,      "Backspace" },
    {KeyEnum::Insert,         "Insert" },
    {KeyEnum::Delete,         "Delete" },
    {KeyEnum::Right,          "Right" },
    {KeyEnum::Left,           "Left" },
    {KeyEnum::Down,           "Down" },
    {KeyEnum::Up,             "Up" },
    {KeyEnum::PageUp,         "PageUp" },
    {KeyEnum::PageDown,       "PageDown" },
    {KeyEnum::Home,           "Home" },
    {KeyEnum::End,            "End" },
    {KeyEnum::CapsLock,       "CapsLock" },
    {KeyEnum::ScrollLock,     "ScrollLock" },
    {KeyEnum::NumLock,        "NumLock" },
    {KeyEnum::PrintScreen,    "PrintScreen" },
    {KeyEnum::Pause,          "Pause" },
    {KeyEnum::F1,             "F1" },
    {KeyEnum::F2,             "F2" },
    {KeyEnum::F3,             "F3" },
    {KeyEnum::F4,             "F4" },
    {KeyEnum::F5,             "F5" },
    {KeyEnum::F6,             "F6" },
    {KeyEnum::F7,             "F7" },
    {KeyEnum::F8,             "F8" },
    {KeyEnum::F9,             "F9" },
    {KeyEnum::F10,            "F10" },
    {KeyEnum::F11,            "F11" },
    {KeyEnum::F12,            "F12" },
    {KeyEnum::F13,            "F13" },
    {KeyEnum::F14,            "F14" },
    {KeyEnum::F15,            "F15" },
    {KeyEnum::F16,            "F16" },
    {KeyEnum::F17,            "F17" },
    {KeyEnum::F18,            "F18" },
    {KeyEnum::F19,            "F19" },
    {KeyEnum::F20,            "F20" },
    {KeyEnum::F21,            "F21" },
    {KeyEnum::F22,            "F22" },
    {KeyEnum::F23,            "F23" },
    {KeyEnum::F24,            "F24" },
    {KeyEnum::F25,            "F25" },
    {KeyEnum::NumpadZero,     "NumpadZero" },
    {KeyEnum::NumpadOne,      "NumpadOne" },
    {KeyEnum::NumpadTwo,      "NumpadTwo" },
    {KeyEnum::NumpadThree,    "NumpadThree" },
    {KeyEnum::NumpadFour,     "NumpadFour" },
    {KeyEnum::NumpadFive,     "NumpadFive" },
    {KeyEnum::NumpadSix,      "NumpadSix" },
    {KeyEnum::NumpadSeven,    "NumpadSeven" },
    {KeyEnum::NumpadEight,    "NumpadEight" },
    {KeyEnum::NumpadNine,     "NumpadNine" },
    {KeyEnum::NumpadDecimal,  "NumpadDecimal" },
    {KeyEnum::NumpadDivide,   "NumpadDivide" },
    {KeyEnum::NumpadMultiply, "NumpadMultiply" },
    {KeyEnum::NumpadSubtract, "NumpadSubtract" },
    {KeyEnum::NumpadAdd,      "NumpadAdd" },
    {KeyEnum::NumpadEnter,    "NumpadEnter" },
    {KeyEnum::NumpadEqual,    "NumpadEqual" },
    {KeyEnum::LeftShift,      "LeftShift" },
    {KeyEnum::LeftControl,    "LeftControl" },
    {KeyEnum::LeftAlt,        "LeftAlt" },
    {KeyEnum::LeftSuper,      "LeftSuper" },
    {KeyEnum::RightShift,     "RightShift" },
    {KeyEnum::RightControl,   "RightControl" },
    {KeyEnum::RightAlt,       "RightAlt" },
    {KeyEnum::RightSuper,     "RightSuper" },
    {KeyEnum::Menu,           "Menu" }
};

const std::map<ActionEnum, const char*> g_actionEnumDebugStrings = {
    { ActionEnum::Release, "Release" },
    { ActionEnum::Press,   "Press" },
    { ActionEnum::Repeat,  "Repeat" }
};

const std::map<Event::EventType, const char*> g_eventTypeDebugStrings = {
    { Event::EventType::Update,                "Update" },
    { Event::EventType::Tick,                  "Tick" },
    { Event::EventType::WindowCreate,          "WindowCreate" },
    { Event::EventType::WindowDestroy,         "WindowDestroy" },
    { Event::EventType::WindowShouldClose,     "WindowShouldClose" },
    { Event::EventType::WindowSize,            "WindowSize" },
    { Event::EventType::WindowFramebufferSize, "WindowFramebufferSize" },
    { Event::EventType::WindowContentScale,    "WindowContentScale" },
    { Event::EventType::WindowPosition,        "WindowPosition" },
    { Event::EventType::WindowIconify,         "WindowIconify" },
    { Event::EventType::WindowMaximize,        "WindowMaximize" },
    { Event::EventType::WindowFocus,           "WindowFocus" },
    { Event::EventType::WindowRefresh,         "WindowRefresh" },
    { Event::EventType::Key,                   "Key" },
    { Event::EventType::Char,                  "Char" },
    { Event::EventType::MousePosition,         "MousePosition" },
    { Event::EventType::MouseEnter,            "MouseEnter" },
    { Event::EventType::MouseButton,           "MouseButton" },
    { Event::EventType::Scroll,                "Scroll" },
    { Event::EventType::DropPath,              "DropPath" }

    // TODO joystick/gamepad input
};

}   // namespace BIGGEngine::Debug