#include <iostream>
#include <any>

#include "Function_cache.hpp"


struct Background{};
struct State{};


template<typename In>
auto f (State& state, In&& input, std::shared_ptr<Background const> background) {
  auto func = [&](State& state, std::any&&, std::shared_ptr<Background const>) -> State& {
    // the costly computation is moved in here.
    // The changes are, that this lambda returns the resulting state instead of changing
    // the input parameter and the type of the input argument is changed to std::any.

    return state;  // placeholder
  };
  static Function_cache func_wrapper{&func, FC_CACHE_FILEPATH, FC_MAX_CACHE_SIZE};
  state = func_wrapper(state, input, background);
}


int main()
{
  std::cout << "hello\n";
}
