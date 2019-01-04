#pragma once
#include <type_traits>
#include <vector>
#include <map>

template<typename T>
struct struct_trait :std::false_type
{
};

#define DECLAR_META(Type,ElmCount)\
    template<>\
    struct struct_trait<Type> :std::true_type\
    {\
        static constexpr size_t count = ElmCount;\
    };\

namespace moon
{
    namespace detail
    {
#define ArgDecl1 arg1
#define ArgDecl2 arg2,ArgDecl1
#define ArgDecl3 arg3,ArgDecl2
#define ArgDecl4 arg4,ArgDecl3
#define ArgDecl5 arg5,ArgDecl4
#define ArgDecl6 arg6,ArgDecl5
#define ArgDecl7 arg7,ArgDecl6
#define ArgDecl8 arg8,ArgDecl7

#define FORWARD_REMOVE_CV(arg) std::forward<decltype(arg)>(arg)

#define ArgForward1 FORWARD_REMOVE_CV(arg1)
#define ArgForward2 FORWARD_REMOVE_CV(arg2),ArgForward1
#define ArgForward3 FORWARD_REMOVE_CV(arg3),ArgForward2
#define ArgForward4 FORWARD_REMOVE_CV(arg4),ArgForward3
#define ArgForward5 FORWARD_REMOVE_CV(arg5),ArgForward4
#define ArgForward6 FORWARD_REMOVE_CV(arg6),ArgForward5
#define ArgForward7 FORWARD_REMOVE_CV(arg7),ArgForward6
#define ArgForward8 FORWARD_REMOVE_CV(arg8),ArgForward7

        template<typename T>
        struct  is_vector :std::false_type {};

        template<typename T>
        struct  is_vector<std::vector<T>> :std::true_type {};

        template<typename T>
        struct  is_map :std::false_type {};

        template<typename Key, typename Value>
        struct  is_map<std::map<Key, Value>> :std::true_type {};

        template<size_t n>
        struct decoder_imp;

        class decoder
        {
        public:
            template<typename TStream, typename T>
            constexpr static void decode(TStream& stream, T& t)
            {
                decoder_imp<struct_trait<std::decay_t<T>>::count>::decode(stream, t);
            }

            template<typename TStream, typename Tuple, size_t... Idx>
            constexpr static void traversal_decode(TStream& stream, Tuple& tp, std::index_sequence<Idx...>)
            {
                (decode_one(stream, std::get<Idx>(tp)), ...);
            }

            template<typename TStream, typename T>
            constexpr static void decode_one(TStream& stream, T& t)
            {
                using value_type = std::decay_t<T>;
                if constexpr (struct_trait<value_type>::value)
                {
                    decode(stream, t);
                }
                else
                {
                    read(stream, t);
                }
            }

            template<typename TStream, typename T, std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>, int> = 0>
            static void read(TStream& stream, T& t)
            {
                stream >> t;
            }

            template<typename TStream, typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, int> = 0>
            static void read(TStream& stream, T& t)
            {
                stream >> t;
            }

            template<typename TStream, typename T, std::enable_if_t<is_vector<std::decay_t<T>>::value, int> = 0>
            static void read(TStream& stream, T& t)
            {
                using vector_type = std::decay_t <T>;
                using value_type = typename vector_type::value_type;
                int size = 0;
                stream >> size;
                for (int i = 0; i < size; ++i)
                {
                    value_type v;
                    decode_one(stream, v);
                    t.emplace_back(std::move(v));
                }
            }
        };

#define DECODER_DEFINE(N)\
        template<>\
        struct decoder_imp<N>\
        {\
            template<typename TStream,typename T>\
            static void decode(TStream& stream,T& t)\
            {\
                auto&[ArgDecl##N] =t; \
                auto tp = std::forward_as_tuple(ArgForward##N);\
                decoder::traversal_decode(stream, tp, std::make_index_sequence<N>{});\
            }\
        };\

        DECODER_DEFINE(1);
        DECODER_DEFINE(2);
        DECODER_DEFINE(3);
        DECODER_DEFINE(4);
        DECODER_DEFINE(5);
        DECODER_DEFINE(6);


        template<size_t n>
        struct encoder_imp;

        class encoder
        {
        public:
			// 常量表达式的值需要在编译时确定
            template<typename TStream, typename T>
            constexpr static void encode(TStream& stream, T& t)
            {
				// std::decay_t 把类型退化为基本形态
                encoder_imp<struct_trait<std::decay_t<T>>::count>::encode(stream, t);
            }

            template<typename TStream, typename Tuple, size_t... Idx>
            constexpr static void traversal_encode(TStream& stream, Tuple& tp, std::index_sequence<Idx...>)
            {
                (encode_one(stream, std::get<Idx>(tp)), ...);
            }

            template<typename TStream, typename T>
            constexpr static void encode_one(TStream& stream, T& t)
            {
                using value_type = std::decay_t<T>;
                if constexpr (struct_trait<value_type>::value)
                {
                    encode(stream, t);
                }
                else
                {
                    write(stream, t);
                }
            }
			// template< class T > struct is_arithmetic;
			// 若 T 为算术类型（即整数类型或浮点类型）或其 cv 限定版本，则提供等于 true 的成员常量 value 。
			// 对于任何其他类型， value 为 false 。
            template<typename TStream, typename T, std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>, int> = 0>
            static void write(TStream& stream, T& t)
            {
                stream << t;
            }

            template<typename TStream, typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, int> = 0>
            static void write(TStream& stream, T& t)
            {
                stream << t;
            }

            template<typename TStream, typename T, std::enable_if_t<is_vector<std::decay_t<T>>::value, int> = 0>
            static void write(TStream& stream, T& t)
            {
                using vector_type = std::decay_t <T>;
                using value_type = typename vector_type::value_type;
                stream << int(t.size());
                for (auto& v : t)
                {
                    encode_one(stream, v);
                }
            }
        };

#define ENCODER_DEFINE(N)\
            template<>\
            struct encoder_imp<N>\
            {\
                template<typename TStream,typename T>\
                static void encode(TStream& stream,const T& t)\
                {\
                    auto&[ArgDecl##N] = t; \
                    auto tp = std::forward_as_tuple(ArgForward##N);\
                    encoder::traversal_encode(stream, tp, std::make_index_sequence<N>{});\
                }\
            };\

        ENCODER_DEFINE(1);
        ENCODER_DEFINE(2);
        ENCODER_DEFINE(3);
        ENCODER_DEFINE(4);
        ENCODER_DEFINE(5);
        ENCODER_DEFINE(6);
    }

    class serialize
    {
    public:
        template<typename T, typename TStream>
        T decode(TStream& stream)
        {
            T res;
            detail::decoder::decode(stream, res);
            return std::move(res);
        }

        template<typename T, typename TStream>
        void encode(TStream& stream, const T& t)
        {
            detail::encoder::encode(stream, t);
        }
    };
}


