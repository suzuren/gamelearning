#pragma once
#include <cstdint>
#include <cassert>
#include <memory>
#include <type_traits>
#include <string>
#include <string_view>

namespace moon
{
    class buffer_view
    {
    public:
        buffer_view(const char* data, size_t size)
            :data_(data)
            , readpos_(0)
            , size_(size)
        {
        }

        buffer_view(const buffer_view&) = delete;
        buffer_view& operator=(const buffer_view&) = delete;

        ~buffer_view(void)
        {
        }
		//noexcept 指定符(C++11 起)指定函数是否抛出异常。
        template<typename T>
        bool read(T* Outdata, size_t count = 1) noexcept
        {
			// template<class T> struct is_trivially_copyable;
			// 若 T 为可平凡复制 (TriviallyCopyable) 类型，则提供等于 true 的成员常量 value 。对于任何其他类型， value 是 false 。
			// 可平凡复制的对象是仅有的能以 std::memcpy 安全复制或以 std::ofstream::write()/std::ifstream::read() 
			// 序列化自/到二进制文件的对象。通常，可平凡复制的对象是任何底层字节能复制到 char 或 unsigned char 数组，
			// 并复制到同类型的新对象，而结果对象与原对象拥有原值的类型。
            static_assert(std::is_trivially_copyable<T>::value, "type T must be trivially copyable");
            if (nullptr == Outdata || 0 == count)
                return false;

            size_t n = sizeof(T)*count;

            if (readpos_ + n > size_)
            {
                return false;
            }

            auto* buff = data();
            memcpy(Outdata, buff, n);
            readpos_ += n;
            return true;
        }

        template<typename T>
        typename std::enable_if<
            !std::is_same<T, bool>::value &&
            !std::is_same<T, std::string>::value, T>::type
            read()
        {
            static_assert(std::is_trivially_copyable<T>::value, "type T must be trivially copyable.");
            return _read<T>();
        }

        template<typename T>
        typename std::enable_if<
            std::is_same<T, bool>::value, T>::type read()
        {
            return (_read<uint8_t>() != 0) ? true : false;
        }

        template<typename T>
        typename std::enable_if<
            std::is_same<T, std::string>::value, T>::type read()
        {
            std::string tmp;
            while (readpos_ < size_)
            {
                char c = _read<char>();
                if (c == '\0')
                    break;
                tmp += c;
            }
            return std::move(tmp);
        }

        std::string bytes()
        {
            return std::string((const char*)data(), size());
        }

        template<class T>
        std::vector<T> read_vector()
        {
            std::vector<T> vec;
            size_t vec_size = 0;
            (*this) >> vec_size;
            for (size_t i = 0; i < vec_size; i++)
            {
                T tmp;
                (*this) >> tmp;
                vec.push_back(tmp);
            }
            return std::move(vec);
        }


        template<class T>
        buffer_view& operator >> (T& value)
        {
            value = read<T>();
            return *this;
        }

        const char* data() const
        {
            return data_ + readpos_;
        }

        //readable size
        size_t size() const
        {
            return (size_ - readpos_);
        }

        void skip(size_t len)
        {
            if (len < size())
            {
                readpos_ += len;
            }
            else
            {
                readpos_ = size_ = 0;
            }
        }
		// 类模板 basic_string_view 描述一个能指代常量连续仿 char 对象序列的对象，序列首元素在零位置。
		// std::string_view <- std::basic_string_view<char>
        std::string_view readline()
        {
            std::string_view strref(data_ + readpos_, size());
            size_t pos = strref.find("\r\n");
            if (pos != std::string_view::npos)
            {
                readpos_ += (pos + 2);
                return std::string_view(strref.data(), pos);
            }
            pos = size();
            readpos_ += pos;
            return std::string_view(strref.data(), pos);
        }

        std::string_view read_delim(char c)
        {
            std::string_view strref(data_ + readpos_, size());
            size_t pos = strref.find(c);
            if (pos != std::string_view::npos)
            {
                readpos_ += (pos + sizeof(c));
                return std::string_view(strref.data(), pos);
            }
            return std::string_view();
        }

    private:
        template <typename T>
        T _read()
        {
            if ((readpos_ + sizeof(T)) > size_)
                throw std::runtime_error("reading out of size");
            T val = *((T const*)&data_[readpos_]);
            readpos_ += sizeof(T);
            return val;
        }
    protected:
        const char*	data_;
        //read position
        size_t	readpos_;
        //size
        size_t   size_;
    };
};



