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

KeyboardInput::KeyboardInput(SDL_Scancode inKeyCode) {
    keyCode = inKeyCode;
}

UserInput::DEVICE KeyboardInput::getDevice() const {
    return UserInput::DEVICE::KEYBOARD;
}

SDL_Scancode KeyboardInput::getButton() const {
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

GamepadInput::GamepadInput(SDL_GameControllerButton inGamepadButton) {
    gamepadButton = inGamepadButton;
}

UserInput::DEVICE GamepadInput::getDevice() const {
    return UserInput::DEVICE::GAMEPAD;
}

SDL_GameControllerButton GamepadInput::getButton() const {
    return gamepadButton;
}
