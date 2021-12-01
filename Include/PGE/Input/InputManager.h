#ifndef PGE_INPUTMANAGER_H_INCLUDED
#define PGE_INPUTMANAGER_H_INCLUDED

#include <PGE/SysEvents/SysEvents.h>
#include <PGE/Math/Vector.h>

#include "Input.h"

namespace PGE {

// TODO: C++20 Modules make monomorphic.
class InputManager : private PolymorphicHeap {
    public:
        static InputManager* create(const class Graphics& gfx);
        virtual ~InputManager() = default;

        virtual void update() = 0;

        virtual void trackInput(Input* input) = 0;
        virtual void untrackInput(Input* input) = 0;

        virtual int getControllerCount() const = 0;
        virtual Controller* getController(int index) const = 0;
        virtual bool isControllerValid(Controller* controller) const = 0;

        virtual const Vector2f& getMousePosition() const = 0;
        virtual const Vector2f consumeMouseDelta() = 0;
        virtual void setMousePosition(const Vector2f& position) = 0;
        virtual void setMouseRelativeInput(bool relative) = 0;
        virtual Vector2i consumeMouseWheelDelta() = 0;
    
        virtual void startTextInputCapture() const = 0;
        virtual void stopTextInputCapture() const = 0;
        virtual const String& getTextInput() const = 0;
    
        virtual void setClipboardText(const String& str) const = 0;
        virtual String getClipboardText() const = 0;
};

}

#endif // PGE_INPUTMANAGER_H_INCLUDED
