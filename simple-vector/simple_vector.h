#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include "array_ptr.h"

struct ReserveProxyObj {
    ReserveProxyObj() = default;
    ReserveProxyObj(size_t capacity_to_reserve)
        : capacity(capacity_to_reserve) {

    }
    size_t capacity = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
        : SimpleVector(size, Type{}) 
    {
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
        : arr_(size), size_(size), capacity_(size)
    {
        std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : arr_(init.size()), size_(init.size()), capacity_(init.size())
    {
        std::copy(init.begin(), init.end(), begin());
    }

    // Создаёт вектор из other вектора
    SimpleVector(const SimpleVector& other)
        : arr_(other.size()), size_(other.size()), capacity_(other.size())
    {
        std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector(ReserveProxyObj ProxyObj)
        : arr_(ProxyObj.capacity), size_(0), capacity_(ProxyObj.capacity)
    {
    }

    SimpleVector(SimpleVector&& other) {
        arr_.swap(other.arr_);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    SimpleVector& operator=(SimpleVector&& other) {
        if (this != &other) {
            arr_.swap(other.arr_);
            size_ = std::exchange(other.size_, 0);
            capacity_ = std::exchange(other.capacity_, 0);
        }
        return *this;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            if (rhs.IsEmpty()) {
                Clear();
                return *this;
            }
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> tmp(new_capacity);
            std::copy(begin(), end(), &tmp[0]);
            arr_.swap(tmp);
            capacity_ = new_capacity;
        }
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (size_ == capacity_) {
            Resize(size_ + 1);
            arr_[size_ - 1] = item;
        }
        else {
            arr_[size_] = item;
            ++size_;
        }
    }

    void PushBack(Type&& item) {
        if (size_ == capacity_) {
            Resize(size_ + 1);
            arr_[size_ - 1] = std::move(item);
        }
        else {
            arr_[size_] = std::move(item);
            ++size_;
        }
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= cbegin() && pos <= cend());
        if (size_ == 0 && capacity_ == 0) {
            ArrayPtr<Type> tmp(1);
            tmp[0] = value;
            arr_.swap(tmp);
            ++capacity_;
            ++size_;
            return Iterator{ &arr_[0] };
        }
        auto index = std::distance(cbegin(), pos);
        if (size_ == capacity_) {
            Resize(size_ + 1);
            std::copy(&arr_[index], end(), &arr_[index + 1]);
            arr_[index] = value;
        }
        else {
            std::copy(&arr_[index], end(), &arr_[index + 1]);
            arr_[index] = value;
            ++size_;
        }
        return Iterator{ &arr_[index] };
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= cbegin() && pos <= cend());
        if (size_ == 0 && capacity_ == 0) {
            ArrayPtr<Type> tmp(1);
            tmp[0] = std::move(value);
            arr_.swap(tmp);
            ++capacity_;
            ++size_;
            return Iterator{ &arr_[0] };
        }
        auto index = std::distance(cbegin(), pos);
        if (size_ == capacity_) {
            Resize(size_ + 1);
            std::move(&arr_[index], end(), &arr_[index + 1]);
            arr_[index] = std::move(value);
        }
        else {
            std::move(&arr_[index], end(), &arr_[index + 1]);
            arr_[index] = std::move(value);
            ++size_;
        }
        return Iterator{ &arr_[index] };
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (size_) {
            --size_;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(pos >= cbegin() && pos < cend());
        auto index = std::distance(cbegin(), pos);
        std::move(&arr_[index + 1], end(), &arr_[index]);
        --size_;
        return Iterator(&arr_[index]);
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        arr_.swap(other.arr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index >= 0 && index < size_);
        return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index >= 0 && index < size_);
        return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("index >= size");
        }
        return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("index >= size");
        }
        return arr_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size < size_) {
            size_ = new_size;
        }
        else if (new_size <= capacity_) {
            std::generate(end(), end() + new_size, []() { return Type{}; });
            size_ = new_size;
        }
        else {
            ArrayPtr<Type> tmp(2 * new_size);
            std::move(begin(), end(), &tmp[0]);
            arr_.swap(tmp);
            capacity_ = 2 * new_size;
            size_ = new_size;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return Iterator{ &arr_[0] };
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return Iterator{ &arr_[size_] };
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return ConstIterator{ &arr_[0] };
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return ConstIterator{ &arr_[size_] };
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return end();
    }

private:

    ArrayPtr<Type> arr_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
