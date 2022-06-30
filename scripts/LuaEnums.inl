//
// Created by Stefan Antoszko on 2022-06-30.
//

#pragma once

// all the open_Xxx functions expect a stack with table 'BIGGEngine' at the top.
void open_MouseButtonEnum(lua_State* L) {
    lua_createtable(L, 0, 11);      // create table MouseButtonEnum

    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Zero));
    lua_setfield(L, -2, "Zero");
    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::One));
    lua_setfield(L, -2, "One");
    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Two));
    lua_setfield(L, -2, "Two");
    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Three));
    lua_setfield(L, -2, "Three");
    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Four));
    lua_setfield(L, -2, "Four");
    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Five));
    lua_setfield(L, -2, "Five");
    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Six));
    lua_setfield(L, -2, "Six");
    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Seven));
    lua_setfield(L, -2, "Seven");
    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Left));
    lua_setfield(L, -2, "Left");
    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Middle));
    lua_setfield(L, -2, "Middle");
    lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Right));
    lua_setfield(L, -2, "Right");

    lua_setfield(L, -2, "MouseButtonEnum"); // set BIGGEngine.MouseButtonEnum
}

void open_KeyEnum(lua_State* L) {
    lua_createtable(L, 0, 121);      // create table KeyEnum

    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Unknown));
    lua_setfield(L, -2, "Unknown");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Space));
    lua_setfield(L, -2, "Space");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Apostrophe));
    lua_setfield(L, -2, "Apostrophe");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Comma));
    lua_setfield(L, -2, "Comma");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Minus));
    lua_setfield(L, -2, "Minus");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Period));
    lua_setfield(L, -2, "Period");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Slash));
    lua_setfield(L, -2, "Slash");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Zero));
    lua_setfield(L, -2, "Zero");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::One));
    lua_setfield(L, -2, "One");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Two));
    lua_setfield(L, -2, "Two");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Three));
    lua_setfield(L, -2, "Three");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Four));
    lua_setfield(L, -2, "Four");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Five));
    lua_setfield(L, -2, "Five");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Six));
    lua_setfield(L, -2, "Six");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Seven));
    lua_setfield(L, -2, "Seven");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Eight));
    lua_setfield(L, -2, "Eight");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Nine));
    lua_setfield(L, -2, "Nine");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Semicolon));
    lua_setfield(L, -2, "Semicolon");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Equal));
    lua_setfield(L, -2, "Equal");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::A));
    lua_setfield(L, -2, "A");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::B));
    lua_setfield(L, -2, "B");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::C));
    lua_setfield(L, -2, "C");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::D));
    lua_setfield(L, -2, "D");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::E));
    lua_setfield(L, -2, "E");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F));
    lua_setfield(L, -2, "F");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::G));
    lua_setfield(L, -2, "G");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::H));
    lua_setfield(L, -2, "H");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::I));
    lua_setfield(L, -2, "I");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::J));
    lua_setfield(L, -2, "J");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::K));
    lua_setfield(L, -2, "K");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::L));
    lua_setfield(L, -2, "L");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::M));
    lua_setfield(L, -2, "M");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::N));
    lua_setfield(L, -2, "N");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::O));
    lua_setfield(L, -2, "O");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::P));
    lua_setfield(L, -2, "P");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Q));
    lua_setfield(L, -2, "Q");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::R));
    lua_setfield(L, -2, "R");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::S));
    lua_setfield(L, -2, "S");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::T));
    lua_setfield(L, -2, "T");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::U));
    lua_setfield(L, -2, "U");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::V));
    lua_setfield(L, -2, "V");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::W));
    lua_setfield(L, -2, "W");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::X));
    lua_setfield(L, -2, "X");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Y));
    lua_setfield(L, -2, "Y");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Z));
    lua_setfield(L, -2, "Z");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::LeftBracket));
    lua_setfield(L, -2, "LeftBracket");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Backslash));
    lua_setfield(L, -2, "Backslash");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::RightBracket));
    lua_setfield(L, -2, "RightBracket");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::GraveAccent));
    lua_setfield(L, -2, "GraveAccent");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::World1));
    lua_setfield(L, -2, "World1");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::World2));
    lua_setfield(L, -2, "World2");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Escape));
    lua_setfield(L, -2, "Escape");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Enter));
    lua_setfield(L, -2, "Enter");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Tab));
    lua_setfield(L, -2, "Tab");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Backspace));
    lua_setfield(L, -2, "Backspace");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Insert));
    lua_setfield(L, -2, "Insert");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Delete));
    lua_setfield(L, -2, "Delete");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Right));
    lua_setfield(L, -2, "Right");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Left));
    lua_setfield(L, -2, "Left");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Down));
    lua_setfield(L, -2, "Down");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Up));
    lua_setfield(L, -2, "Up");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::PageUp));
    lua_setfield(L, -2, "PageUp");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::PageDown));
    lua_setfield(L, -2, "PageDown");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Home));
    lua_setfield(L, -2, "Home");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::End));
    lua_setfield(L, -2, "End");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::CapsLock));
    lua_setfield(L, -2, "CapsLock");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::ScrollLock));
    lua_setfield(L, -2, "ScrollLock");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumLock));
    lua_setfield(L, -2, "NumLock");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::PrintScreen));
    lua_setfield(L, -2, "PrintScreen");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Pause));
    lua_setfield(L, -2, "Pause");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F1));
    lua_setfield(L, -2, "F1");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F2));
    lua_setfield(L, -2, "F2");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F3));
    lua_setfield(L, -2, "F3");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F4));
    lua_setfield(L, -2, "F4");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F5));
    lua_setfield(L, -2, "F5");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F6));
    lua_setfield(L, -2, "F6");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F7));
    lua_setfield(L, -2, "F7");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F8));
    lua_setfield(L, -2, "F8");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F9));
    lua_setfield(L, -2, "F9");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F10));
    lua_setfield(L, -2, "F10");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F11));
    lua_setfield(L, -2, "F11");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F12));
    lua_setfield(L, -2, "F12");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F13));
    lua_setfield(L, -2, "F13");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F14));
    lua_setfield(L, -2, "F14");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F15));
    lua_setfield(L, -2, "F15");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F16));
    lua_setfield(L, -2, "F16");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F17));
    lua_setfield(L, -2, "F17");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F18));
    lua_setfield(L, -2, "F18");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F19));
    lua_setfield(L, -2, "F19");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F20));
    lua_setfield(L, -2, "F20");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F21));
    lua_setfield(L, -2, "F21");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F22));
    lua_setfield(L, -2, "F22");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F23));
    lua_setfield(L, -2, "F23");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F24));
    lua_setfield(L, -2, "F24");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F25));
    lua_setfield(L, -2, "F25");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadZero));
    lua_setfield(L, -2, "NumpadZero");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadOne));
    lua_setfield(L, -2, "NumpadOne");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadTwo));
    lua_setfield(L, -2, "NumpadTwo");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadThree));
    lua_setfield(L, -2, "NumpadThree");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadFour));
    lua_setfield(L, -2, "NumpadFour");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadFive));
    lua_setfield(L, -2, "NumpadFive");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadSix));
    lua_setfield(L, -2, "NumpadSix");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadSeven));
    lua_setfield(L, -2, "NumpadSeven");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadEight));
    lua_setfield(L, -2, "NumpadEight");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadNine));
    lua_setfield(L, -2, "NumpadNine");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadDecimal));
    lua_setfield(L, -2, "NumpadDecimal");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadDivide));
    lua_setfield(L, -2, "NumpadDivide");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadMultiply));
    lua_setfield(L, -2, "NumpadMultiply");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadSubtract));
    lua_setfield(L, -2, "NumpadSubtract");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadAdd));
    lua_setfield(L, -2, "NumpadAdd");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadEnter));
    lua_setfield(L, -2, "NumpadEnter");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadEqual));
    lua_setfield(L, -2, "NumpadEqual");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::LeftShift));
    lua_setfield(L, -2, "LeftShift");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::LeftControl));
    lua_setfield(L, -2, "LeftControl");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::LeftAlt));
    lua_setfield(L, -2, "LeftAlt");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::LeftSuper));
    lua_setfield(L, -2, "LeftSuper");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::RightShift));
    lua_setfield(L, -2, "RightShift");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::RightControl));
    lua_setfield(L, -2, "RightControl");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::RightAlt));
    lua_setfield(L, -2, "RightAlt");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::RightSuper));
    lua_setfield(L, -2, "RightSuper");
    lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Menu));
    lua_setfield(L, -2, "Menu");

    lua_setfield(L, -2, "KeyEnum"); // set BIGGEngine.KeyEnum
}

void open_ActionEnum(lua_State* L) {
    lua_createtable(L, 0, 3);      // create table ActionEnum

    lua_pushnumber(L, static_cast<lua_Number>(ActionEnum::Release));
    lua_setfield(L, -2, "Release");
    lua_pushnumber(L, static_cast<lua_Number>(ActionEnum::Press));
    lua_setfield(L, -2, "Press");
    lua_pushnumber(L, static_cast<lua_Number>(ActionEnum::Repeat));
    lua_setfield(L, -2, "Repeat");

    lua_setfield(L, -2, "ActionEnum"); // set BIGGEngine.ActionEnum
}

void open_ModsEnum(lua_State* L) {
    lua_createtable(L, 0, 6);      // create table ModsEnum

    lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::Shift));
    lua_setfield(L, -2, "Shift");
    lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::Control));
    lua_setfield(L, -2, "Control");
    lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::Alt));
    lua_setfield(L, -2, "Alt");
    lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::Super));
    lua_setfield(L, -2, "Super");
    lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::CapsLock));
    lua_setfield(L, -2, "CapsLock");
    lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::NumLock));
    lua_setfield(L, -2, "NumLock");

    lua_setfield(L, -2, "ModsEnum"); // set BIGGEngine.ModsEnum
}