#ifndef FUNCTION_CACHE_HPP
#define FUNCTION_CACHE_HPP


#include <filesystem>
#include <unordered_map>
#include <optional>
#include <tuple>

#include "tuple_hash.hpp"

/// @brief This class implements a function wrapper that provides a cache. Calls to a
///        passed function `func` are cached, together with their results. If a cached
///        function call is called again, the result is read from the cache instead of
///        being computed again.
/// @tparam ...Func_input Types of input parameters for `func`.
/// @tparam Func_output output parameter for `func`.
template<typename Func_output, typename... Func_input>
class Function_cache
{
 public:
  /// @brief Constructs a Function cache object for the passed function `func`
  /// @param func A function pointer to the function that is to be wrapped.
  /// @param cache_file The filepath to the persistent cache file.
  /// @param max_cache_size The maximal amount of cached function calls.
  Function_cache(Func_output(*func)(Func_input...),
                  const std::filesystem::path& cache_file = {},
                  std::size_t max_cache_size = 100)
    : func_{func}
    , cache_{load_cache(cache_file)}
    , max_cache_size_{max_cache_size}
    , cache_file_{cache_file}
  {}


  ~Function_cache() {
    save_cache(cache_file_);
  }


  /// @brief This is the substitute function to call instead of the function `func` with
  ///        which this Function_cache object was constructed.
  /// @param ...f_in input parameters for func.
  /// @return returns the output of func, when called with f_in.
  const Func_output operator()(Func_input... f_in) {
    if (auto cache_result = find_and_update(std::make_tuple(f_in...)); cache_result) {
      return cache_result->get();
    } else {
      auto f_out = func_(f_in...);
      insert_new_entry(std::make_tuple(f_in...), f_out);
      control_cache_size();
      return f_out;
    }
  }


 private:
  using Cache_key_t = std::tuple<Func_input...>;
  struct Cache_Entry {
    Func_output f_out_;
    typename std::unordered_map<Cache_key_t, Cache_Entry>::iterator prev_;
    typename std::unordered_map<Cache_key_t, Cache_Entry>::iterator next_;
  };
  using Cache_t = std::unordered_map<Cache_key_t, Cache_Entry>;


  Func_output(*const func_)(Func_input...);
  Cache_t cache_;
  std::size_t max_cache_size_;
  typename Cache_t::iterator newest_cache_entry_ = {};
  typename Cache_t::iterator oldest_cache_entry_ = {};
  const std::filesystem::path& cache_file_;


  Cache_t load_cache(const std::filesystem::path& cache_file) {
    // reading the cache from memory is to be implemented here. This is only a placeholder
    return {};
  }
  void save_cache(const std::filesystem::path& cache_file) {
    // writing the cache to memory is to be implemented here. This is only a placeholder
      return;
  }


  /// @brief updates the order of cashed entries and makes the bassed item the newest
  /// @param f_in a entry that already exists in cache_
  void make_newest_entry(const typename Cache_t::iterator f_in) {
    if(f_in == newest_cache_entry_) return;

    if(f_in == oldest_cache_entry_){
      oldest_cache_entry_ = cache_.at(oldest_cache_entry_).prev_;
      cache_.at(oldest_cache_entry_).next_ = cache_.end();
      cache_.at(newest_cache_entry_).prev_ = f_in;
      newest_cache_entry_ = f_in;
      return;
    }

    //update surrounding entries
    auto next = cache_.at(f_in).next_;
    auto prev = cache_.at(f_in).prev_;
    cache_.at(next).prev_ = prev;
    cache_.at(prev).next_ = next;
    // update the cache for the found input
    cache_.at(f_in).prev_ = cache_.end();
    cache_.at(f_in).next_ = newest_cache_entry_;
    // update the newest entry, since the found entry is now the newest
    cache_.at(newest_cache_entry_).prev_ = f_in;
    newest_cache_entry_ = f_in;
  }


  /// @brief searches for the passed key in cache_. If found returns the value and makes
  ///        this entry the newest.
  /// @param key the key for wich is searched.
  /// @return returns the value if the key was found, otherwise std::nullopt is returned.
  std::optional<Func_output> find_and_update(const Cache_key_t& key) {
    if(auto cached_out = cache_.find(key); cached_out != cache_.end()) {
      make_newest_entry(key);
      return cached_out;
    }
    return std::nullopt;
  }


  /// @brief inserts the passed key and output into cache_ and makes this entry the newest
  /// @param key
  /// @param f_out
  void insert_new_entry(const Cache_key_t& key, const Func_output& f_out) {
    const auto new_entry
      = cache_.emplace(key, {f_out, cache_.end(), newest_cache_entry_}).first;
    cache_.at(newest_cache_entry_).prev_ = new_entry;
    newest_cache_entry_ = new_entry;
  }


  /// @brief checks the size of the cache_. If it exceeds the specified limit the oldest
  ///        entry is removed from cache_.
  void control_cache_size() {
    if(cache_.size() > max_cache_size_){
      auto second_oldest = cache_.find(oldest_cache_entry_).prev_;
      second_oldest.next_ = cache_.end();
      cache_.erase(oldest_cache_entry_);
      oldest_cache_entry_ = second_oldest;
    }
  }
};


#endif  // FUNCTION_CACHE_HPP
