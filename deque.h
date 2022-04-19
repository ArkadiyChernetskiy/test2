#ifndef INC_1_5_DEQUE_DEQUE_H
#define INC_1_5_DEQUE_DEQUE_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <type_traits>

template <typename T>
class Deque {
private:
    static const size_t internal_size = 5;
    std::vector<T*> external;
    size_t begin_;
    size_t end_;

    void expand() {
        if (begin_ != 0 && end_ != external.size() * internal_size)
            return;
        size_t n = external.size();
        external.resize(3 * n);
        std::copy(external.begin(), external.begin() + n, external.begin() + n);
        begin_ += n * internal_size;
        end_ += n * internal_size;
    }

public:
    Deque(): external(1), begin_(internal_size / 2), end_(internal_size / 2) {
        external[0] = reinterpret_cast<T*>(new int8_t[internal_size * sizeof(T)]);
    }
    Deque(const Deque& other): begin_(other.begin_), end_(other.end_) {
        size_t n = other.external.size();
        external.resize(n);
        for (size_t i = other.begin_ / internal_size; i < (other.end_ + internal_size - 1) / internal_size; i++) {
            try {
                external[i] = reinterpret_cast<T*>(new int8_t[internal_size * sizeof(T)]);
            } catch (...) {
                for (size_t i2 = other.begin_ / internal_size; i2 < i; i2++)
                    delete[] reinterpret_cast<uint8_t*>(external[i]);
                throw;
            }
            for (size_t j = 0; j < internal_size && i * internal_size + j < other.end_; j++)
                try {
                    new(external[i] + j) T(other.external[i][j]);
                } catch (...) {
                    for (size_t j2 = 0; j2 < j; j2++)
                        (external[i] + j)->~T();
                    throw;
                }
        }
    }
    explicit Deque(size_t count) {
        size_t remainder = count % internal_size;
        size_t used = (count + internal_size - 1) / internal_size; // rounded up
        external.resize(used * 3); // can throw exception, but strong-guarantee
        begin_ = used * internal_size;
        end_ = 2 * used * internal_size; // remainder = 0
        if (remainder > 0)
            end_ -= internal_size - remainder;

        for (size_t i = used; i < 2 * used; i++)
            try {
                external[i] = new T[internal_size]; // actually more default Ts are constructed than required
            } catch (...) {
                for (size_t i2 = used; i2 < i; i2++) {
                    for (size_t j = 0; j < internal_size; j++)
                        (external[i2] + j)->~T();
                    delete[] reinterpret_cast<uint8_t*>(external[i2]);
                }
                throw;
            }
    }
    Deque(size_t count, const T& value) {
        size_t remainder = count % internal_size;
        size_t used = (count + internal_size - 1) / internal_size; // rounded up
        external.resize(used * 3);
        begin_ = used * internal_size;
        end_ = 2 * used * internal_size; // remainder = 0
        if (remainder > 0)
            end_ -= internal_size - remainder;

        size_t partial = 2 * used - (remainder > 0);
        for (size_t i = used; i < 2 * used; i++) {
            try {
                external[i] = reinterpret_cast<T*>(new int8_t[internal_size * sizeof(T)]);
            } catch (...) {
                for (size_t i2 = used; i2 < i; i2++)
                    delete[] reinterpret_cast<uint8_t*>(external[i2]);
                throw;
            }
            for (size_t j = 0; j < (i == partial ? remainder : internal_size); j++)
                try {
                    new(external[i] + j) T(value);
                } catch (...) {
                    for (size_t i2 = used; i2 <= i; i2++) {
                        for (size_t j2 = 0; j2 < (i2 == i ? j : internal_size); j2++)
                            (external[i] + j2)->~T();
                        delete[] reinterpret_cast<uint8_t *>(external[i2]);
                    }
                    throw;
                }
        }
    }

    void swap(Deque& other) {
        external.swap(other.external);
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
    }
    Deque& operator==(const Deque& other) const {
        Deque& copy(other);
        swap(other);
    }

    size_t size() const {
        return end_ - begin_;
    }

    T& operator[](size_t pos) {
        pos += begin_;
        return *(external[pos / internal_size] + pos % internal_size);
    }
    const T& operator[](size_t pos) const {
        pos += begin_;
        return *(external[pos / internal_size] + pos % internal_size);
    }

    T& at(size_t pos) {
        if (pos >= size())
            throw std::out_of_range("Index out of range");
        return (*this)[pos];
    }
    const T& at(size_t pos) const {
        if (pos >= size())
            throw std::out_of_range("Index out of range");
        return (*this)[pos];
    }

    void push_back(const T& value) {
        std::vector<T*> copy;
        bool expand_successful = false, expand_done = false;
        bool mini_array_successful = false, assigning_successful = false;
        size_t i, j;
        try {
            if (begin_ == 0 || end_ == external.size() * internal_size) {
                expand_done = true;
                copy = external;
            }
            expand();
            expand_successful = true;
            i = end_ / internal_size, j = end_ % internal_size;
            if (j == 0)
                external[i] = reinterpret_cast<T*>(new int8_t[internal_size * sizeof(T)]);
            mini_array_successful = true;
            new(external[i] + j) T(value);
            assigning_successful = true;
        } catch (...) {
            if (expand_successful && mini_array_successful && !assigning_successful) {
                if (j == 0)
                    delete[] reinterpret_cast<uint8_t*>(external[i]);
            }
            if (expand_successful && expand_done)
                external = copy;
            throw;
        }
        end_++;
    }
    void pop_back() {
        end_--;
        (external[end_ / internal_size] + end_ % internal_size)->~T();
    }
    void push_front(const T& value) {
        std::vector<T*> copy;
        begin_--;
        size_t i = begin_ / internal_size, j = begin_ % internal_size;
        bool expand_successful = false, expand_done = false;
        bool mini_array_successful = false, assigning_successful = false;
        try {
            if (begin_ == 0 || end_ == external.size() * internal_size) {
                expand_done = true;
                copy = external;
            }
            expand();
            expand_successful = true;
            if (j == internal_size - 1)
                external[i] = reinterpret_cast<T*>(new int8_t[internal_size * sizeof(T)]);
            mini_array_successful = true;
            new(external[i] + j) T(value);
            assigning_successful = true;
        } catch (...) {
            if (expand_successful && mini_array_successful && !assigning_successful) {
                if (j == internal_size - 1)
                    delete[] reinterpret_cast<uint8_t*>(external[i]);
            }
            if (expand_successful && expand_done)
                external = copy;
            begin_++;
            throw;
        }
    }
    void pop_front() {
        (external[end_ / internal_size] + end_ % internal_size)->~T();
        begin_++;
    }


    template <bool is_const>
    class Iter {
    private:
    size_t place;
    const std::vector<T*>* deque;
    Iter(size_t place, const std::vector<T*>* deque): place(place), deque(deque) {};
    friend Deque;
    public:
        Iter(const Iter& other) = default;
        using difference_type = long long; // TODO
        using value_type = typename std::conditional<is_const, const T, T>::type;;
        using pointer = typename std::conditional<is_const, const T*, T*>::type;
        using reference = typename std::conditional<is_const, const T&, T&>::type;
        using iterator_category = std::random_access_iterator_tag;

        operator Iter<true>() {
            return Iter<true>(place, deque);
        }

        Iter& operator=(const Iter& other) = default;
        reference operator*() const {
            return *((*deque)[place / internal_size] + place % internal_size);
        }
        pointer operator->() const {
            return (*deque)[place / internal_size] + place % internal_size;
        }
        Iter& operator++() { // ++it
            place++;
            return *this;
        }
        Iter operator++(int) { // it++
            Iter copy(*this);
            place++;
            return copy;
        }
        Iter& operator--() { // --it
            place--;
            return *this;
        }
        Iter operator--(int) { // it--
            Iter copy(*this);
            place--;
            return copy;
        }
        Iter& operator+=(difference_type step) {
            place += step;
            return *this;
        }
        Iter& operator-=(difference_type step) {
            place -= step;
            return *this;
        }
        reference operator[](difference_type n) const {
            size_t new_place = place + n;
            return *((*deque)[new_place / internal_size] + new_place % internal_size);
        }
        Iter operator+(difference_type step) const {
            Iter copy(*this);
            return copy += step;
        }
        Iter operator-(difference_type step) const {
            Iter copy(*this);
            return copy -= step;
        }
        friend Iter operator+(difference_type step, const Iter& it) {
            Iter copy(*it);
            return copy += step;
        }
        friend bool operator==(const Iter& lhs, const Iter& rhs) {
            return lhs.deque == rhs.deque && lhs.place == rhs.place;
        }
        friend bool operator!=(const Iter& lhs, const Iter& rhs) {
            return lhs.deque != rhs.deque || lhs.place != rhs.place;
        }
        friend difference_type operator-(const Iter& lhs, const Iter& rhs) {
            return lhs.place - rhs.place;
        }
        friend bool operator<(const Iter& lhs, const Iter& rhs) {
            return lhs.place < rhs.place;
        }
        friend bool operator>(const Iter& lhs, const Iter& rhs) {
            return lhs.place > rhs.place;
        }
        friend bool operator<=(const Iter& lhs, const Iter& rhs) {
            return lhs.place <= rhs.place;
        }
        friend bool operator>=(const Iter& lhs, const Iter& rhs) {
            return lhs.place >= rhs.place;
        }
    };


    using iterator = Iter<false>;
    using const_iterator = Iter<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin()  {
        return iterator(begin_, &this->external);
    }
    iterator end() {
        return iterator(end_, &this->external);
    }
    const_iterator begin() const {
        return const_iterator(begin_, &this->external);
    }
    const_iterator end() const {
        return const_iterator(end_, &this->external);
    }
    const_iterator cbegin() const {
        return const_iterator(begin_, &this->external);
    }
    const_iterator cend() const {
        return const_iterator(end_, &this->external);
    }
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }
    const_reverse_iterator rcbegin() const {
        return const_reverse_iterator(cend());
    }
    const_reverse_iterator rcend() const {
        return const_reverse_iterator(cbegin());
    }


    void insert(const iterator& place, const T& value) {
        Deque copy(*this);
        try {
            T curr = value;
            for (iterator it = place; it < end(); it++)
                std::swap(*it, curr);
            push_back(curr);
        } catch (...) {
            *this = copy;
            throw;
        }
    }
    void erase(const iterator& place) {
        Deque copy(*this);
        try {
            for (iterator it = place; it < end() - 1; it++)
                *it = *(it + 1);
            pop_back();
        } catch (...) {
            *this = copy;
            throw;
        }
    }
};

#endif //INC_1_5_DEQUE_DEQUE_H