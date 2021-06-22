#ifndef PGE_SYSEVENTS_H_INCLUDED
#define PGE_SYSEVENTS_H_INCLUDED

namespace PGE {

class SysEvents {
    public:
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
