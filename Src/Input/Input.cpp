#include <Input/Input.h>

using namespace PGE;

Input::Input() {
    inputDown = false;
    inputHit = false;
}

bool Input::isDown() const {
    return inputDown;
}

bool Input::isHit() const {
    return inputHit;
}

void Input::setDown(bool down) {
    inputDown = down;
}

void Input::setHit(bool hit) {
    inputHit = hit;
}

KeyboardInput::KeyboardInput(KeyboardInput::Keycode inKeyCode) {
    keyCode = inKeyCode;
}

Input::Device KeyboardInput::getDevice() const {
    return Input::Device::KEYBOARD;
}

int KeyboardInput::getKey() const {
    return (int)keyCode;
}

KeyboardInput::Keycode KeyboardInput::getButton() const {
    return keyCode;
}

MouseInput::MouseInput(MouseInput::Button inMouseButton) {
    mouseButton = inMouseButton;
    clicks = 0;
}

Input::Device MouseInput::getDevice() const {
    return Input::Device::MOUSE;
}

int MouseInput::getKey() const {
    return (int)mouseButton;
}

MouseInput::Button MouseInput::getButton() const {
    return mouseButton;
}

void MouseInput::setClickCount(int count) {
    clicks = count;
}

int MouseInput::getClickCount() const {
    return clicks;
}

ControllerInput::ControllerInput(Controller* ctrlr, ControllerInput::Button inControllerButton, float threshold) {
    controller = ctrlr;
    controllerButton = inControllerButton;
    stickPosition = Vector2f::ZERO;
    pressDepth = 0.f;
    downThreshold = threshold;
}

Input::Device ControllerInput::getDevice() const {
    return Input::Device::CONTROLLER;
}

int ControllerInput::getKey() const {
    return (int)controllerButton;
}

ControllerInput::Button ControllerInput::getButton() const {
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

Controller* ControllerInput::getController() const {
    return controller;
}

void ControllerInput::removeController() {
    controller = nullptr;
    stickPosition = Vector2f::ZERO;
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
