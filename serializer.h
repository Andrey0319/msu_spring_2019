#ifndef serializer_h
#define serializer_h

#include <string>
#include <string.h>
#include <type_traits>
#include <stdlib.h>

using namespace std;

enum class Error
{
    NoError,
    CorruptedArchive
};


class Serializer
{
private:
    static constexpr char Separator = ' ';
    ostream &out_;

public:

    explicit Serializer(ostream& out)
        : out_(out)
    {
    }

    template <class T>
    Error save(T&& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(const ArgsT... args)
    {
        return process(args...);
    }

private:

    template <class T, class... Args>
    Error process(const T val)
    {
        constexpr bool is_uint64_t = is_same<T, uint64_t>::value;
        constexpr bool is_bool = is_same<T, bool>::value;

        if (is_uint64_t)
        {
            out_ << val << Separator;
            return Error::NoError;
        }
        if (is_bool)
        {
            if (val)
                out_ << "true" << Separator;
            else
                out_ << "false" << Separator;
            return Error::NoError;
        }
        return Error::CorruptedArchive;
    }

    template <class T, class... Args>
    Error process(const T val, const Args... args)
    {
        if (process(val) != Error::NoError)
            return Error::CorruptedArchive;
        return process(std::forward<const Args>(args)...);
    }
};


class Deserializer
{
    istream &in_;

public:
    explicit Deserializer(istream& in)
        : in_(in)
    {
    }

    template <class T>
    Error load(T&& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&&... args)
    {
        return process(args...);
    }

private:

    template <typename T, class... Args>
    Error process(T&& val)
    {
        constexpr bool is_uint64_t = is_same<T, uint64_t(&)>::value;
        constexpr bool is_bool = is_same<T, bool(&)>::value;

        string txt;
        in_ >> txt;

        if (is_uint64_t && txt[0] >= '0' && txt[0] <= '9')
        {
            val = atoi(txt.c_str());
            return Error::NoError;
        }
        if (is_bool && txt == "true")
        {
            val = true;
            return Error::NoError;
        }
        if (is_bool && txt == "false")
        {
            val = false;
            return Error::NoError;
        }
        return Error::CorruptedArchive;
    }

    template <class T, class... Args>
    Error process(T&& val, Args&&... args)
    {
        if (process(val) != Error::NoError)
            return Error::CorruptedArchive;
        return process(std::forward<Args>(args)...);
    }
};

#endif // serializer