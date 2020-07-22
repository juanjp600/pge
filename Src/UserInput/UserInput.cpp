#include <UserInput/UserInput.h>

using namespace PGE;

UserInput::UserInput() {
    inputDown = false;
    inputHit = false;
}

bool UserInput::isDown() const {
    return inputDown;
}

bool UserInput::isHit() const {
    return inputHit;
}

void UserInput::setDown(bool down) {
    inputDown = down;
}

void UserInput::setHit(bool hit) {
    inputHit = hit;
}

KeyboardInput::KeyboardInput(KeyboardInput::KEYCODE inKeyCode) {
    keyCode = inKeyCode;
}

UserInput::DEVICE KeyboardInput::getDevice() const {
    return UserInput::DEVICE::KEYBOARD;
}

KeyboardInput::KEYCODE KeyboardInput::getButton() const {
    return keyCode;
}

MouseInput::MouseInput(MouseInput::BUTTON inMouseButton) {
    mouseButton = inMouseButton;
    clicks = 0;
}

UserInput::DEVICE MouseInput::getDevice() const {
    return UserInput::DEVICE::MOUSE;
}

MouseInput::BUTTON MouseInput::getButton() const {
    return mouseButton;
}

void MouseInput::setClickCount(int count) {
    clicks = count;
}

int MouseInput::getClickCount() const {
    return clicks;
}

ControllerInput::ControllerInput(Controller* ctrlr, ControllerInput::BUTTON inControllerButton, float threshold) {
    controller = ctrlr;
    controllerButton = inControllerButton;
    stickPosition = Vector2f::zero;
    pressDepth = 0.f;
    downThreshold = threshold;
}

UserInput::DEVICE ControllerInput::getDevice() const {
    return UserInput::DEVICE::CONTROLLER;
}

ControllerInput::BUTTON ControllerInput::getButton() const {
    return controllerButton;
}

Vector2f ControllerInput::getStickPosition() const {
    return stickPosition;
}

float ControllerInput::getPressDepth() const {
    return pressDepth;
}

float ControllerInput::getDownThreshold() const {
    return downThreshold;
}

Controller* ControllerInput::getController() const {
    return controller;
}

void ControllerInput::removeController() {
    controller = nullptr;
    stickPosition = Vector2f::zero;
    inputHit = false; inputDown = false;
    pressDepth = 0.f;
}

void ControllerInput::setStickPosition(Vector2f pos) {
    stickPosition = pos;
}

void ControllerInput::setPressDepth(float depth) {
    pressDepth = depth;
    inputDown = pressDepth > downThreshold;
}

void ControllerInput::setDownThreshold(float threshold) {
    downThreshold = threshold;
    inputDown = pressDepth > downThreshold;
}
