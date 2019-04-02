#ifndef PGE_SYSEVENTS_H_INCLUDED
#define PGE_SYSEVENTS_H_INCLUDED

class SysEvents {
    public:
        static void update();
        class Subscriber {
            protected:
                Subscriber(){}
        };
    protected:
        SysEvents(){}
};

#endif
