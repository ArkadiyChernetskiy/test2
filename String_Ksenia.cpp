//
// Created by YUIP on 04.11.2021.
//

#ifndef STRING_STRING_H
#define STRING_STRING_H
#include <iostream>
#include <cstring>

//
class String {
private:
    char *data ;//array of chars
    size_t size ;//current_length
    size_t capacity ;//max_length
    void changeCapacity(size_t newSize);
    void expandCapacity ();

public:
    String ( const char* c);
    String( size_t n, char c);
    String( const String& str); //конструктор копирования
    String& operator=( const String& other) &;
    String(); // конструктор по умолчанию
    ~String();
    friend bool operator==(const String& first, const String& second);
    friend bool operator==(const String& s, const char& c);
    friend bool operator==(const char& c, const String& s);

    void push_back (char a);
    void pop_back();
    char front() const;
    char back() const;
    char& front();
    char& back();
    size_t length() const;

    char& operator[] (size_t index) ;
    char operator[] (size_t index) const;
    String& operator += (const String& other);
    String& operator+=( const char& c);

    size_t find( const String& other) const;
    size_t rfind( const String& other) const;
    String substr( size_t start, size_t count) const;

    friend std::ostream& operator <<(std::ostream& out, const String& s);
    friend std::istream& operator >>(std::istream& in, String& s) ;

    bool empty() const;
    void clear();


};
//private functions
void String::changeCapacity(size_t newSize){
    capacity = newSize;
    char* newData = new char[newSize];
    memcpy (newData, data, size);
    delete[] (data);
    data = newData;
}
void String::expandCapacity (){
    if (size >= capacity){
        changeCapacity (size*2);
    }
}
//constructors and so on
String::String (const char* c){
    size=strlen(c);
    capacity = strlen(c)*2+1;
    data = new char[capacity];
    memcpy (data, c, size);
}

String::String() {
    data= new char [1];
    size=0;
    capacity=1;
}

String::String( size_t n, char c){
    size=n;
    capacity=2*n+1;
    data=new char[capacity];
    // memset (data, c, n);
    for (size_t i=0; i<size; i++){
        data[i]=c;
    }
}
String::String( const String& str) : data(new char[str.capacity]), size(str.size), capacity(str.capacity){
    memcpy( data, str.data, size);
}

bool operator==(const String& first, const String& second){
    if ( first.size!=second.size) return false;
    if ( memcmp(first.data, second.data, first.size)==0){ return true;}
    return false;
}

bool operator==(const String& s, const char& c){
    if (s.size!=1) return false;
    if (s[0]!=c) return false;
    return true;
};

bool operator==(const char& c, const String& s){
    return ( s==c);
};
String& String::operator=( const String& other) &{
    if (this == &other) {
        return *this;
    }
    size = other.size;
    capacity=other.capacity;
    delete[] data;
    data = new char[capacity];
    memcpy(data, other.data, size);
    return *this; // адрес- this
}
String::~String() {
    delete[] data;//array of chars
}

void String::push_back (char a){
    expandCapacity();
    size+=1;
    data[size-1]=a;
}
void String::pop_back(){
    size-=1;
}

char String::front() const{
    return data[0];
}
char& String::front() {
    return data[0];
}

char String::back() const{
    return data[size-1];
}
char& String::back(){
    return data[size-1];
}


char String::operator[] (size_t index) const{
    return data[index];
}
char& String::operator[] (size_t index) {
    return data[index];
}

String& String::operator+= (const String& other){ //работает только для константной строки(прибавляемой) или для всех?
    if ( capacity< size+other.size){
        changeCapacity(2*(size+other.size));
    }
    memcpy(&data[size], other.data,  other.size); //data
    size+=other.size;
    return *this;
}
String& String::operator+=( const char& c){
    push_back(c);
    return *this;
}
String operator+( const String& first,const String& second){
    String new_str=first;
    new_str+=second;
    return new_str;
}
String operator+( const String& str, const char& c){
    return str+String(1, c);//мб стоит здесь поменять
}
String operator+( const char& c, const String& str) {
    return String(1, c)+str;
}

size_t String::find( const String& other) const{
    if (other.size==0){
        return 0;
    }
    if (other.size>size){
        return length();
    }
    size_t i=0;
    while( i<size-other.size){
        if (substr(i, other.size)==other) return i;
        i++;
    }
    return length();
};
size_t String::rfind( const String& other) const{
    if (other.size==0){
        return length()-1;
    }
    if (other.size>size){
        return length();
    }
    size_t i=size-other.size+1; //индекс с которого начинаем читать строку
    while (i>0){
        if (substr(i-1, other.size)==other) return i-1;
        i--;
    }
    return length();
}

String String::substr( size_t start, size_t count) const{
    String sub( count, '\0');
    memcpy(sub.data, data+start, count);
    return sub;
}

std::ostream& operator <<(std::ostream& out, const String& s) {
    for (size_t i=0; i<s.size; i++){
        out<< s.data[i];
    }
    return out;
};
std::istream& operator >>(std::istream& in, String& s) {
    s.clear();
    char c;
    while (isspace(in.peek())){
        if(!in.get(c)) { break; }
    }

    while ( in.get(c) && !isspace(c) ){
        s.push_back(c);
    }
    return in;
};

size_t String::length() const{
    return size;
}
bool String::empty() const{
    if (size==0){
        return true;
    }
    return false;
}
void String::clear(){
    size=0;
    changeCapacity(1);

}

#endif //STRING_STRING_H
