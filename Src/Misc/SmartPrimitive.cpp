
/*template<typename T, typename S>
SmartPrimitive<T,S>::operator T() const {
    return value;
}

template <typename T, typename S>
T SmartPrimitive<T,S>::operator()() const {
    return value;
}

template <typename T, typename S>
void SmartPrimitive<T,S>::operator=(T t) {
    value = t;
}

template <typename T, typename S>
T* SmartPrimitive<T,S>::operator&() {
    return &value;
}

template <typename T, typename S>
SmartPrimitive<T, S>::SmartPrimitive() {
    destructor = [](){};
}

template <typename T, typename S>
SmartPrimitive<T,S>::SmartPrimitive(T val) {
    value = val;
    destructor = [](){};
}

template <typename T, typename S>
SmartPrimitive<T,S>::SmartPrimitive(T val, void(*dtor)(S)) {
    value = val;
    destructor = dtor;
}

template <typename T, typename S>
SmartPrimitive<T,S>::~SmartPrimitive() {
    destructor(value);
}


template <typename T>
SmartPrimitiveMinimalCopying<T>::operator T&() const {
    return value;
}

template <typename T>
T& SmartPrimitiveMinimalCopying<T>::operator()() const {
    return value;
}

template <typename T>
SmartPrimitiveMinimalCopying<T>::SmartPrimitiveMinimalCopying(T val, void(*dtor)(const T&)) {
    value = val;
    destructor = dtor;
}

template <typename T>
SmartPrimitiveMinimalCopying<T>::~SmartPrimitiveMinimalCopying() {
    destructor(value);
}


template <typename T>
SmartPrimitiveArray<T>::operator std::vector<T>&() const {
    return values;
}

template <typename T>
std::vector<T>& SmartPrimitiveArray<T>::operator()() const {
    return values;
}

template <typename T>
T& SmartPrimitiveArray<T>::operator[](int i) const{
    return values[i];
}

template <typename T>
SmartPrimitiveArray<T>::SmartPrimitiveArray(void(*dtor)(T&)) {
    destructor = dtor;
}

template <typename T>
SmartPrimitiveArray<T>::SmartPrimitiveArray(int size, void(*dtor)(T&)) : SmartPrimitiveArray(dtor) {
    values = std::vector<T>(size);
}

template <typename T>
SmartPrimitiveArray<T>::~SmartPrimitiveArray() {
    for (T v : values) {
        destructor(v);
    }
}*/
