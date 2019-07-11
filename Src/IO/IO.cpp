#ifdef __APPLE__
#ifdef __OBJC__
#import <AppKit/AppKit.h>
#include <SDL_syswm.h>
#endif
#endif

#include "../SysEvents/SysEventsInternal.h"
#include <Window/Window.h>
#include "../Window/WindowInternal.h"
#include <IO/IO.h>

using namespace PGE;

IO* IO::create(Window* window) {
    return new IO(window);
}

IO::IO(Window* win) {
    window = win;

    keyboardSubscriber = new SysEventsInternal::SubscriberInternal(window,SysEventsInternal::SubscriberInternal::EventType::KEYBOARD);
    mouseSubscriber = new SysEventsInternal::SubscriberInternal(window,SysEventsInternal::SubscriberInternal::EventType::MOUSE);
    gamepadSubscriber = new SysEventsInternal::SubscriberInternal(window,SysEventsInternal::SubscriberInternal::EventType::GAMEPAD);

    SysEventsInternal::subscribe(keyboardSubscriber);
    SysEventsInternal::subscribe(mouseSubscriber);
    SysEventsInternal::subscribe(gamepadSubscriber);

    inputs.clear();
}

IO::~IO() {
    SysEventsInternal::unsubscribe(keyboardSubscriber);
    SysEventsInternal::unsubscribe(mouseSubscriber);
    SysEventsInternal::unsubscribe(gamepadSubscriber);
}

void IO::update() {
    SDL_Event event;
    while (((SysEventsInternal::SubscriberInternal*)keyboardSubscriber)->popEvent(event)) {
        SDL_KeyboardEvent keyEvent = event.key;
        for (std::set<UserInput*>::iterator it=inputs.begin();it!=inputs.end();it++) {
            UserInput* input = (*it);
            if (input->getDevice()==UserInput::DEVICE::KEYBOARD) {
                KeyboardInput* keyboardInput = (KeyboardInput*)input;
                if ((int)keyEvent.keysym.scancode==(int)keyboardInput->getButton()) {
                    if (event.type == SDL_KEYDOWN) {
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
 #ifdef __APPLE__
 #ifdef __OBJC__
            // Get the mouse position from NSWindow on macOS.
            NSWindow* nsWin = ((WindowInternal*) window)->getCocoaWindow();
            NSPoint pos = nsWin.mouseLocationOutsideOfEventStream;
            pos = [nsWin convertPointToBacking: pos];

            mousePos.x = (float)pos.x;
            mousePos.y = (float)pos.y;
 #endif
 #else
            mousePos.x = (float)event.motion.x;
            mousePos.y = (float)event.motion.y;
 #endif
        } else if (event.type==SDL_MOUSEBUTTONDOWN || event.type==SDL_MOUSEBUTTONUP) {
            SDL_MouseButtonEvent mouseButtonEvent = event.button;
            for (std::set<UserInput*>::iterator it=inputs.begin();it!=inputs.end();it++) {
                UserInput* input = (*it);
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
                            input->setDown(true);
                        } else if (event.type==SDL_MOUSEBUTTONUP) {
                            input->setDown(false);
                        }
                    }
                }
            }
        }
    }
}

Vector2f IO::getMousePosition() const {
    return mousePos;
}

void IO::setMousePosition(Vector2f position) {
    if (!window->isFocused()) { return; }

    Vector2f sdlPosition = position;
#ifdef __APPLE__
#ifdef __OBJC__
    // Get the mouse position from NSWindow on macOS.
    NSWindow* nsWin = ((WindowInternal*) window)->getCocoaWindow();
    NSPoint pos = NSMakePoint(sdlPosition.x, sdlPosition.y);
    pos = [nsWin convertPointFromBacking: pos];

    sdlPosition.x = (float)pos.x;
    sdlPosition.y = (float)pos.y;
#endif
#endif

    SDL_WarpMouseInWindow(((WindowInternal*)window)->getSdlWindow(),sdlPosition.x,sdlPosition.y);
    mousePos = position;
}

void IO::setMouseVisibility(bool visible) {
    SDL_ShowCursor(visible ? 1 : 0);
}

void IO::trackInput(UserInput* input) {
    inputs.emplace(input);
}

void IO::untrackInput(UserInput* input) {
    std::set<UserInput*>::iterator it = inputs.find(input);
    if (it != inputs.end()) {
        inputs.erase(it);
    }
}
