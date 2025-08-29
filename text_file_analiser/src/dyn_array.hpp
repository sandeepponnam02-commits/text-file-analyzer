// src/dyn_array.hpp
#pragma once
#include <cstdlib>
#include <stdexcept>
#include <utility>
#include <initializer_list>

template <typename T>
class DynamicArray {
    T* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t cap_ = 0;

    void reallocate(std::size_t new_cap) {
        T* nd = static_cast<T*>(::operator new(sizeof(T) * new_cap));
        for (std::size_t i = 0; i < size_; ++i)
            new (nd + i) T(std::move(data_[i]));
        for (std::size_t i = 0; i < size_; ++i)
            data_[i].~T();
        ::operator delete(data_);
        data_ = nd;
        cap_ = new_cap;
    }

public:
    DynamicArray() = default;
    explicit DynamicArray(std::size_t n, const T& value = T()) : size_(n), cap_(n) {
        data_ = static_cast<T*>(::operator new(sizeof(T) * cap_));
        for (std::size_t i = 0; i < size_; ++i) new (data_ + i) T(value);
    }
    DynamicArray(std::initializer_list<T> il) : DynamicArray(il.size()) {
        std::size_t i = 0;
        for (auto& v : il) data_[i++] = v;
    }
    DynamicArray(const DynamicArray& o) : size_(o.size_), cap_(o.size_) {
        data_ = static_cast<T*>(::operator new(sizeof(T) * cap_));
        for (std::size_t i = 0; i < size_; ++i) new (data_ + i) T(o.data_[i]);
    }
    DynamicArray& operator=(DynamicArray o) { swap(o); return *this; }
    DynamicArray(DynamicArray&& o) noexcept : data_(o.data_), size_(o.size_), cap_(o.cap_) {
        o.data_ = nullptr; o.size_ = o.cap_ = 0;
    }
    ~DynamicArray() {
        for (std::size_t i = 0; i < size_; ++i) data_[i].~T();
        ::operator delete(data_);
    }

    void push_back(const T& v) {
        if (size_ == cap_) reallocate(cap_ ? cap_ * 2 : 4);
        new (data_ + size_) T(v);
        ++size_;
    }
    void push_back(T&& v) {
        if (size_ == cap_) reallocate(cap_ ? cap_ * 2 : 4);
        new (data_ + size_) T(std::move(v));
        ++size_;
    }

    template <class... Args>
    T& emplace_back(Args&&... args) {
        if (size_ == cap_) reallocate(cap_ ? cap_ * 2 : 4);
        new (data_ + size_) T(std::forward<Args>(args)...);
        return data_[size_++];
    }

    T& operator[](std::size_t i) { return data_[i]; }
    const T& operator[](std::size_t i) const { return data_[i]; }

    T& at(std::size_t i) {
        if (i >= size_) throw std::out_of_range("DynamicArray::at");
        return data_[i];
    }
    const T& at(std::size_t i) const {
        if (i >= size_) throw std::out_of_range("DynamicArray::at");
        return data_[i];
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        for (std::size_t i = 0; i < size_; ++i) data_[i].~T();
        size_ = 0;
    }

    void reserve(std::size_t n) {
        if (n > cap_) reallocate(n);
    }

    T* begin() { return data_; }
    T* end() { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }

    void swap(DynamicArray& o) noexcept {
        std::swap(data_, o.data_);
        std::swap(size_, o.size_);
        std::swap(cap_, o.cap_);
    }
};