#pragma once

#include <bitset>
#include <map>
#include <memory>
#include <stdint.h>
#include <string.h>
#include <string>
#include <type_traits>
#include <vector>

#include "Error.h"

namespace efs {
namespace serialize {
    // size/////////////////////////////////////////////////////
    template <class T>
    typename std::enable_if<std::is_class<T>::value, int32_t>::type size(const T& a)
    {
        return a.size();
    }

    template <class T>
    typename std::enable_if<std::is_arithmetic<T>::value, int32_t>::type size(const T& a)
    {
        int s = sizeof(a);
        return s;
    }

    template <class T>
    typename std::enable_if<std::is_enum<T>::value, int32_t>::type size(const T& a)
    {
        int s = sizeof(a);
        return s;
    }

    template <>
    inline int32_t size(const std::string& a)
    {
        return a.size() + 4;
    }

    template <class T>
    int32_t size(const std::vector<T>& vs)
    {
        int32_t s = 4;
        for (int i = 0; i < int(vs.size()); i++) {
            s += serialize::size(vs[i]);
        }
        return s;
    }

    // serialize/////////////////////////////////////////////////////
    template <class T>
    typename std::enable_if<std::is_class<T>::value, int32_t>::type serialize(const T& a, char* buf, int32_t bufsize)
    {
        return a.serialize(buf, bufsize);
    }

    template <class T>
    typename std::enable_if<std::is_arithmetic<T>::value, int32_t>::type serialize(const T& a, char* buf, int32_t bufsize)
    {
        int size = sizeof(a);
        memcpy(buf, &a, size);
        return size;
    }

    template <class T>
    typename std::enable_if<std::is_enum<T>::value, int32_t>::type serialize(const T& a, char* buf, int32_t bufsize)
    {
        int size = sizeof(a);
        memcpy(buf, &a, size);
        return size;
    }

    template <>
    inline int32_t serialize<std::string>(const std::string& a, char* buf, int32_t bufsize)
    {
        int32_t size = 0;
        int32_t len = a.size();
        size += serialize::serialize(len, buf + size, bufsize - size);
        memcpy(buf + size, a.c_str(), len);
        size += len;
        return size;
    }

    template <class T>
    int32_t serialize(const std::vector<T>& vs, char* buf, int32_t bufsize)
    {
        int32_t size = 0;
        int32_t cnt = vs.size();
        size += serialize::serialize(cnt, buf + size, bufsize - size);
        for (int i = 0; i < int(vs.size()); i++) {
            size += serialize::serialize(vs[i], buf + size, bufsize - size);
        }
        return size;
    }

    // deserialize//////////////////////////////////////////////////
    template <class T>
    typename std::enable_if<std::is_class<T>::value, int32_t>::type deserialize(T& a, const char* buf, int32_t bufsize)
    {
        return a.deserialize(buf, bufsize);
    }

    template <class T>
    typename std::enable_if<std::is_arithmetic<T>::value, int32_t>::type deserialize(T& a, const char* buf, int32_t bufsize)
    {
        int size = sizeof(a);
        if (bufsize < size) {
            return -1;
        }
        memcpy(&a, buf, size);
        return size;
    }

    template <class T>
    typename std::enable_if<std::is_enum<T>::value, int32_t>::type deserialize(T& a, const char* buf, int32_t bufsize)
    {
        int size = sizeof(a);
        if (bufsize < size) {
            return -1;
        }
        memcpy(&a, buf, size);
        return size;
    }

    template <>
    inline int32_t deserialize(std::string& a, const char* buf, int32_t bufsize)
    {
        int32_t size = 0;
        int32_t len = 0;

        int32_t size1 = serialize::deserialize(len, buf + size, bufsize - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        if (len > bufsize - size) {
            return -1;
        }
        a = std::string(buf + size, len);
        size += len;
        return size;
    }

    template <class T>
    int32_t deserialize(std::vector<T>& vs, const char* buf, int32_t bufsize)
    {
        int32_t size = 0;
        int32_t cnt = 0;
        int32_t size1 = serialize::deserialize(cnt, buf + size, bufsize - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        vs.clear();
        for (int i = 0; i < cnt; i++) {
            T v;
            size1 = serialize::deserialize(v, buf + size, bufsize - size);
            if (size1 < 0) {
                return -1;
            }
            size += size1;
            vs.push_back(v);
        }
        return size;
    }
}
}