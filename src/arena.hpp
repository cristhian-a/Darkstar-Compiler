#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <type_traits>
#include <utility>
#include <stdexcept>

class ArenaAllocator {
public:
    explicit ArenaAllocator(size_t bytes)
      : m_size(bytes)
    {
        m_buffer = static_cast<std::byte*>(std::malloc(m_size));
        if (!m_buffer) throw std::bad_alloc();
        m_offset = 0;
    }

    ArenaAllocator(const ArenaAllocator&) = delete;
    ArenaAllocator& operator=(const ArenaAllocator&) = delete;

    ~ArenaAllocator() { std::free(m_buffer); }

    void* alloc_raw(size_t size, size_t align = alignof(std::max_align_t)) {
        uintptr_t base = reinterpret_cast<uintptr_t>(m_buffer);
        uintptr_t cur = base + m_offset;
        uintptr_t aligned = (cur + (align - 1)) & ~(align - 1);
        size_t new_offset = static_cast<size_t>((aligned - base) + size);
        if (new_offset > m_size) {
            throw std::bad_alloc();
        }
        m_offset = new_offset;
        return reinterpret_cast<void*>(aligned);
    }

    template<typename T, typename... Args>
    T* alloc(Args&&... args) {
        static_assert(std::is_constructible_v<T, Args...>,
                      "Type must be constructible");
        void* mem = alloc_raw(sizeof(T), alignof(T));
        return new(mem) T(std::forward<Args>(args)...);
    }

    template<typename T>
    T* alloc_trivial() {
        static_assert(std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>,
                      "alloc_trivial only for trivially copyable types");
        return reinterpret_cast<T*>(alloc_raw(sizeof(T), alignof(T)));
    }

    void reset() { m_offset = 0; } // DOES NOT call destructors

    size_t used() const { return m_offset; }
    size_t capacity() const { return m_size; }

private:
    size_t m_size;
    std::byte* m_buffer = nullptr;
    size_t m_offset = 0;
};
