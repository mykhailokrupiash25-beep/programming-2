#pragma once

#include <stdexcept>
#include <array>

template <typename T, size_t N>
class StackVector {
public:
    explicit StackVector(size_t a_size = 0) {
        if (a_size > N) {
            throw std::invalid_argument("Size exceeds capacity");
        }
        size_ = a_size;
    }

    using Iterator = T*;
    using ConstIterator = const T*;

    T& operator[](size_t index) {
        return data_[index];
    }

    const T& operator[](size_t index) const {
        return data_[index];
    }

    Iterator begin() {
        return data_.begin();
    }

    Iterator end() {
        return data_.begin() + size_;
    }

    ConstIterator begin() const {
        return data_.begin();
    }

    ConstIterator end() const {
        return data_.begin() + size_;
    }

    size_t Size() const {
        return size_;
    }

    size_t Capacity() const {
        return N;
    }

    void PushBack(const T& value) {
        if (size_ >= N) {
            throw std::overflow_error("Vector is full");
        }
        data_[size_++] = value;
    }

    T PopBack() {
        if (size_ == 0) {
            throw std::underflow_error("Vector is empty");
        }
        return data_[--size_];
    }

private:
    std::array<T, N> data_;
    size_t size_ = 0;
};