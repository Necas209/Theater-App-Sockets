#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include "json.hpp"

// ReSharper disable once CppRedundantNamespaceDefinition
namespace nlohmann {
    template<typename Clock, typename Duration>
    struct adl_serializer<std::chrono::time_point<Clock, Duration> > {
        static void to_json(json &j, const std::chrono::time_point<Clock, Duration> &tp) {
            j["since_epoch"] = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count();
        }

        static void from_json(const json &j, std::chrono::time_point<Clock, Duration> &tp) {
            const auto count = j.at("since_epoch").get<std::chrono::microseconds::rep>();
            tp = std::chrono::time_point<Clock, Duration>(std::chrono::microseconds(count));
        }
    };
}

template<typename T,
    typename TIter = decltype(std::begin(std::declval<T>())),
    typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T &&iterable) {
    struct iterator {
        size_t i;
        TIter iter;
        bool operator !=(const iterator &other) const { return iter != other.iter; }

        void operator ++() {
            ++i;
            ++iter;
        }

        auto operator *() const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper {
        T iterable;
        auto begin() { return iterator{0, std::begin(iterable)}; }
        auto end() { return iterator{0, std::end(iterable)}; }
    };
    return iterable_wrapper{std::forward<T>(iterable)};
}

#endif //EXTENSIONS_H
