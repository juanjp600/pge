#ifndef PGE_SYSEVENTS_H_INCLUDED
#define PGE_SYSEVENTS_H_INCLUDED

namespace PGE {

// TODO: This can probably be made cleaner assuming a singular Graphics instance to be the standard.
/// Class that is required for dealing with system events.
class SysEvents {
    public:
        /// This must be called in order for system events to be handled.
        /// System events include window closing, keyboard input, etc.
        static void update();
        class Subscriber {
            protected:
                Subscriber() = default;
        };

    protected:
        SysEvents() = default;
};

}

#endif // PGE_SYSEVENTS_H_INCLUDED
