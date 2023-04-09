#ifndef X17_UNIQUE_POINTER_STABLE
#define X17_UNIQUE_POINTER_STABLE

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include <memory>
#include <exception>
#include <iostream>
#include <cstdio>

namespace X17 {

template <typename T>
class unique_ptr {
   public:
    using pointer = T*;
    using reference = T&;

    /* DELETED */
    unique_ptr(const unique_ptr& other) = delete;
    unique_ptr& operator=(const unique_ptr& other) = delete;
    /* END DELETED */

    constexpr unique_ptr() noexcept : m_object(nullptr) {}

    unique_ptr(pointer pointer_to_object) noexcept
        : m_object(pointer_to_object) {}

    unique_ptr(unique_ptr&& other) {
        m_object = other.m_object;
        other.m_object = nullptr;
    }

    ~unique_ptr() { clean(); }

    unique_ptr& operator=(unique_ptr&& other) {
        clean();
        m_object = other.m_object;
        other.m_object = nullptr;
    }

    pointer get() { return m_object; }

    void reset(pointer new_ptr) {
        clean();
        m_object = new_ptr;
    }

    pointer release() {
        pointer ret_ptr = m_object;
        clean();

        return ret_ptr;
    }

    pointer operator->() { return m_object; }

    reference operator*() { return *m_object; }

   private:
    pointer m_object;

    void clean() {
        if (m_object != nullptr) {
            delete m_object;
        }
    }
};

template <typename T, typename... Args>
constexpr inline unique_ptr<T> make_unique(Args&&... arguments) {
    return unique_ptr<T>(new T(std::forward<Args>(arguments)...));
}

}  // namespace X17

#endif  // !X17_UNIQUE_POINTER_STABLE