#include <PGE/Input/Input.h>

using namespace PGE;

bool Input::isDown() const {
    return inputDown;
}

bool Input::isHit() const {
    return inputHit;
}

bool Input::isReleased() const {
    return inputReleased;
}

void Input::setDown(bool down) {
    inputDown = down;
}

void Input::setHit(bool hit) {
    inputHit = hit;
}

void Input::setReleased(bool released) {
    inputReleased = released;
}

Device KeyboardInput::getDevice() const {
    return Device::KEYBOARD;
}

int KeyboardInput::getKey() const {
    return (int)keyCode;
}

Keycode KeyboardInput::getButton() const {
    return keyCode;
}

Device MouseInput::getDevice() const {
    return Device::MOUSE;
}

int MouseInput::getKey() const {
    return (int)mouseButton;
}

MouseButton MouseInput::getButton() const {
    return mouseButton;
}

void MouseInput::setClickCount(int count) {
    clicks = count;
}

int MouseInput::getClickCount() const {
    return clicks;
}

ControllerInput::ControllerInput(Controller* ctrlr, ControllerButton inControllerButton, float threshold)
    : controllerButton(inControllerButton) {
    controller = ctrlr;
    stickPosition = Vectors::ZERO2F;
    pressDepth = 0.f;
    downThreshold = threshold;
}

Device ControllerInput::getDevice() const {
    return Device::CONTROLLER;
}

int ControllerInput::getKey() const {
    return (int)controllerButton;
}

ControllerButton ControllerInput::getButton() const {
    return controllerButton;
}

const Vector2f& ControllerInput::getStickPosition() const {
    return stickPosition;
}

float ControllerInput::getPressDepth() const {
    return pressDepth;
}

float ControllerInput::getDownThreshold() const {
    return downThreshold;
}

const Controller* ControllerInput::getController() const {
    return controller;
}

void ControllerInput::removeController() {
    controller = nullptr;
    stickPosition = Vectors::ZERO2F;
    inputHit = false; inputDown = false;
    pressDepth = 0.f;
}

void ControllerInput::setStickPosition(const Vector2f& pos) {
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
