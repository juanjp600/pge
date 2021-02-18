#ifndef PGEINTERNAL_SMARTPRIMITIVE_INCLUDED
#define PGEINTERNAL_SMARTPRIMITIVE_INCLUDED

#include <vector>

// TODO: Find a way to get this to work.
//template <typename T, typename S=T, typename = typename std::enable_if<std::is_base_of<T, S>::value || std::is_same<T,S>::value || true>::type >
//class SmartPrimitive;

template <typename T, typename S=T>
class SmartPrimitive {
    public:
        // Cast.
        operator T() const {
            return value;
        }
        // Get.
        T operator()() const {
            return value;
        }
        // Set.
        void operator=(T t) {
            value = t;
        }
        // Get as pointer.
        T* operator&() {
            return &value;
        }
        void operator=(const SmartPrimitive& sp) {
            value = sp.value;
            destructor = sp.destructor;
            ((SmartPrimitive&)sp).destructor = noop;
        }

        SmartPrimitive() {
            destructor = noop;
        }
        SmartPrimitive(T val) : SmartPrimitive() {
            value = val;
        }
        SmartPrimitive(T val, void(*dtor)(S)) : SmartPrimitive(val) {
            destructor = dtor;
        }
        ~SmartPrimitive() {
            destructor(value);
        }

    private:
        static inline void(*noop)(S t) = [](S){};
        
        T value;
        void(*destructor)(S t);
};

template <typename T>
class SmartPrimitiveMinimalCopying {
    public:
        // Cast.
        operator T() const {
            return value;
        }
        // Get.
        T operator()() const {
            return value;
        }
        void operator=(const SmartPrimitiveMinimalCopying& sp) {
            value = sp.value;
            destructor = sp.destructor;
            ((SmartPrimitiveMinimalCopying&)sp).destructor = noop;
        }

        SmartPrimitiveMinimalCopying() {
            //destructor = [](T&){};
        }
        // TODO: The lambda breaks with a reference, why?
        SmartPrimitiveMinimalCopying(T val, void(*dtor)(const T&)) {
            value = val;
            destructor = dtor;
        }
        ~SmartPrimitiveMinimalCopying() {
            destructor(value);
        }

    private:
        static inline void(*noop)(const T& t) = [](const T&){};

        T value;
        void(*destructor)(const T& t);
};

template <typename T>
class SmartPrimitiveArray {
    public:
        // Cast.
        operator std::vector<T> () {
            return values;
        }
        // Get.
        std::vector<T> operator()() {
            return values;
        }
        // Index.
        /*T operator[](int i) const {
            return values[i];
        }*/
        void operator=(const SmartPrimitiveArray& sp) {
            values = sp.values;
            destructor = sp.destructor;
            ((SmartPrimitiveArray&)sp).destructor = noop;
        }

        SmartPrimitiveArray() {
            //destructor = [](T&){};
        }
        SmartPrimitiveArray(void(*dtor)(T&)) {
            destructor = dtor;
        }
        SmartPrimitiveArray(int size, void(*dtor)(T&)) : SmartPrimitiveArray(dtor) {
            values.resize(size);
        }
        ~SmartPrimitiveArray() {
            for (T val : values) {
                destructor(val);
            }
        }

    private:
        static inline void(*noop)(T& t) = [](T&){};

        std::vector<T> values;
        void(*destructor)(T& t);
};

#endif // PGEINTERNAL_SMARTPRIMITIVE_INCLUDED
