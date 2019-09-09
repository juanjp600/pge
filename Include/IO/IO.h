#ifndef PGE_IO_H_INCLUDED
#define PGE_IO_H_INCLUDED

#include <SysEvents/SysEvents.h>
#include <UserInput/UserInput.h>
#include <Window/Window.h>

#include <Math/Vector.h>

#include <set>

namespace PGE {

class IO {
    private:
        Window* window;

        SysEvents::Subscriber* keyboardSubscriber;
        SysEvents::Subscriber* mouseSubscriber;
        SysEvents::Subscriber* controllerSubscriber;
        SysEvents::Subscriber* textSubscriber;

        std::set<UserInput*> inputs;
        Vector2f mousePos;
    
        // The last received string from SDL_TEXTINPUT events.
        String textInput;

        IO(Window* win);
    public:
        static IO* create(Window* window);
        ~IO();

        void update();

        void trackInput(UserInput* input);
        void untrackInput(UserInput* input);

        Vector2f getMousePosition() const;
        void setMousePosition(Vector2f position);
        void setMouseVisibility(bool visible);
    
        void startTextInputCapture() const;
        void stopTextInputCapture() const;
        String getTextInput() const;
    
        void setClipboardText(String str) const;
        String getClipboardText() const;
};

}

#endif

