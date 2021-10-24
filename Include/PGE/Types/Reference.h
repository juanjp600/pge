#ifndef PGE_REFERENCE_H_INCLUDED
#define PGE_REFERENCE_H_INCLUDED

namespace PGE {

template <typename T>
class Reference {
	public:
		Reference() = delete;
		Reference(T& val) : value(&val) { }

		const T& get() const { return *value; }
		T& get() { return *value; }

		operator const T&() const { return *value; }
		operator T&() { return *value; }

		const T* operator&() const { return value; }
		T* operator&() { return value; }

		const T* operator->() const { return value; }
		T* operator->() { return value; }

	private:
		T* value = nullptr;
};

}

#endif // PGE_REFERENCE_H_INCLUDED
