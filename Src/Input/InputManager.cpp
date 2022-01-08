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
    using enum EventType;
    keyboardSubscriber = new SysEventsInternal::SubscriberInternal(graphics, KEYBOARD);
    mouseSubscriber = new SysEventsInternal::SubscriberInternal(graphics, MOUSE);
    controllerSubscriber = new SysEventsInternal::SubscriberInternal(graphics, CONTROLLER);
    textSubscriber = new SysEventsInternal::SubscriberInternal(graphics, TEXTINPUT);

    SysEventsInternal::subscribe(keyboardSubscriber);
    SysEventsInternal::subscribe(mouseSubscriber);
    SysEventsInternal::subscribe(controllerSubscriber);
    SysEventsInternal::subscribe(textSubscriber);

    openControllers.clear();
    inputs.clear();

    textInput = "";
}

InputManagerInternal::~InputManagerInternal() {
    for (ControllerInternal* ci : openControllers) {
        delete ci;
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
    using enum Device;
    using enum ControllerButton;

    textInput = "";
    for (Input* input : inputs) {
        input->setHit(false);
        input->setReleased(false);

        if (input->getDevice() == MOUSE) {
            MouseInput* mouse = (MouseInput*)input;
            mouse->setClickCount(0);
        }
    }

    SDL_Event event;
    while (((SysEventsInternal::SubscriberInternal*)keyboardSubscriber)->popEvent(event)) {
        SDL_KeyboardEvent keyEvent = event.key;
        for (Input* input : inputs) {
            if (input->getDevice()==KEYBOARD) {
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
                if (input->getDevice()==MOUSE) {
                    MouseInput* mouseInput = (MouseInput*)input;
                    MouseButton button;
                    switch (mouseButtonEvent.button) {
                        using enum MouseButton;
                        case SDL_BUTTON_LEFT: {
                            button = LEFT;
                        } break;
                        case SDL_BUTTON_RIGHT: {
                            button = RIGHT;
                        } break;
                        case SDL_BUTTON_MIDDLE: {
                            button = MIDDLE;
                        } break;
                        case SDL_BUTTON_X1: {
                            button = SIDE1;
                        } break;
                        case SDL_BUTTON_X2: {
                            button = SIDE2;
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
            for (ControllerInternal* c : openControllers) {
                if (&c->getSdlController() == sdlController) {
                    c->setName(SDL_GameControllerName(sdlController));
                    break;
                }
            }
        } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
            SDL_ControllerDeviceEvent deviceEvent = event.cdevice;
            SDL_GameController* sdlController = SDL_GameControllerFromInstanceID(deviceEvent.which);
            for (auto it = openControllers.begin(); it != openControllers.end(); it++) {
                if ((&(*it)->getSdlController()) == sdlController) {
                    for (Input* input : inputs) {
                        if (input->getDevice()==CONTROLLER) {
                            ControllerInput* controllerInput = (ControllerInput*)input;
                            if (controllerInput->getController() == *it) {
                                controllerInput->removeController();
                            }
                        }
                    }
                    delete *it;
                    openControllers.erase(it);
                    break;
                }
            }
        } else if (event.type == SDL_CONTROLLERAXISMOTION) {
            SDL_ControllerAxisEvent axisEvent = event.caxis;
            SDL_GameController* sdlController = SDL_GameControllerFromInstanceID(axisEvent.which);
            for (Input* input : inputs) {
                if (input->getDevice()==CONTROLLER) {
                    ControllerInput* controllerInput = (ControllerInput*)input;
                    if (controllerInput->getController() == nullptr) { continue; }
                    if (&((ControllerInternal*)controllerInput->getController())->getSdlController() != sdlController) { continue; }
                    ControllerButton button = INVALID;
                    switch (axisEvent.axis) {
                        case SDL_CONTROLLER_AXIS_LEFTX:
                        case SDL_CONTROLLER_AXIS_LEFTY: {
                            button = LEFTSTICK;
                        } break;
                        case SDL_CONTROLLER_AXIS_RIGHTX:
                        case SDL_CONTROLLER_AXIS_RIGHTY: {
                            button = RIGHTSTICK;
                        } break;
                        case SDL_CONTROLLER_AXIS_TRIGGERLEFT: {
                            button = LEFTTRIGGER;
                        } break;
                        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: {
                            button = RIGHTTRIGGER;
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
                if (input->getDevice()==CONTROLLER) {
                    ControllerInput* controllerInput = (ControllerInput*)input;
                    if (controllerInput->getController() == nullptr) { continue; }
                    if (&((ControllerInternal*)controllerInput->getController())->getSdlController() != sdlController) { continue; }
                    ControllerButton button = INVALID;
                    switch (buttonEvent.button) {
                        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: {
                            button = LEFTBUMPER;
                        } break;
                        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: {
                            button = RIGHTBUMPER;
                        } break;
                        case SDL_CONTROLLER_BUTTON_A: {
                            button = A;
                        } break;
                        case SDL_CONTROLLER_BUTTON_B: {
                            button = B;
                        } break;
                        case SDL_CONTROLLER_BUTTON_X: {
                            button = X;
                        } break;
                        case SDL_CONTROLLER_BUTTON_Y: {
                            button = Y;
                        } break;
                        case SDL_CONTROLLER_BUTTON_BACK: {
                            button = BACK;
                        } break;
                        case SDL_CONTROLLER_BUTTON_GUIDE: {
                            button = GUIDE;
                        } break;
                        case SDL_CONTROLLER_BUTTON_START: {
                            button = START;
                        } break;
                        case SDL_CONTROLLER_BUTTON_LEFTSTICK: {
                            button = LEFTSTICK;
                        } break;
                        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: {
                            button = RIGHTSTICK;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_UP: {
                            button = DPAD_UP;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: {
                            button = DPAD_LEFT;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: {
                            button = DPAD_DOWN;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: {
                            button = DPAD_RIGHT;
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

Vector2f InputManagerInternal::consumeMouseDelta() {
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
    asrt(res == 0 || res == -1, "Could not set relative mouse input (SDLERROR: " + String(SDL_GetError()) + ")");
}

Vector2i InputManagerInternal::consumeMouseWheelDelta() {
    Vector2i mwp = mouseWheelPos;
    mouseWheelPos = Vector2is::ZERO;
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
    for (Controller* c : openControllers) {
        if (c == controller) { return true; }
    }
    return false;
}
