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
            mousePos.x = event.motion.x;
            mousePos.y = event.motion.y;
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

Vector2i IO::getMousePosition() const {
    return mousePos;
}

void IO::setMousePosition(Vector2i position) {
    if (!window->isFocused()) { return; }
    SDL_WarpMouseInWindow(((WindowInternal*)window)->getSdlWindow(),position.x,position.y);
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
