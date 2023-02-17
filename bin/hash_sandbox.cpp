#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "tuple_hash.hpp"

struct Hasher {
  template <typename T>
  std::size_t operator()(T in) const {
    return 5;
  }
};

// is this possible?
// namespace std{
//   template<typename ... T>
//   std::hash{}::operator()(T...) -> std::hash<std::tuple<T...>>::operator();
// };

int main() {
  std::unordered_map<std::string, std::string, Hasher> m{
      {"hello", "world"},
      {"gubl", "gublgubl"},
      {"I am", "Groot"},
      {"this is", "Spartaaa"}};
  std::cout << Hasher{}("hello") << Hasher{}("hellosdfsdf") << '\n';
  for (auto [key, val] : m) std::cout << '[' << key << ", " << val << "]\n";
  std::cout << m.bucket_count() << ", " << m.bucket("hello") << ", "
            << m.bucket_size(m.bucket("hello")) << '\n';

  auto tp = std::make_tuple(2, 4, "klj");
  auto tp2 = std::make_tuple('d', 2.5);
  auto h = std::hash<decltype(tp)>{}(tp);

  std::cout << std::hash<decltype(tp)>{}(tp) << '\n'
            << std::hash<decltype(tp)>{}(tp) << '\n'
            << std::hash<decltype(tp2)>{}(tp2) << '\n';
}
