#ifndef PGE_INPUT_H_INCLUDED
#define PGE_INPUT_H_INCLUDED

#include <PGE/Math/Vector.h>
#include <PGE/String/String.h>

namespace PGE {

class Input {
    public:
        enum class Device {
            KEYBOARD,
            MOUSE,
            CONTROLLER
        };

        virtual ~Input() = default;

        virtual Device getDevice() const = 0;
        virtual int getKey() const = 0;

        bool isDown() const;
        bool isHit() const;
        bool isReleased() const;
        void setDown(bool down);
        void setHit(bool hit);
        void setReleased(bool released);

    protected:
        Input() = default;

        bool inputDown = false;
        bool inputHit = false;
        bool inputReleased = false;
};

class KeyboardInput : public Input {
    public:
        //carbon copy of SDL's keycode enum
        enum class Keycode {
            UNKNOWN = 0x00000000,
            RETURN = 0x0000000d,
            ESCAPE = 0x0000001b,
            BACKSPACE = 0x00000008,
            TAB = 0x00000009,
            SPACE = 0x00000020,
            EXCLAIM = 0x00000021,
            QUOTEDBL = 0x00000022,
            HASH = 0x00000023,
            PERCENT = 0x00000025,
            DOLLAR = 0x00000024,
            AMPERSAND = 0x00000026,
            QUOTE = 0x00000027,
            LEFTPAREN = 0x00000028,
            RIGHTPAREN = 0x00000029,
            ASTERISK = 0x0000002a,
            PLUS = 0x0000002b,
            COMMA = 0x0000002c,
            MINUS = 0x0000002d,
            PERIOD = 0x0000002e,
            SLASH = 0x0000002f,
            NUM0 = 0x00000030,
            NUM1 = 0x00000031,
            NUM2 = 0x00000032,
            NUM3 = 0x00000033,
            NUM4 = 0x00000034,
            NUM5 = 0x00000035,
            NUM6 = 0x00000036,
            NUM7 = 0x00000037,
            NUM8 = 0x00000038,
            NUM9 = 0x00000039,
            COLON = 0x0000003a,
            SEMICOLON = 0x0000003b,
            LESS = 0x0000003c,
            EQUALS = 0x0000003d,
            GREATER = 0x0000003e,
            QUESTION = 0x0000003f,
            AT = 0x00000040,
            LEFTBRACKET = 0x0000005b,
            BACKSLASH = 0x0000005c,
            RIGHTBRACKET = 0x0000005d,
            CARET = 0x0000005e,
            UNDERSCORE = 0x0000005f,
            BACKQUOTE = 0x00000060,
            A = 0x00000061,
            B = 0x00000062,
            C = 0x00000063,
            D = 0x00000064,
            E = 0x00000065,
            F = 0x00000066,
            G = 0x00000067,
            H = 0x00000068,
            I = 0x00000069,
            J = 0x0000006a,
            K = 0x0000006b,
            L = 0x0000006c,
            M = 0x0000006d,
            N = 0x0000006e,
            O = 0x0000006f,
            P = 0x00000070,
            Q = 0x00000071,
            R = 0x00000072,
            S = 0x00000073,
            T = 0x00000074,
            U = 0x00000075,
            V = 0x00000076,
            W = 0x00000077,
            X = 0x00000078,
            Y = 0x00000079,
            Z = 0x0000007a,
            CAPSLOCK = 0x40000039,
            F1 = 0x4000003a,
            F2 = 0x4000003b,
            F3 = 0x4000003c,
            F4 = 0x4000003d,
            F5 = 0x4000003e,
            F6 = 0x4000003f,
            F7 = 0x40000040,
            F8 = 0x40000041,
            F9 = 0x40000042,
            F10 = 0x40000043,
            F11 = 0x40000044,
            F12 = 0x40000045,
            PRINTSCREEN = 0x40000046,
            SCROLLLOCK = 0x40000047,
            PAUSE = 0x40000048,
            INSERT = 0x40000049,
            HOME = 0x4000004a,
            PAGEUP = 0x4000004b,
            DELETE = 0x0000007f,
            END = 0x4000004d,
            PAGEDOWN = 0x4000004e,
            RIGHT = 0x4000004f,
            LEFT = 0x40000050,
            DOWN = 0x40000051,
            UP = 0x40000052,
            NUMLOCKCLEAR = 0x40000053,
            KP_DIVIDE = 0x40000054,
            KP_MULTIPLY = 0x40000055,
            KP_MINUS = 0x40000056,
            KP_PLUS = 0x40000057,
            KP_ENTER = 0x40000058,
            KP_1 = 0x40000059,
            KP_2 = 0x4000005a,
            KP_3 = 0x4000005b,
            KP_4 = 0x4000005c,
            KP_5 = 0x4000005d,
            KP_6 = 0x4000005e,
            KP_7 = 0x4000005f,
            KP_8 = 0x40000060,
            KP_9 = 0x40000061,
            KP_0 = 0x40000062,
            KP_PERIOD = 0x40000063,
            APPLICATION = 0x40000065,
            POWER = 0x40000066,
            KP_EQUALS = 0x40000067,
            F13 = 0x40000068,
            F14 = 0x40000069,
            F15 = 0x4000006a,
            F16 = 0x4000006b,
            F17 = 0x4000006c,
            F18 = 0x4000006d,
            F19 = 0x4000006e,
            F20 = 0x4000006f,
            F21 = 0x40000070,
            F22 = 0x40000071,
            F23 = 0x40000072,
            F24 = 0x40000073,
            EXECUTE = 0x40000074,
            HELP = 0x40000075,
            MENU = 0x40000076,
            SELECT = 0x40000077,
            STOP = 0x40000078,
            AGAIN = 0x40000079,
            UNDO = 0x4000007a,
            CUT = 0x4000007b,
            COPY = 0x4000007c,
            PASTE = 0x4000007d,
            FIND = 0x4000007e,
            MUTE = 0x4000007f,
            VOLUMEUP = 0x40000080,
            VOLUMEDOWN = 0x40000081,
            KP_COMMA = 0x40000085,
            KP_EQUALSAS400 = 0x40000086,
            ALTERASE = 0x40000099,
            SYSREQ = 0x4000009a,
            CANCEL = 0x4000009b,
            CLEAR = 0x4000009c,
            PRIOR = 0x4000009d,
            RETURN2 = 0x4000009e,
            SEPARATOR = 0x4000009f,
            OUT = 0x400000a0,
            OPER = 0x400000a1,
            CLEARAGAIN = 0x400000a2,
            CRSEL = 0x400000a3,
            EXSEL = 0x400000a4,
            KP_00 = 0x400000b0,
            KP_000 = 0x400000b1,
            THOUSANDSSEPARATOR = 0x400000b2,
            DECIMALSEPARATOR = 0x400000b3,
            CURRENCYUNIT = 0x400000b4,
            CURRENCYSUBUNIT = 0x400000b5,
            KP_LEFTPAREN = 0x400000b6,
            KP_RIGHTPAREN = 0x400000b7,
            KP_LEFTBRACE = 0x400000b8,
            KP_RIGHTBRACE = 0x400000b9,
            KP_TAB = 0x400000ba,
            KP_BACKSPACE = 0x400000bb,
            KP_A = 0x400000bc,
            KP_B = 0x400000bd,
            KP_C = 0x400000be,
            KP_D = 0x400000bf,
            KP_E = 0x400000c0,
            KP_F = 0x400000c1,
            KP_XOR = 0x400000c2,
            KP_POWER = 0x400000c3,
            KP_PERCENT = 0x400000c4,
            KP_LESS = 0x400000c5,
            KP_GREATER = 0x400000c6,
            KP_AMPERSAND = 0x400000c7,
            KP_DBLAMPERSAND = 0x400000c8,
            KP_VERTICALBAR = 0x400000c9,
            KP_DBLVERTICALBAR = 0x400000ca,
            KP_COLON = 0x400000cb,
            KP_HASH = 0x400000cc,
            KP_SPACE = 0x400000cd,
            KP_AT = 0x400000ce,
            KP_EXCLAM = 0x400000cf,
            KP_MEMSTORE = 0x400000d0,
            KP_MEMRECALL = 0x400000d1,
            KP_MEMCLEAR = 0x400000d2,
            KP_MEMADD = 0x400000d3,
            KP_MEMSUBTRACT = 0x400000d4,
            KP_MEMMULTIPLY = 0x400000d5,
            KP_MEMDIVIDE = 0x400000d6,
            KP_PLUSMINUS = 0x400000d7,
            KP_CLEAR = 0x400000d8,
            KP_CLEARENTRY = 0x400000d9,
            KP_BINARY = 0x400000da,
            KP_OCTAL = 0x400000db,
            KP_DECIMAL = 0x400000dc,
            KP_HEXADECIMAL = 0x400000dd,
            LCTRL = 0x400000e0,
            LSHIFT = 0x400000e1,
            LALT = 0x400000e2,
            LGUI = 0x400000e3,
            RCTRL = 0x400000e4,
            RSHIFT = 0x400000e5,
            RALT = 0x400000e6,
            RGUI = 0x400000e7,
            MODE = 0x40000101,
            AUDIONEXT = 0x40000102,
            AUDIOPREV = 0x40000103,
            AUDIOSTOP = 0x40000104,
            AUDIOPLAY = 0x40000105,
            AUDIOMUTE = 0x40000106,
            MEDIASELECT = 0x40000107,
            WWW = 0x40000108,
            MAIL = 0x40000109,
            CALCULATOR = 0x4000010a,
            COMPUTER = 0x4000010b,
            AC_SEARCH = 0x4000010c,
            AC_HOME = 0x4000010d,
            AC_BACK = 0x4000010e,
            AC_FORWARD = 0x4000010f,
            AC_STOP = 0x40000110,
            AC_REFRESH = 0x40000111,
            AC_BOOKMARKS = 0x40000112,
            BRIGHTNESSDOWN = 0x40000113,
            BRIGHTNESSUP = 0x40000114,
            DISPLAYSWITCH = 0x40000115,
            KBDILLUMTOGGLE = 0x40000116,
            KBDILLUMDOWN = 0x40000117,
            KBDILLUMUP = 0x40000118,
            EJECT = 0x40000119,
            SLEEP = 0x4000011a,
            APP1 = 0x4000011b,
            APP2 = 0x4000011c,
            AUDIOREWIND = 0x4000011d,
            AUDIOFASTFORWARD = 0x4000011e
        };

        virtual Device getDevice() const;
        int getKey() const override;
        Keycode getButton() const;

        KeyboardInput(Keycode keyCode) : keyCode(keyCode) { }
    private:
        const Keycode keyCode;
};

class MouseInput : public Input {
    public:
        enum class Button {
            LEFT,
            RIGHT,
            MIDDLE,
            SIDE1,
            SIDE2,
        };

        virtual Device getDevice() const;
        int getKey() const override;
        Button getButton() const;
    
        void setClickCount(int count);
        int getClickCount() const;

        MouseInput(Button mouseButton) : mouseButton(mouseButton) { }

    private:
        const Button mouseButton;
    
        // The amount of clicks done in rapid succession. 1 for single-clicks, 2 for double-clicks, etc.
        int clicks = 0;
};

class Controller {
    public:
        virtual ~Controller() = default;
        virtual const String& getName() const = 0;
        virtual void rumble(float lowFreqIntensity, float highFreqIntensity, int durationMs) = 0;
};

class ControllerInput : public Input {
    public:
        enum class Button {
            INVALID = -1,
            A,
            B,
            X,
            Y,
            BACK,
            GUIDE,
            START,
            LEFTSTICK,
            RIGHTSTICK,
            LEFTBUMPER,
            RIGHTBUMPER,
            LEFTTRIGGER,
            RIGHTTRIGGER,
            DPAD_UP,
            DPAD_DOWN,
            DPAD_LEFT,
            DPAD_RIGHT,
            MAX
        };

        virtual Device getDevice() const;
        int getKey() const override;
        Button getButton() const;
        const Vector2f& getStickPosition() const;
        float getPressDepth() const;
        float getDownThreshold() const;
        const Controller* getController() const;
        void removeController();

        void setStickPosition(const Vector2f& pos);
        void setPressDepth(float depth);
        void setDownThreshold(float threshold);

        ControllerInput(Controller* ctrlr, Button inControllerButton, float threshold = 0.5f);

    private:
        const Controller* controller;
        const Button controllerButton;
        Vector2f stickPosition;
        float pressDepth;
        float downThreshold;
};

}

#endif // PGE_INPUT_H_INCLUDED
