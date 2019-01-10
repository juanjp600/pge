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
        SysEvents::Subscriber keyboardSubscriber;
        SysEvents::Subscriber mouseSubscriber;
        SysEvents::Subscriber gamepadSubscriber;

        std::set<UserInput*> inputs;
        Vector2i mousePos;

        IO(Window* window);
    public:
        static IO* create(Window* window);
        ~IO();

        void update();

        void trackInput(UserInput* input);
        void untrackInput(UserInput* input);

        Vector2i getMousePosition() const;
};

}

#endif

