#include <iostream>

#include <cstring>

#include <algorithm>

#include <cmath>

class String
{
private:
    void new_memory()
    {
        char *new_str = new char[cap * 2];
        memcpy(new_str, arr, sz);
        delete[] arr;
        cap *= 2;
        arr = new_str;
    }

    friend std::ostream &operator<<(std::ostream &os, const String &stroka);
    friend std::istream &operator>>(std::istream &is, String &stroka);
    friend bool operator==(const String &, const String &);
    friend bool operator<(const String &, const String &);
    char *arr;  // указатель на начало строки
    size_t sz;  // реальное количество элементов
    size_t cap; // под сколько элементов зарезервировано памяти

public:
    // КОНСТРУКТОРЫ //

    // конструктор по умолчанию
    String() : arr(new char[1]), sz(0), cap(0) {}

    // конструктор C-Style строки
    String(const char *cstyle) : arr(new char[strlen(cstyle) + 1]), sz(strlen(cstyle)), cap(strlen(cstyle))
    {
        memcpy(arr, cstyle, sz);
    }

    // конструктор создающий строку из n копий символа c
    String(size_t n, char c) : arr(new char[n + 1]), sz(n), cap(n)
    {
        memset(arr, c, n);
        arr[n] = '\0';
    }

    // конструктор копирующий
    String(const String &stroka) : arr(new char[stroka.cap + 1]), sz(stroka.sz), cap(stroka.cap)
    {
        memcpy(arr, stroka.arr, sz + 1);
    }

    ~String()
    {
        delete[] arr;
    }
    // ОПЕРАТОРЫ //

    // оператор присваивания copy & swap
    String &operator=(String stroka)
    {
        swap(stroka);
        return *this;
    }

    void swap(String &stroka)
    {
        std::swap(arr, stroka.arr);
        std::swap(sz, stroka.sz);
        std::swap(cap, stroka.cap);
    }

    // оператор [], возвращаем ссылку, чтобы поддерживалосьь присваивание по индексу
    char &operator[](size_t index)
    {
        return arr[index];
    }

    const char &operator[](size_t index) const
    {
        return arr[index];
    }

    size_t length() const
    {
        return sz;
    }

    size_t capacity() const
    {
        return cap;
    }

    char &front()
    {
        return arr[0];
    }

    const char &front() const
    {
        return arr[0];
    }

    char &back()
    {
        return arr[sz - 1];
    }

    const char &back() const
    {
        return arr[sz - 1];
    }

    bool empty()
    {
        return (sz == 0);
    }

    void clear()
    {
        sz = 0;
    }

    char *data()
    {
        return arr;
    }

    const char *data() const
    {
        return arr;
    }

    String substr(size_t start, size_t count) const
    {
        String sub(count, '!');
        for (size_t i = start; i < start + count; i++)
        {
            sub[i - start] = arr[i];
        }
        return sub;
    }

    void shrink_to_fit()
    {
        char *newarr = new char[sz + 1];
        strcpy(newarr, arr);
        delete[] arr;
        arr = newarr;
    }

    size_t find(const String &substring) const
    {
        if (substring.sz > sz)
        {
            return substring.length();
        }
        for (size_t i = 0; i <= sz - substring.sz; ++i)
        {
            if (this->substr(i, substring.sz) == substring)
            {
                return i;
            }
        }
    }

    size_t rfind(const String &substring) const
    {
        if (substring.sz > sz)
        {
            return substring.length();
        }
        int r = 0;
        for (size_t i = 0; i <= sz - substring.sz; ++i)
        {
            if (this->substr(i, substring.sz) == substring)
            {
                r = i;
            }
        }
        return r;
    }

    void push_back(char bukva)
    {
        if (sz == cap)
        {
            new_memory();
        }
        arr[sz] = bukva;
        sz += 1;
    }

    void pop_back()
    {
        sz -= 1;
        if (sz < cap / 4)
        {
            char *new_str = new char[cap / 2];
            memcpy(new_str, arr, sz);
            delete[] arr;
            cap /= 2;
            arr = new_str;
        }
    }

    String &operator+=(char bukva)
    {
        push_back(bukva);
        return *this;
    }

    String &operator+=(const String &stroka)
    {
        size_t allsize = sz + stroka.sz;
        size_t iter = ceil(allsize / sz);
        for (size_t i = 0; i < iter; ++i)
        {
            new_memory();
        }
        for (size_t i = sz; i < allsize; ++i)
        {
            arr[i] = stroka.arr[i - sz];
        }
        sz += stroka.length();
        return *this;
    }

    String operator+(char bukva) const
    {
        String result = *this;
        result += bukva;
        return result;
    }

    String operator+(const String &stroka) const
    {
        String result = *this;
        result += stroka;
        return result;
    }
};

std::ostream &operator<<(std::ostream &os, const String &stroka)
{
    for (size_t i = 0; i < stroka.sz; ++i)
    {
        os << stroka.arr[i];
    }
    return os;
}

std::istream &operator>>(std::istream &is, String &stroka)
{
    int i = 0;
    char c;
    while (is.get(c))
    {
        stroka.arr[i] = c;
        i++;
    }
    return is;
}

bool operator==(const String &stroka1, const String &stroka2)
{
    if (stroka1.sz != stroka2.sz)
    {
        return false;
    }
    for (size_t i = 0; i < stroka1.sz; ++i)
    {
        if (stroka1.arr[i] != stroka2.arr[i])
        {
            return false;
        }
    }
    return true;
}

// оператор проверки неравенства строк
bool operator!=(const String &stroka1, const String &stroka2)
{
    return !(stroka1 == stroka2);
}

// оператор сравнения меньше
bool operator<(const String &stroka1, const String &stroka2)
{
    if (stroka1.sz < stroka2.sz)
    {
        return true;
    }
    if (stroka1.sz > stroka2.sz)
    {
        return false;
    }
    for (size_t i = 0; i < stroka1.sz; ++i)
    {
        if (stroka1.arr[i] < stroka2.arr[i])
        {
            return true;
        }
    }
    return false;
}

bool operator>(const String &stroka1, const String &stroka2)
{
    return (!(stroka1 < stroka2) && (stroka1 != stroka2));
}

bool operator<=(const String &stroka1, const String &stroka2)
{
    return ((stroka1 < stroka2) || (stroka1 == stroka2));
}

bool operator>=(const String &stroka1, const String &stroka2)
{
    return (!(stroka1 < stroka2) || (stroka1 == stroka2));
}

int main()
{
    String s;
}