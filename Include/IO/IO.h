#ifndef PGE_IO_H_INCLUDED
#define PGE_IO_H_INCLUDED

#include <SysEvents/SysEvents.h>
#include <UserInput/UserInput.h>
#include <Window/Window.h>

#include <Math/Vector.h>

#include <set>

namespace PGE {

class IO {
    public:
        static IO* create(Window* window);
        virtual ~IO() {}

        virtual void update() =0;

        virtual void trackInput(UserInput* input) =0;
        virtual void untrackInput(UserInput* input) =0;

        virtual int getControllerCount() const =0;
        virtual Controller* getController(int index) const =0;
        virtual bool isControllerValid(Controller* controller) const =0;

        virtual Vector2f getMousePosition() const =0;
        virtual void setMousePosition(Vector2f position) =0;
        virtual void setMouseVisibility(bool visible) =0;
    
        virtual void startTextInputCapture() const =0;
        virtual void stopTextInputCapture() const =0;
        virtual String getTextInput() const =0;
    
        virtual void setClipboardText(String str) const =0;
        virtual String getClipboardText() const =0;
};

}

#endif

