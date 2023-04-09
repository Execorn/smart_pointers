#ifndef X17_SHARED_POINTER_STABLE
#define X17_SHARED_POINTER_STABLE

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include <cassert>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>

namespace X17 {

template <typename T, typename... Args>
inline shared_ptr<T> make_shared(Args&&... arguments) {
    return shared_ptr<T>(std::forward<Args>(arguments)...);
}

struct shared_control_underflow : std::runtime_error {
   public:
    shared_control_underflow(const char* file, size_t line_number)
        : std::runtime_error(report_message) {
        // This check probably should be here, but my throw_bad_call macro
        // guarantees __FILE__ will be passed as the second argument

        assert(file != nullptr);
        // TODO: change to sprintf
        std::ostringstream exception_message_stream;
        exception_message_stream << file << ":" << line_number << ": "
                                 << report_message;

        m_message = exception_message_stream.str();
    }

    ~shared_control_underflow() throw() {}

    const char* what() const throw() { return m_message.c_str(); }

   private:
    const std::string report_message = "Bad shared_control quantity";
    std::string m_message;
};

#define throw_underflow() throw shared_control_underflow(__FILE__, __LINE__);

template <typename T>
class shared_control {
    using pointer = T*;

   public:
    explicit constexpr shared_control() noexcept : m_quantity(1) {}

    /* VIRTUAL FUNCTIONS */
    virtual ~shared_control();
    virtual void block() noexcept = 0;
    virtual pointer curPtr() const noexcept = 0;
    /* END OF VIRTUAL FUNCTIONS */

    void operator++(int) noexcept { ++m_quantity; }
    void operator++() noexcept { ++m_quantity; };

    void operator--(int) {
        if (m_quantity == 0) {
            throw_underflow();
        }
        --m_quantity;
    }

    void operator--() {
        if (m_quantity == 0) {
            throw_underflow();
        }
        --m_quantity;
    }

    size_t getQuantity() const noexcept { return m_quantity; }

   private:
    size_t m_quantity{1};
};

template <typename T, class Del>
class shared_deleter_control : public shared_control<T> {
    using pointer = T*;

   public:
    explicit constexpr shared_deleter_control() = delete;

    shared_deleter_control(pointer object_ptr, Del deleter_ptr)
        : m_object_ptr(object_ptr), m_deleter(deleter_ptr) {}

    pointer curPtr() const noexcept final override { return m_object_ptr; }

    void block() noexcept final override { m_deleter(m_object_ptr); }

   private:
    pointer m_object_ptr;
    Del m_deleter;
};

template <typename T>
class shared_default_delete {
    using pointer = T*;

   public:
    void operator()(pointer object_to_del) const { delete object_to_del; }

    void operator()(T array_to_del) const { delete[] array_to_del; }
};

template <typename T>
class shared_ptr : public shared_control<T> {
    using pointer = T*;

   public:
    explicit constexpr shared_ptr() : m_control(nullptr) {}

    constexpr shared_ptr(std::nullptr_t) {
        // call default (no-args) constructor
        shared_ptr();
    }

    explicit constexpr shared_ptr(pointer object_ptr)
        : m_control(new shared_deleter_control<T, shared_default_delete<T>>(
              object_ptr,
              shared_default_delete<T>())) {}

    template <class Del>
    explicit constexpr shared_ptr(pointer object_ptr, Del deleter_class)
        : m_control(
              new shared_deleter_control<T, Del>(object_ptr, deleter_class)) {}

    shared_ptr(const shared_ptr& other) {
        m_control = other.m_control;
        operator++();
    }

    shared_ptr(shared_ptr&& other) {
        m_control = other.m_control;
        other.m_control = nullptr;
    }

    ~shared_ptr() { deshare(); }

    /* INTERFACE FUNCTIONS */

    bool unique() const noexcept {
        if (!m_control)
            return false;

        return m_control->getQuantity() == 1;
    }

    size_t use_count() const noexcept {
        if (!m_control)
            return false;

        return m_control->getQuantity();
    }

    pointer get() const {
        if (!m_control)
            return false;

        return m_control->curPtr();
    }

    void swap(shared_ptr& other) noexcept {
        m_control<T>* temp = m_control;
        m_control = other.m_control;
        other.m_control = m_control;
    }

    shared_ptr& operator=(const shared_ptr& other) {
        if (this != &other) {
            deshare();
            m_control = other.m_control;
            add();
        }

        return *this;
    }

    bool owner_before(const shared_ptr& other) const noexcept {
        return m_control == other.m_control;
    }

    shared_ptr& operator=(shared_ptr&& other) {
        if (this != &other) {
            m_control = other.m_control;
            other.m_control = nullptr;
        }

        return *this;
    }

    T* operator->() const noexcept {
        return m_control == nullptr ? nullptr : m_control->curPtr();
    }

    T& operator*() const noexcept { return *(m_control->curPtr()); }

    explicit operator bool() const noexcept {
        if (m_control)
            return m_control->curPtr() == nullptr ? false : true;
        return false;
    }

   private:
    shared_control<T>* m_control;

    void operator++() noexcept {
        if (m_control != nullptr) {
            m_control->operator++();
        }
    }

    void operator--() noexcept {
        if (m_control != nullptr) {
            m_control->operator--();
        }
    }

    void deshare() {
        operator--();
        if (m_control->getQuantity() == 0) {
            m_control->block();

            delete m_control;
            m_control == nullptr;
        }
    }
};

}  // namespace X17

#endif  // !X17_SHARED_POINTER_STABLE