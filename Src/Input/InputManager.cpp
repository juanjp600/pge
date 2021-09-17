#include "InputManagerInternal.h"

#if defined(__APPLE__) && defined(__OBJC__)
#import <AppKit/AppKit.h>
#include <PGE/SDL_syswm.h>
#endif

#include <PGE/Exception/Exception.h>
#include <PGE/Graphics/Graphics.h>

#include "../SysEvents/SysEventsInternal.h"
#include "../Graphics/GraphicsInternal.h"

using namespace PGE;

InputManager* InputManager::create(const Graphics& gfx) {
    return new InputManagerInternal(gfx);
}

InputManagerInternal::InputManagerInternal(const Graphics& gfx) : graphics(gfx) {
    keyboardSubscriber = new SysEventsInternal::SubscriberInternal(graphics,SysEventsInternal::SubscriberInternal::EventType::KEYBOARD);
    mouseSubscriber = new SysEventsInternal::SubscriberInternal(graphics,SysEventsInternal::SubscriberInternal::EventType::MOUSE);
    controllerSubscriber = new SysEventsInternal::SubscriberInternal(graphics,SysEventsInternal::SubscriberInternal::EventType::CONTROLLER);
    textSubscriber = new SysEventsInternal::SubscriberInternal(graphics,SysEventsInternal::SubscriberInternal::EventType::TEXTINPUT);

    SysEventsInternal::subscribe(keyboardSubscriber);
    SysEventsInternal::subscribe(mouseSubscriber);
    SysEventsInternal::subscribe(controllerSubscriber);
    SysEventsInternal::subscribe(textSubscriber);

    openControllers.clear();
    inputs.clear();

    textInput = "";
}

InputManagerInternal::~InputManagerInternal() {
    for (int i = 0; i < (int)openControllers.size(); i++) {
        delete openControllers[i];
    }
    openControllers.clear();

    SysEventsInternal::unsubscribe(keyboardSubscriber);
    SysEventsInternal::unsubscribe(mouseSubscriber);
    SysEventsInternal::unsubscribe(controllerSubscriber);
    SysEventsInternal::unsubscribe(textSubscriber);

    delete keyboardSubscriber;
    delete mouseSubscriber;
    delete controllerSubscriber;
    delete textSubscriber;
}

void InputManagerInternal::startTextInputCapture() const {
    SDL_StartTextInput();
}

void InputManagerInternal::stopTextInputCapture() const {
    SDL_StopTextInput();
}

const String& InputManagerInternal::getTextInput() const {
    return textInput;
}

void InputManagerInternal::setClipboardText(const String& str) const {
    SDL_SetClipboardText(str.cstr());
}

String InputManagerInternal::getClipboardText() const {
    return String(SDL_GetClipboardText());
}

ControllerInternal::ControllerInternal(const InputManagerInternal& io, SDL_GameController& sdlController)
    : io(io), sdlController(sdlController) {
    removed = false;
    name = SDL_GameControllerName(&sdlController);
}

ControllerInternal::~ControllerInternal() {
    SDL_GameControllerClose(&sdlController);
}

const String& ControllerInternal::getName() const {
    return name;
}

void ControllerInternal::rumble(float lowFreqIntensity,float highFreqIntensity,int durationMs) {
    int lfiUshort = (int)(lowFreqIntensity*((float)0xffff));
    int hfiUshort = (int)(highFreqIntensity*((float)0xffff));
    SDL_GameControllerRumble(&sdlController, lfiUshort>0xffff ? 0xffff : (Uint16)lfiUshort, hfiUshort>0xffff ? 0xffff : (Uint16)hfiUshort, durationMs);
}

SDL_GameController& ControllerInternal::getSdlController() const {
    return sdlController;
}

void ControllerInternal::setName(const String& inName) {
    name = inName;
}

void InputManagerInternal::update() {
    textInput = "";
    for (Input* input : inputs) {
        input->setHit(false);
        input->setReleased(false);

        if (input->getDevice() == Input::Device::MOUSE) {
            MouseInput* mouse = (MouseInput*)input;
            mouse->setClickCount(0);
        }
    }

    SDL_Event event;
    while (((SysEventsInternal::SubscriberInternal*)keyboardSubscriber)->popEvent(event)) {
        SDL_KeyboardEvent keyEvent = event.key;
        for (Input* input : inputs) {
            if (input->getDevice()==Input::Device::KEYBOARD) {
                KeyboardInput* keyboardInput = (KeyboardInput*)input;
                if ((int)keyEvent.keysym.sym==(int)keyboardInput->getButton()) {
                    if (event.type == SDL_KEYDOWN) {
                        if (!input->isDown()) { input->setHit(true); }
                        input->setDown(true);
                    } else if (event.type == SDL_KEYUP) {
                        if (input->isDown()) { input->setReleased(true); }
                        input->setDown(false);
                    }
                }
            }
        }
    }

    while (((SysEventsInternal::SubscriberInternal*)mouseSubscriber)->popEvent(event)) {
        if (event.type==SDL_MOUSEMOTION) {
#if defined(__APPLE__) && defined(__OBJC__)
            // Get the mouse position from NSWindow on macOS.
            NSWindow* nsWin = ((WindowInternal*) window)->getCocoaWindow();
            NSPoint pos = nsWin.mouseLocationOutsideOfEventStream;
            pos = [nsWin convertPointToBacking: pos];

            mousePos.x = (float)pos.x;
            // Cocoa's origin is bottom-left, convert to top-left.
            mousePos.y = (float)window->getHeight() - pos.y;

            // TODO: mouseDelta on OS X.
 #else
            mousePos.x = (float)event.motion.x;
            mousePos.y = (float)event.motion.y;
            mouseDelta.x += (float)event.motion.xrel;
            mouseDelta.y += (float)event.motion.yrel;
 #endif
        } else if (event.type == SDL_MOUSEWHEEL) {
            mouseWheelPos.x -= event.wheel.x;
            mouseWheelPos.y -= event.wheel.y;
            // If the direction is flipped multiply by -1.
            if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
                mouseWheelPos = -mouseWheelPos;
            }
        } else if (event.type==SDL_MOUSEBUTTONDOWN || event.type==SDL_MOUSEBUTTONUP) {
            SDL_MouseButtonEvent mouseButtonEvent = event.button;
            for (Input* input : inputs) {
                if (input->getDevice()==Input::Device::MOUSE) {
                    MouseInput* mouseInput = (MouseInput*)input;
                    MouseInput::Button button;
                    switch (mouseButtonEvent.button) {
                        case SDL_BUTTON_LEFT: {
                            button = MouseInput::Button::LEFT;
                        } break;
                        case SDL_BUTTON_RIGHT: {
                            button = MouseInput::Button::RIGHT;
                        } break;
                        case SDL_BUTTON_MIDDLE: {
                            button = MouseInput::Button::MIDDLE;
                        } break;
                        case SDL_BUTTON_X1: {
                            button = MouseInput::Button::SIDE1;
                        } break;
                        case SDL_BUTTON_X2: {
                            button = MouseInput::Button::SIDE2;
                        } break;
                        default: {
                            continue;
                        } break;
                    }
                    if (button==mouseInput->getButton()) {
                        if (event.type==SDL_MOUSEBUTTONDOWN) {
                            if (!input->isDown()) { input->setHit(true); }
                            input->setDown(true);
                            mouseInput->setClickCount(mouseButtonEvent.clicks);
                        } else if (event.type==SDL_MOUSEBUTTONUP) {
                            if (input->isDown()) { input->setReleased(true); }
                            input->setDown(false);
                        }
                    }
                }
            }
        }
    }

    while (((SysEventsInternal::SubscriberInternal*)controllerSubscriber)->popEvent(event)) {
        if (event.type == SDL_CONTROLLERDEVICEADDED) {
            SDL_ControllerDeviceEvent deviceEvent = event.cdevice;
            SDL_GameController* sdlController = SDL_GameControllerOpen(deviceEvent.which);
            openControllers.emplace_back(new ControllerInternal(*this, *sdlController));
        } else if (event.type == SDL_CONTROLLERDEVICEREMAPPED) {
            SDL_ControllerDeviceEvent deviceEvent = event.cdevice;
            SDL_GameController* sdlController = SDL_GameControllerOpen(deviceEvent.which);
            for (int i = 0; i < (int)openControllers.size(); i++) {
                if (&openControllers[i]->getSdlController() == sdlController) {
                    openControllers[i]->setName(SDL_GameControllerName(sdlController));
                    break;
                }
            }
        } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
            SDL_ControllerDeviceEvent deviceEvent = event.cdevice;
            SDL_GameController* sdlController = SDL_GameControllerFromInstanceID(deviceEvent.which);
            for (int i = 0; i < (int)openControllers.size(); i++) {
                if (&openControllers[i]->getSdlController() == sdlController) {
                    for (Input* input : inputs) {
                        if (input->getDevice()==Input::Device::CONTROLLER) {
                            ControllerInput* controllerInput = (ControllerInput*)input;
                            if (controllerInput->getController() == openControllers[i]) {
                                controllerInput->removeController();
                            }
                        }
                    }
                    delete openControllers[i];
                    openControllers.erase(openControllers.begin()+i);
                    break;
                }
            }
        } else if (event.type == SDL_CONTROLLERAXISMOTION) {
            SDL_ControllerAxisEvent axisEvent = event.caxis;
            SDL_GameController* sdlController = SDL_GameControllerFromInstanceID(axisEvent.which);
            for (Input* input : inputs) {
                if (input->getDevice()==Input::Device::CONTROLLER) {
                    ControllerInput* controllerInput = (ControllerInput*)input;
                    if (controllerInput->getController() == nullptr) { continue; }
                    if (&((ControllerInternal*)controllerInput->getController())->getSdlController() != sdlController) { continue; }
                    ControllerInput::Button button = ControllerInput::Button::INVALID;
                    switch (axisEvent.axis) {
                        case SDL_CONTROLLER_AXIS_LEFTX:
                        case SDL_CONTROLLER_AXIS_LEFTY: {
                            button = ControllerInput::Button::LEFTSTICK;
                        } break;
                        case SDL_CONTROLLER_AXIS_RIGHTX:
                        case SDL_CONTROLLER_AXIS_RIGHTY: {
                            button = ControllerInput::Button::RIGHTSTICK;
                        } break;
                        case SDL_CONTROLLER_AXIS_TRIGGERLEFT: {
                            button = ControllerInput::Button::LEFTTRIGGER;
                        } break;
                        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: {
                            button = ControllerInput::Button::RIGHTTRIGGER;
                        } break;
                    }

                    if (button == controllerInput->getButton()) {
                        switch (axisEvent.axis) {
                            case SDL_CONTROLLER_AXIS_LEFTX:
                            case SDL_CONTROLLER_AXIS_RIGHTX: {
                                Vector2f newStickPosition = controllerInput->getStickPosition();
                                newStickPosition.x = (((float)axisEvent.value)+0.5f)/32767.5f;
                                controllerInput->setStickPosition(newStickPosition);
                            } break;
                            case SDL_CONTROLLER_AXIS_LEFTY:
                            case SDL_CONTROLLER_AXIS_RIGHTY: {
                                Vector2f newStickPosition = controllerInput->getStickPosition();
                                newStickPosition.y = (((float)axisEvent.value)+0.5f)/32767.5f;
                                controllerInput->setStickPosition(newStickPosition);
                            } break;
                            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: {
                                bool prevDown = controllerInput->isDown();
                                controllerInput->setPressDepth((((float)axisEvent.value)+0.5f)/32767.5f);
                                if (!prevDown && controllerInput->isDown()) { input->setHit(true); }
                                else if (prevDown && !controllerInput->isDown()) { input->setReleased(true); }
                            } break;
                        }
                    }
                }
            }
        } else if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) {
            SDL_ControllerButtonEvent buttonEvent = event.cbutton;
            SDL_GameController* sdlController = SDL_GameControllerFromInstanceID(buttonEvent.which);
            for (Input* input : inputs) {
                if (input->getDevice()==Input::Device::CONTROLLER) {
                    ControllerInput* controllerInput = (ControllerInput*)input;
                    if (controllerInput->getController() == nullptr) { continue; }
                    if (&((ControllerInternal*)controllerInput->getController())->getSdlController() != sdlController) { continue; }
                    ControllerInput::Button button = ControllerInput::Button::INVALID;
                    switch (buttonEvent.button) {
                        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: {
                            button = ControllerInput::Button::LEFTBUMPER;
                        } break;
                        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: {
                            button = ControllerInput::Button::RIGHTBUMPER;
                        } break;
                        case SDL_CONTROLLER_BUTTON_A: {
                            button = ControllerInput::Button::A;
                        } break;
                        case SDL_CONTROLLER_BUTTON_B: {
                            button = ControllerInput::Button::B;
                        } break;
                        case SDL_CONTROLLER_BUTTON_X: {
                            button = ControllerInput::Button::X;
                        } break;
                        case SDL_CONTROLLER_BUTTON_Y: {
                            button = ControllerInput::Button::Y;
                        } break;
                        case SDL_CONTROLLER_BUTTON_BACK: {
                            button = ControllerInput::Button::BACK;
                        } break;
                        case SDL_CONTROLLER_BUTTON_GUIDE: {
                            button = ControllerInput::Button::GUIDE;
                        } break;
                        case SDL_CONTROLLER_BUTTON_START: {
                            button = ControllerInput::Button::START;
                        } break;
                        case SDL_CONTROLLER_BUTTON_LEFTSTICK: {
                            button = ControllerInput::Button::LEFTSTICK;
                        } break;
                        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: {
                            button = ControllerInput::Button::RIGHTSTICK;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_UP: {
                            button = ControllerInput::Button::DPAD_UP;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: {
                            button = ControllerInput::Button::DPAD_LEFT;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: {
                            button = ControllerInput::Button::DPAD_DOWN;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: {
                            button = ControllerInput::Button::DPAD_RIGHT;
                        } break;
                    }

                    if (button == controllerInput->getButton()) {
                        if (event.type==SDL_CONTROLLERBUTTONDOWN) {
                            if (!input->isDown()) { input->setHit(true); }
                            input->setDown(true);
                        } else if (event.type==SDL_CONTROLLERBUTTONUP) {
                            if (input->isDown()) { input->setReleased(true); }
                            input->setDown(false);
                        }
                    }
                } 
            }
        }
    }

    while (((SysEventsInternal::SubscriberInternal*)textSubscriber)->popEvent(event)) {
        SDL_TextInputEvent txtEvent = event.text;
        textInput = textInput + txtEvent.text;
    }
}

const Vector2f& InputManagerInternal::getMousePosition() const {
    return mousePos;
}

const Vector2f InputManagerInternal::consumeMouseDelta() {
    Vector2f ret = mouseDelta;
    mouseDelta = Vector2f();
    return ret;
}

void InputManagerInternal::setMousePosition(const Vector2f& position) {
    if (!graphics.isWindowFocused()) { return; }

    mousePos = position;

#if defined(__APPLE__) && defined(__OBJC__)
    // Convert the mouse position from the retina coordinates to screen coordinates on macOS.
    NSWindow* nsWin = ((WindowInternal*) window)->getCocoaWindow();
    NSPoint mousePosition = [nsWin convertPointFromBacking: NSMakePoint(position.x, position.y)];

    // Mouse can only be set using display coordinates, so offset with the window position to get window coordinates.
    Vector2i windowPosition;
    SDL_GetWindowPosition(((WindowInternal*) window)->getSdlWindow(), &windowPosition.x, &windowPosition.y);

    mousePosition = NSMakePoint(mousePosition.x + windowPosition.x, mousePosition.y + windowPosition.y);
    CGWarpMouseCursorPosition(mousePosition);
    // For some reason updating the mouse position this way doesn't update the cursor position, so we need to tell Cocoa to sync that.
    CGAssociateMouseAndMouseCursorPosition(true);
#else
    SDL_WarpMouseInWindow(((GraphicsInternal&)graphics).getWindow(), (int)position.x, (int)position.y);
#endif
}

void InputManagerInternal::setMouseRelativeInput(bool relative) {
    int res = SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE);
    // It's fine if it's not available (-1).
    PGE_ASSERT(res == 0 || res == -1, "Could not set relative mouse input (SDLERROR: " + String(SDL_GetError()) + ")");
}

Vector2i InputManagerInternal::consumeMouseWheelDelta() {
    Vector2i mwp = mouseWheelPos;
    mouseWheelPos = Vectors::ZERO2I;
    return mwp;
}

// TODO: Keep all contents unique to increase performance?
void InputManagerInternal::trackInput(Input* input) {
    inputs.emplace(input);
}

void InputManagerInternal::untrackInput(Input* input) {
    std::unordered_set<Input*>::iterator it = inputs.find(input);
    if (it != inputs.end()) {
        inputs.erase(it);
    }
}

int InputManagerInternal::getControllerCount() const {
    return (int)openControllers.size();
}

Controller* InputManagerInternal::getController(int index) const {
    return index >= 0 && index < (int)openControllers.size() ? openControllers[index] : nullptr;
}

bool InputManagerInternal::isControllerValid(Controller* controller) const {
    for (int i = 0; i < (int)openControllers.size(); i++) {
        if (openControllers[i]==controller) { return true; }
    }
    return false;
}
