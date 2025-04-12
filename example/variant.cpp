#include <utility>

#ifdef STD
#include <variant>
using std::variant;
#else
#include <rsl/variant>
using rsl::variant;
#endif

template <std::size_t Idx> 
struct C{};

template <std::size_t... Idx>
auto generate_type(std::index_sequence<Idx...>){
  return std::type_identity<variant<C<Idx>...>>{};
}

template <typename type, std::size_t... Idx>
void construct(std::index_sequence<Idx...>) {
  ((void)type(std::in_place_index<Idx>), ...);
}

template <typename type, std::size_t... Idx>
void get(std::index_sequence<Idx...>){
  auto t = type(std::in_place_index<sizeof...(Idx) - 1>);
  ((void)get<Idx>(t), ...);
}

int main(){
  constexpr auto seq = std::make_index_sequence<250>();
  using type = typename decltype(generate_type(seq))::type;
  construct<type>(seq);
  get<type>(seq);
}