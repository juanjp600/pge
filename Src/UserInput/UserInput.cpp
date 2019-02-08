#include <UserInput/UserInput.h>

using namespace PGE;

UserInput::UserInput() {
    inputDown = false;
}

bool UserInput::isDown() const {
    return inputDown;
}

void UserInput::setDown(bool down) {
    inputDown = down;
}

KeyboardInput::KeyboardInput(KeyboardInput::SCANCODE inKeyCode) {
    keyCode = inKeyCode;
}

UserInput::DEVICE KeyboardInput::getDevice() const {
    return UserInput::DEVICE::KEYBOARD;
}

KeyboardInput::SCANCODE KeyboardInput::getButton() const {
    return keyCode;
}

MouseInput::MouseInput(MouseInput::BUTTON inMouseButton) {
    mouseButton = inMouseButton;
}

UserInput::DEVICE MouseInput::getDevice() const {
    return UserInput::DEVICE::MOUSE;
}

MouseInput::BUTTON MouseInput::getButton() const {
    return mouseButton;
}

GamepadInput::GamepadInput(GamepadInput::CONTROLLER_BUTTON inGamepadButton) {
    gamepadButton = inGamepadButton;
}

UserInput::DEVICE GamepadInput::getDevice() const {
    return UserInput::DEVICE::GAMEPAD;
}

GamepadInput::CONTROLLER_BUTTON GamepadInput::getButton() const {
    return gamepadButton;
}
