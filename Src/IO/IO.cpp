#if defined(__APPLE__) && defined(__OBJC__)
#import <AppKit/AppKit.h>
#include <SDL_syswm.h>
#endif

#include "../SysEvents/SysEventsInternal.h"
#include <IO/IO.h>
#include "IOInternal.h"
#include <Exception/Exception.h>
#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsInternal.h"

using namespace PGE;

IO* IO::create(Graphics* graphics) {
    return new IOInternal(graphics);
}

IOInternal::IOInternal(Graphics* gfx) {
    graphics = gfx;

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

IOInternal::~IOInternal() {
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

void IOInternal::startTextInputCapture() const {
    SDL_StartTextInput();
}

void IOInternal::stopTextInputCapture() const {
    SDL_StopTextInput();
}

String IOInternal::getTextInput() const {
    return textInput;
}

void IOInternal::setClipboardText(const String& str) const {
    SDL_SetClipboardText(str.cstr());
}

String IOInternal::getClipboardText() const {
    return String(SDL_GetClipboardText());
}

ControllerInternal::ControllerInternal(const IOInternal* inIo, SDL_GameController* inSdlController) {
    removed = false;
    io = inIo;
    sdlController = inSdlController;
    name = SDL_GameControllerName(inSdlController);
}

ControllerInternal::~ControllerInternal() {
    SDL_GameControllerClose(sdlController);
}

const String& ControllerInternal::getName() const {
    return name;
}

void ControllerInternal::rumble(float lowFreqIntensity,float highFreqIntensity,int durationMs) {
    int lfiUshort = (int)(lowFreqIntensity*((float)0xffff));
    int hfiUshort = (int)(highFreqIntensity*((float)0xffff));
    SDL_GameControllerRumble(sdlController, lfiUshort>0xffff ? 0xffff : lfiUshort, hfiUshort>0xffff ? 0xffff : hfiUshort, durationMs);
}

SDL_GameController* ControllerInternal::getSdlController() const {
    return sdlController;
}

void ControllerInternal::setName(const String& inName) {
    name = inName;
}

void IOInternal::update() {
    textInput = "";
    for (UserInput* input : inputs) {
        input->setHit(false);

        if (input->getDevice() == UserInput::DEVICE::MOUSE) {
            MouseInput* mouse = (MouseInput*)input;
            mouse->setClickCount(0);
        }
    }

    SDL_Event event;
    while (((SysEventsInternal::SubscriberInternal*)keyboardSubscriber)->popEvent(event)) {
        SDL_KeyboardEvent keyEvent = event.key;
        for (UserInput* input : inputs) {
            if (input->getDevice()==UserInput::DEVICE::KEYBOARD) {
                KeyboardInput* keyboardInput = (KeyboardInput*)input;
                if ((int)keyEvent.keysym.sym==(int)keyboardInput->getButton()) {
                    if (event.type == SDL_KEYDOWN) {
                        if (!input->isDown()) { input->setHit(true); }
                        input->setDown(true);
                    } else if (event.type == SDL_KEYUP) {
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
 #else
            mousePos.x = (float)event.motion.x;
            mousePos.y = (float)event.motion.y;
 #endif
        } else if (event.type == SDL_MOUSEWHEEL) {
            mouseWheelPos.x -= event.wheel.x;
            mouseWheelPos.y -= event.wheel.y;
            // If the direction is flipped multiply by -1.
            if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
                mouseWheelPos.multiply(-1);
            }
        } else if (event.type==SDL_MOUSEBUTTONDOWN || event.type==SDL_MOUSEBUTTONUP) {
            SDL_MouseButtonEvent mouseButtonEvent = event.button;
            for (UserInput* input : inputs) {
                if (input->getDevice()==UserInput::DEVICE::MOUSE) {
                    MouseInput* mouseInput = (MouseInput*)input;
                    MouseInput::BUTTON button;
                    switch (mouseButtonEvent.button) {
                        case SDL_BUTTON_LEFT: {
                            button = MouseInput::BUTTON::LEFT;
                        } break;
                        case SDL_BUTTON_RIGHT: {
                            button = MouseInput::BUTTON::RIGHT;
                        } break;
                        case SDL_BUTTON_MIDDLE: {
                            button = MouseInput::BUTTON::MIDDLE;
                        } break;
                        case SDL_BUTTON_X1: {
                            button = MouseInput::BUTTON::SIDE1;
                        } break;
                        case SDL_BUTTON_X2: {
                            button = MouseInput::BUTTON::SIDE2;
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
            openControllers.push_back(new ControllerInternal(this, sdlController));
        } else if (event.type == SDL_CONTROLLERDEVICEREMAPPED) {
            SDL_ControllerDeviceEvent deviceEvent = event.cdevice;
            SDL_GameController* sdlController = SDL_GameControllerOpen(deviceEvent.which);
            for (int i = 0; i < (int)openControllers.size(); i++) {
                if (openControllers[i]->getSdlController() == sdlController) {
                    openControllers[i]->setName(SDL_GameControllerName(sdlController));
                    break;
                }
            }
        } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
            SDL_ControllerDeviceEvent deviceEvent = event.cdevice;
            SDL_GameController* sdlController = SDL_GameControllerFromInstanceID(deviceEvent.which);
            for (int i = 0; i < (int)openControllers.size(); i++) {
                if (openControllers[i]->getSdlController() == sdlController) {
                    for (UserInput* input : inputs) {
                        if (input->getDevice()==UserInput::DEVICE::CONTROLLER) {
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
            for (UserInput* input : inputs) {
                if (input->getDevice()==UserInput::DEVICE::CONTROLLER) {
                    ControllerInput* controllerInput = (ControllerInput*)input;
                    if (controllerInput->getController() == nullptr) { continue; }
                    if (((ControllerInternal*)controllerInput->getController())->getSdlController() != sdlController) { continue; }
                    ControllerInput::BUTTON button = ControllerInput::BUTTON::INVALID;
                    switch (axisEvent.axis) {
                        case SDL_CONTROLLER_AXIS_LEFTX:
                        case SDL_CONTROLLER_AXIS_LEFTY: {
                            button = ControllerInput::BUTTON::LEFTSTICK;
                        } break;
                        case SDL_CONTROLLER_AXIS_RIGHTX:
                        case SDL_CONTROLLER_AXIS_RIGHTY: {
                            button = ControllerInput::BUTTON::RIGHTSTICK;
                        } break;
                        case SDL_CONTROLLER_AXIS_TRIGGERLEFT: {
                            button = ControllerInput::BUTTON::LEFTTRIGGER;
                        } break;
                        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: {
                            button = ControllerInput::BUTTON::RIGHTTRIGGER;
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
                            } break;
                        }
                    }
                }
            }
        } else if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) {
            SDL_ControllerButtonEvent buttonEvent = event.cbutton;
            SDL_GameController* sdlController = SDL_GameControllerFromInstanceID(buttonEvent.which);
            for (UserInput* input : inputs) {
                if (input->getDevice()==UserInput::DEVICE::CONTROLLER) {
                    ControllerInput* controllerInput = (ControllerInput*)input;
                    if (controllerInput->getController() == nullptr) { continue; }
                    if (((ControllerInternal*)controllerInput->getController())->getSdlController() != sdlController) { continue; }
                    ControllerInput::BUTTON button = ControllerInput::BUTTON::INVALID;
                    switch (buttonEvent.button) {
                        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: {
                            button = ControllerInput::BUTTON::LEFTBUMPER;
                        } break;
                        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: {
                            button = ControllerInput::BUTTON::RIGHTBUMPER;
                        } break;
                        case SDL_CONTROLLER_BUTTON_A: {
                            button = ControllerInput::BUTTON::A;
                        } break;
                        case SDL_CONTROLLER_BUTTON_B: {
                            button = ControllerInput::BUTTON::B;
                        } break;
                        case SDL_CONTROLLER_BUTTON_X: {
                            button = ControllerInput::BUTTON::X;
                        } break;
                        case SDL_CONTROLLER_BUTTON_Y: {
                            button = ControllerInput::BUTTON::Y;
                        } break;
                        case SDL_CONTROLLER_BUTTON_BACK: {
                            button = ControllerInput::BUTTON::BACK;
                        } break;
                        case SDL_CONTROLLER_BUTTON_GUIDE: {
                            button = ControllerInput::BUTTON::GUIDE;
                        } break;
                        case SDL_CONTROLLER_BUTTON_START: {
                            button = ControllerInput::BUTTON::START;
                        } break;
                        case SDL_CONTROLLER_BUTTON_LEFTSTICK: {
                            button = ControllerInput::BUTTON::LEFTSTICK;
                        } break;
                        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: {
                            button = ControllerInput::BUTTON::RIGHTSTICK;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_UP: {
                            button = ControllerInput::BUTTON::DPAD_UP;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: {
                            button = ControllerInput::BUTTON::DPAD_LEFT;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: {
                            button = ControllerInput::BUTTON::DPAD_DOWN;
                        } break;
                        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: {
                            button = ControllerInput::BUTTON::DPAD_RIGHT;
                        } break;
                    }

                    if (button == controllerInput->getButton()) {
                        if (event.type==SDL_CONTROLLERBUTTONDOWN) {
                            if (!input->isDown()) { input->setHit(true); }
                            input->setDown(true);
                        } else if (event.type==SDL_CONTROLLERBUTTONUP) {
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

const Vector2f& IOInternal::getMousePosition() const {
    return mousePos;
}

void IOInternal::setMousePosition(const Vector2f& position) {
    if (!graphics->isWindowFocused()) { return; }

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
    SDL_WarpMouseInWindow(((GraphicsInternal*)graphics)->getSdlWindow(), (int)position.x, (int)position.y);
#endif
}

void IOInternal::setMouseVisibility(bool visible) {
    SDL_ShowCursor(visible ? 1 : 0);
}

Vector2i IOInternal::getMouseWheelDelta() {
    Vector2i mwp = mouseWheelPos;
    mouseWheelPos = Vector2i::zero;
    return mwp;
}

// TODO: Keep all contents unique to increase performance?
void IOInternal::trackInput(UserInput* input) {
    inputs.emplace(input);
}

void IOInternal::untrackInput(UserInput* input) {
    std::unordered_set<UserInput*>::iterator it = inputs.find(input);
    if (it != inputs.end()) {
        inputs.erase(it);
    }
}

int IOInternal::getControllerCount() const {
    return openControllers.size();
}

Controller* IOInternal::getController(int index) const {
    return index >= 0 && index < (int)openControllers.size() ? openControllers[index] : nullptr;
}

bool IOInternal::isControllerValid(Controller* controller) const {
    for (int i = 0; i < (int)openControllers.size(); i++) {
        if (openControllers[i]==controller) { return true; }
    }
    return false;
}
