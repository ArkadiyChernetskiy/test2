//
// Created by Olga Akimova on 02.11.2021.
//


//123
#ifndef INC_1_1_STRING_STRING_H
#define INC_1_1_STRING_STRING_H
#include <iostream>
#include <cstring>

class String{
private:
    size_t size, capacity;
    char* data;
    void changeCapacity(size_t newCapacity) {
        char* newData = new char[newCapacity];
        memcpy(newData, data, size);
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }
    size_t findInSuffix(const String& substr, size_t left_border) const {
        for (size_t left = left_border; left <= size - substr.size; left++) {
            for (size_t right = left; right < size; right++) {
                if (data[right] != substr.data[right - left])
                    break;
                if (right - left + 1 == substr.size)
                    return left;
            }
        }
        return size;
    }
public:
    String(const char* str): size(strlen(str)), capacity(size), data(new char[capacity]) {
        memcpy(data, str, size);
    }
    String(size_t len, char symbol): size(len), capacity(len), data(new char[capacity]) {
        memset(data, symbol, size);
    }
    String(): size(0), capacity(8), data(new char[capacity]) {}
    String(const String& str): size(str.size), capacity(str.size), data(new char[size]) {
        memcpy(data, str.data, size);
    }
    ~String() {
        delete[] data;
    }
    void swap(String& str) {
        std::swap(size, str.size);
        std::swap(capacity, str.capacity);
        std::swap(data, str.data);
    }
    String& operator=(const String& str) {
        String str_copy = str;
        swap(str_copy);
        return *this;
    }
    char& operator[](size_t index) {
        return data[index];
    }
    char operator[](size_t index) const {
        return data[index];
    }
    size_t length() const {
        return size;
    }
    void push_back(char symbol) {
        if (size == capacity)
            changeCapacity(capacity * 2);
        data[size++] = symbol;
    }
    char pop_back() {
        char last_element = data[--size];
        if (size * 4 <= capacity && capacity >= 16)
            changeCapacity(capacity / 2);
        return last_element;
    }
    char& front() {
        return data[0];
    }
    char front() const {
        return data[0];
    }
    char& back() {
        return data[size - 1];
    }
    char back() const {
        return data[size - 1];
    }
    String& operator+=(char symbol) {
        push_back(symbol);
        return *this;
    }
    String& operator+=(const String& str) {
        if (capacity < size + str.size)
            changeCapacity(size + str.size);
        memcpy(data + size, str.data, str.size);
        size += str.size;
        return *this;
    }
    size_t find(const String& substr) const {
        return findInSuffix(substr, 0);
    }
    size_t rfind(const String& substr) const {
        size_t next = findInSuffix(substr, 0), last = next;
        while (next != size) {
            last = next;
            next = findInSuffix(substr, last + 1);
        }
        return last;
    }
    String substr(size_t left, size_t count) const {
        String sub_str;
        if (sub_str.capacity < count)
            sub_str.changeCapacity(count);
        memcpy(sub_str.data, data + left, count);
        sub_str.size = count;
        return sub_str;
    }
    bool empty() const {
        return size == 0;
    }
    void clear() {
        delete[] data;
        size = 0;
        capacity = 8;
        data = new char[capacity];
    }
    bool operator==(const String& str) const {
        if (size != str.size)
            return false;
        return (memcmp(data, str.data, size) == 0);
    }
};

String operator+(const String& first, const String& second) {
    String sum(first);
    sum += second;
    return sum;
}
String operator+(const String& first, char second) {
    String sum(first);
    sum += second;
    return sum;
}
String operator+(char first, const String& second) {
    String sum(1, first);
    sum += second;
    return sum;
}

std::istream& operator>>(std::istream &in, String& str) {
    str.clear();
    char c;
    do {
        in.get(c);
    } while(isspace(c));
    do {
        str.push_back(c);
    } while(in.get(c) && !isspace(c));
    return in;
}

std::ostream& operator<<(std::ostream &out, const String& str) {
    for (size_t i = 0; i < str.length(); i++)
        out << str[i];
    return out;
}
#endif //INC_1_1_STRING_STRING_H
