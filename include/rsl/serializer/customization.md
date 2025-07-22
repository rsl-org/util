# Customization

## Primary template
```cpp
struct Zoinks{
  int foo;
};

template <>
struct rsl::preferred_name<Zoinks> {
  constexpr inline auto value = "Foo";
}
```

## Annotated classes
Class types can be annotated with `preferred_name` to set a preferred name. For example
```cpp
struct [[=rsl::preferred_name("Bar")]] Foo{};

template <typename T>
struct [[=rsl::preferred_name("Foo<"s + display_string_of(^^T) + ">")]] Bar {};
```


## Member Constant
```cpp
struct Boings {
  int foo;

  constexpr static auto preferred_name = "Bar";
};

```

## Member Functions
```cpp
struct Foo {
  static consteval char const* preferred_name() { return "F"; }
};
```
The return type of `preferred_name` must be convertible to `std::string_view`. `preferred_name` members are only considered if they are static and evaluatable in constant-evaluated context.

## ADL
```cpp
struct Foo {
  friend consteval std::string_view preferred_name(std::type_identity<Foo>) { return "A"; }
};

struct Bar {};
consteval std::string_view preferred_name(std::type_identity<Bar>) { return "B"; }
```

The return type of `preferred_name` must be convertible to `std::string_view`. `preferred_name` members are only considered if they are static and evaluatable in constant-evaluated context.

## Precedence
Template Specialization > Annotation > Member Constant > Member Function > ADL 



# Structure

## Annotated Class
For a non-aggregate class with annotated members, ie
```cpp
struct Foo {
  [[=rsl::serializable]] int x;
  std::string b;
  [[=rsl::serializable]] char c;

  [[=rsl::deserializable]] Foo(int a, char b) {}
};
```
That class is only considered deserializable if it has a constructor or static member function annotated with `deserializable`, and every parameter thereof corresponds to a public non-static data member of same type in declaration order of the members. If multiple such members are found, any attempt to deserialize renders the program ill-formed. (TODO throw constexpr exception instead?)

## Described
Class structure can be described inline or out-of-line.
```cpp
struct Boings {
  int foo;
  constexpr static auto structure = Class{&Boings::foo};
};


struct Zoinks { int foo; };

template <>
constexpr inline auto describe<Zoinks> = Class{&Zoinks::foo};
```

