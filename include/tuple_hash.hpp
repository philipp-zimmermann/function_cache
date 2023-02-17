#ifndef _FUNCTION_CACHE_
#define _FUNCTION_CACHE_

#include <tuple>
#include <utility>

namespace tuple_hash {
template <class T>
struct component {
  const T& value;
  constexpr component(const T& value) noexcept : value(value) {}
  constexpr uintmax_t operator^=(uintmax_t n) const {
    n ^= std::hash<T>()(value);
    return n ^= n << (sizeof(uintmax_t) * 4 - 1);
  }
};
}  // namespace tuple_hash

template <class... value_types>
struct std::hash<std::tuple<value_types...>> {
  constexpr std::size_t operator()(
      std::tuple<value_types...> const& tuple) const noexcept {
    return std::hash<uintmax_t>()(std::apply(
        [](const auto&... xs) {
          return (tuple_hash::component(xs) ^= ... ^= 0);
        },
        tuple));
  }

 private:
};

#endif  // _FUNCTION_CACHE_
