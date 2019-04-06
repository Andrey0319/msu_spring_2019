#ifndef serializer_h
#define serializer_h

#include <string>
#include <string.h>
#include <typeinfo>
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
    Error operator()(ArgsT... args)
    {
        return process(args...);
    }

private:
    template <class T, class... Args>
    Error process(T&& val)
    {
        if (typeid(T) == typeid(uint64_t))
        {
            out_ << val;
            return Error::NoError;
        }
        if (typeid(T) == typeid(bool))
        {
            if (val)
                out_ << "true";
            else
                out_ << "false";
            return Error::NoError;
        }
        return Error::CorruptedArchive;
    }

    template <class T, class... Args>
    Error process(T&& val, Args&&... args)
    {
        if (typeid(T) == typeid(uint64_t))
        {
            out_ << val << Separator;
            process(std::forward<Args>(args)...);
            return Error::NoError;
        }
        if (typeid(T) == typeid(bool))
        {
            if (val)
                out_ << "true" << Separator;
            else
                out_ << "false" << Separator;
            process(std::forward<Args>(args)...);
            return Error::NoError;
        }
        return Error::CorruptedArchive;
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
    Error load(T &object)
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
    Error process(T &val)
    {
        string txt;
        in_ >> txt;

        if (typeid(T) == typeid(uint64_t) && txt[0] >= '0' && txt[0] <= '9')
        {
            val = atoi(txt.c_str());
            return Error::NoError;
        }
        if (typeid(T) == typeid(bool) && txt == "true")
        {
            val = true;
            return Error::NoError;
        }
        if (typeid(T) == typeid(bool) && txt == "false")
        {
            val = false;
            return Error::NoError;
        }
        return Error::CorruptedArchive;
    }

    template <class T, class... Args>
    Error process(T &val, Args&&... args)
    {
        string txt;
        in_ >> txt;

        if (typeid(T) == typeid(uint64_t) && txt[0] >= '0' && txt[0] <= '9')
        {
            val = atoi(txt.c_str());
            return process(std::forward<Args>(args)...);
        }
        if (typeid(T) == typeid(bool) && txt == "true")
        {
            val = true;
            return process(std::forward<Args>(args)...);
        }
        if (typeid(T) == typeid(bool) && txt == "false")
        {
            val = false;
            return process(std::forward<Args>(args)...);
        }
        return Error::CorruptedArchive;
    }
};

#endif // serializer
