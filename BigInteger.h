//
// Created by Olga Akimova on 22.11.2021.
//

#ifndef INC_1_2_BIGINTEGER_BIGINTEGER_H
#define INC_1_2_BIGINTEGER_BIGINTEGER_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class BigInteger{
private:
    const static int base = 1e9;
    const static unsigned int TenPower;
    std::vector<int> data;
    bool is_negative;

    void shrink() {
        while (data.size() > 1 && data.back() == 0)
            data.pop_back();
    }

    void sumUpAbsVal(const BigInteger& another_num);
    void substrAbsVal(const BigInteger& minuend, const BigInteger& substr, BigInteger& dest);

    enum class CompareVal {less = -1, equal, bigger};
    CompareVal compareAbsVal(const BigInteger& another_num) const;

    static std::string digitToReversedStr(int digit, bool add_leading_zeros=false) {
        std::string ans;
        while (digit > 0) {
            ans += static_cast<char>(digit % 10 + '0');
            digit /= 10;
        }
        if (add_leading_zeros)
            while (ans.size() < TenPower)
                ans += '0';
        if (ans.empty())
            ans = "0";
        return ans;
    }

    bool is_zero() const {
        return (data.size() == 1 && data[0] == 0);
    }

    BigInteger& shift(size_t places) {
        if (places != 0 && !is_zero())
            data.insert(data.begin(), places, 0);
        return *this;
    }

    static unsigned int log10(int n) {
        unsigned ans = 0;
        while (n > 1) {
            ans++;
            n /= 10;
        }
        return ans;
    }

public:
    BigInteger(): is_negative(false){
        data.resize(1);
    }

    BigInteger(int number): is_negative(number < 0){
        if (number < 0)
            number *= -1;
        while (number > 0) {
            data.push_back(number % base);
            number /= base;
        }
        if (data.empty())
            data.resize(1);
    }

    BigInteger(const BigInteger& another_num) = default;

    void swap(BigInteger& another_num) {
        data.swap(another_num.data);
        std::swap(is_negative, another_num.is_negative);
    }

    BigInteger& operator=(const BigInteger& number) {
        BigInteger copy(number);
        swap(copy);
        return *this;
    }

    ~BigInteger()= default;

    BigInteger& operator+=(const BigInteger& another_num);

    BigInteger& operator-=(const BigInteger& another_num);

    BigInteger& operator*=(long long num);

    BigInteger& operator*=(const BigInteger& another_num);

    BigInteger& operator/=(const BigInteger& another_num);

    BigInteger& operator%=(const BigInteger& mod);

    BigInteger operator-() const {
        BigInteger ans(*this);
        ans.is_negative ^= true;
        return ans;
    }

    BigInteger& operator++() {       // ++x;
        *this += BigInteger(1);
        return *this;
    }

    BigInteger operator++(int) {     // x++;
        BigInteger ans(*this);
        *this += BigInteger(1);
        return ans;
    }

    BigInteger& operator--() {
        *this -= BigInteger(1);
        return *this;
    }

    BigInteger operator--(int) {
        BigInteger ans(*this);
        *this -= BigInteger(1);
        return ans;
    }

    std::string toString() const{
        std::string ans;
        for (size_t digit = 0; digit < data.size(); digit++)
            ans += digitToReversedStr(data[digit], (digit < data.size() - 1));
        if (is_negative && ans != "0")
            ans += '-';
        std::reverse(ans.begin(), ans.end());
        return ans;
    }

    explicit operator bool() const {
        return (!is_zero());
    }

    void clear() {
        is_negative = false;
        data.resize(1, 0);
    }

    friend std::istream& operator>>(std::istream& in, BigInteger& number);
    friend bool operator<(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator==(const BigInteger& lhs, const BigInteger& rhs);
};

const unsigned int BigInteger::TenPower = log10(base);

BigInteger abs(const BigInteger& number) {
    BigInteger ans(number);
    return (number < 0 ? -number : number);
}

void BigInteger::sumUpAbsVal(const BigInteger& another_num){
    size_t curr_size = data.size(), add_size = another_num.data.size();
    size_t bigger = std::max(curr_size, add_size);
    data.resize(bigger + 1);

    int carry = 0;
    for (size_t digit = 0; digit < add_size || carry > 0; digit++) {
        int curr = data[digit] + carry;
        if (digit < add_size)
            curr += another_num.data[digit];
        data[digit] = curr % base;
        carry = curr / base;
    }
    shrink();
}

void BigInteger::substrAbsVal(const BigInteger& bigger, const BigInteger& smaller, BigInteger& dest){
    dest.data.resize(bigger.data.size());
    int carry = 0;
    for (size_t digit = 0; digit < smaller.data.size() || carry > 0; digit++) {
        int curr_subtr = carry;
        if (digit < smaller.data.size())
            curr_subtr += smaller.data[digit];
        if (bigger.data[digit] < curr_subtr) {
            carry = 1;
            curr_subtr -= base;
        }
        else {
            carry = 0;
        }
        dest.data[digit] = bigger.data[digit] - curr_subtr;
    }
    dest.shrink();
}

BigInteger::CompareVal BigInteger::compareAbsVal(const BigInteger& another_num) const {
    size_t curr_size = data.size(), another_size = another_num.data.size();
    if (curr_size != another_size) {
        if (curr_size < another_size)
            return CompareVal::less;
        return CompareVal::bigger;
    }
    size_t digit = curr_size - 1;
    while (true){
        if (data[digit] != another_num.data[digit]) {
            if (data[digit] < another_num.data[digit])
                return CompareVal::less;
            return CompareVal::bigger;
        }
        if (digit == 0)
            break;
        digit--;
    }
    return CompareVal::equal;
}

BigInteger& BigInteger::operator+=(const BigInteger& another_num) {
    if (is_negative == another_num.is_negative) {
        sumUpAbsVal(another_num);
    } else if (compareAbsVal(another_num) == CompareVal::less) {
        substrAbsVal(another_num, *this, *this);
        is_negative ^= true;
    } else {
        substrAbsVal(*this, another_num, *this);
    }
    return *this;
}

BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger sum(lhs);
    sum += rhs;
    return sum;
}

BigInteger& BigInteger::operator-=(const BigInteger &another_num) { // a -= b  <=>  a = a - b  <=>  -a = -a + b
    is_negative ^= true;
    *this += another_num;
    is_negative ^= true;
    return *this;
}

BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger ans(lhs);
    ans -= rhs;
    return ans;
}

BigInteger& BigInteger::operator*=(long long num) {
    is_negative = (is_negative ^ (num < 0));
    if (num < 0)
        num *= -1;
    data.resize(data.size() + 2);

    int carry = 0;
    for (size_t digit = 0; digit < data.size(); digit++) {
        long long curr = carry;
        if (digit < data.size())
            curr += data[digit] * num;
        data[digit] = curr % base;
        carry = curr / base;
    }
    shrink();
    return *this;
}

BigInteger operator*(const BigInteger& long_num, int num) {
    BigInteger ans(long_num);
    ans *= num;
    return ans;
}

BigInteger& BigInteger::operator*=(const BigInteger &another_num) {
    is_negative = (is_negative ^ another_num.is_negative);
    BigInteger ans;
    for (size_t digit = 0; digit < another_num.data.size(); digit++)
        ans += (*this * another_num.data[digit]).shift(digit);
    *this = ans;
    return *this;
}

BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger ans(lhs);
    ans *= rhs;
    return ans;
}

BigInteger& BigInteger::operator/=(const BigInteger& another_num) {
    BigInteger ans;
    ans.data.resize(data.size());
    ans.is_negative = (is_negative ^ another_num.is_negative);
    BigInteger curr_dividend;
    size_t digit = data.size() - 1;
    while (true){
        curr_dividend.shift(1);
        curr_dividend += static_cast<BigInteger>(data[digit]);

        int left = 0, right = base, mid;
        while (right - left > 1) {
            mid = (left + right) / 2;
            if ((another_num * mid).compareAbsVal(curr_dividend) != CompareVal::bigger)
                left = mid;
            else
                right = mid;
        }
        ans.data[digit] = left;
        curr_dividend -= abs(another_num) * left;

        if (digit == 0)
            break;
        digit--;
    }
    ans.shrink();
    *this = ans;
    return *this;
}

BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger ans(lhs);
    ans /= rhs;
    return ans;
}

BigInteger& BigInteger::operator%=(const BigInteger& mod) {
    *this -= *this / mod * mod;
    return *this;
}

BigInteger operator%(const BigInteger& number, const BigInteger& mod) {
    BigInteger ans(number);
    ans %= mod;
    return ans;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& number) {
    out << number.toString();
    return out;
}

std::istream& operator>>(std::istream& in, BigInteger& number) {
    number.clear();
    std::string input;
    in >> input;
    size_t len = input.size();
    size_t block = BigInteger::TenPower, neg = 0;
    if (input[0] == '-'){
        number.is_negative = true;
        neg = 1;
    }
    number.data.resize((len - neg + block - 1) / block);
    for (size_t digit = 0; digit * block < len - neg; digit++) {
        if (len - neg - digit * block >= block)
            number.data[digit] = std::stoi(input.substr(len - (digit + 1) * block, block));
        else
            number.data[digit] = std::stoi(input.substr(neg, len - neg - digit * block));
    }
    return in;
}

bool operator<(const BigInteger& lhs, const BigInteger& rhs) {
    if (lhs.is_zero() && rhs.is_zero())
        return false;
    return ((lhs.is_negative && !rhs.is_negative) ||
            (lhs.is_negative == rhs.is_negative &&
             lhs.compareAbsVal(rhs) == BigInteger::CompareVal::less));
}

bool operator>(const BigInteger& lhs, const BigInteger& rhs) {
    return rhs < lhs;
}

bool operator==(const BigInteger& lhs, const BigInteger& rhs) {
    return ((lhs.is_zero() && rhs.is_zero()) ||
            ((lhs.is_negative == rhs.is_negative) &&
             lhs.compareAbsVal(rhs) == BigInteger::CompareVal::equal));
}

bool operator!=(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs == rhs);
}

bool operator<=(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs > rhs);
}

bool operator>=(const BigInteger& lhs, const BigInteger& rhs) {
    return !(lhs < rhs);
}

BigInteger gcd(const BigInteger& first, const BigInteger& second) {
    if (second == 0)
        return first;
    return gcd(second, first % second);
}


class Rational{
private:
    BigInteger numerator;
    BigInteger denominator;

    void reduce() {
        BigInteger divider = gcd(abs(numerator), denominator);
        numerator /= divider;
        denominator /= divider;
    }
public:
    Rational(): numerator(), denominator(1) {};
    Rational(int number): numerator(number), denominator(1) {};
    Rational(const BigInteger& number): numerator(number), denominator(1) {};
    Rational(const BigInteger& top, const BigInteger& bottom): numerator(top), denominator(bottom) {
        reduce();
    };
    Rational(const Rational& another_num) = default;

    void swap(Rational& another_num) {
        numerator.swap(another_num.numerator);
        denominator.swap(another_num.denominator);
    }

    Rational& operator=(const Rational& another_num) {
        Rational copy(another_num);
        swap(copy);
        return *this;
    }

    ~Rational() = default;

    Rational& operator+=(const Rational& another_num) {
        BigInteger top = numerator * another_num.denominator + denominator * another_num.numerator;
        BigInteger bottom = denominator * another_num.denominator;
        *this = {top, bottom};
        return *this;
    }

    Rational& operator-=(const Rational& another_num) {
        BigInteger top = numerator * another_num.denominator - denominator * another_num.numerator;
        BigInteger bottom = denominator * another_num.denominator;
        *this = {top, bottom};
        return *this;
    }

    Rational& operator*=(const Rational& another_num) {
        BigInteger top = numerator * another_num.numerator;
        BigInteger bottom = denominator * another_num.denominator;
        *this = {top, bottom};
        return *this;
    }

    Rational& operator/=(const Rational& another_num) {
        BigInteger top = numerator * another_num.denominator;
        BigInteger bottom = denominator * another_num.numerator;
        if (bottom < 0){
            bottom *= -1;
            top *= -1;
        }
        *this = {top, bottom};
        return *this;
    }

    Rational operator-() const {
        Rational ans(-numerator, denominator);
        return ans;
    }

    std::string toString() const {
        std::string ans = numerator.toString();
        if (numerator && denominator != 1)
            ans += "/" + denominator.toString();
        return ans;
    }

    std::string asDecimal(size_t precision) const {
        BigInteger extended_numerator(numerator);
        for (size_t pow = 0; pow < precision + 1; pow++)
            extended_numerator *= 10;
        extended_numerator /= denominator;

        bool round_up = false;
        if (extended_numerator % 10 >= 5)
            round_up = true;
        extended_numerator /= 10;
        if (round_up)
            extended_numerator += 1;

        std::string ans = abs(extended_numerator).toString();

        if (ans.size() <= precision) {
            std::string pref(precision - ans.size(), '0');
            ans = "0." + pref + ans;
        }
        else if (precision > 0) {
            ans.insert(ans.begin() + (ans.size() - precision), '.');
        }

        if (extended_numerator < 0)
            ans = "-" + ans;
        return ans;
    }

    explicit operator double() const {
        double ans = stod(asDecimal(308 + 15));
        return ans;
    }

    friend bool operator<(const Rational& lhs, const Rational& rhs);
    friend bool operator==(const Rational& lhs, const Rational& rhs);
};

Rational operator+(const Rational& lhs, const Rational& rhs) {
    Rational ans(lhs);
    ans += rhs;
    return ans;
}

Rational operator-(const Rational& lhs, const Rational& rhs) {
    Rational ans(lhs);
    ans -= rhs;
    return ans;
}

Rational operator*(const Rational& lhs, const Rational& rhs) {
    Rational ans(lhs);
    ans *= rhs;
    return ans;
}

Rational operator/(const Rational& lhs, const Rational& rhs) {
    Rational ans(lhs);
    ans /= rhs;
    return ans;
}

bool operator<(const Rational& lhs, const Rational& rhs) {
    BigInteger top_left = lhs.numerator * rhs.denominator;
    BigInteger top_right = rhs.numerator * lhs.denominator;
    return top_left < top_right;
}

bool operator>(const Rational& lhs, const Rational& rhs) {
    return rhs < lhs;
}

bool operator==(const Rational& lhs, const Rational& rhs) {
    return (lhs.numerator == rhs.numerator && lhs.denominator == rhs.denominator);
}

bool operator!=(const Rational& lhs, const Rational& rhs) {
    return !(lhs == rhs);
}

bool operator<=(const Rational& lhs, const Rational& rhs) {
    return !(lhs > rhs);
}

bool operator>=(const Rational& lhs, const Rational& rhs) {
    return !(lhs < rhs);
}

#endif //INC_1_2_BIGINTEGER_BIGINTEGER_H
