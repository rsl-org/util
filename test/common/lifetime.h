#pragma once
#include <cstdio>
#include <vector>
#include <gmock/gmock.h>

struct State {
  enum Event { DefaultCtor, Ctor, CopyCtor, MoveCtor, Dtor, CopyAssign, MoveAssign, Visit } event;
  std::size_t id;

  State(Event event_, std::size_t id_): event(event_), id(id_){}

  static char const* to_string(Event obj) {
    switch (obj) {
      case DefaultCtor: return "DefaultCtor";
      case Ctor: return "Ctor";
      case CopyCtor: return "CopyCtor";
      case MoveCtor: return "MoveCtor";
      case Dtor: return "Dtor";
      case CopyAssign: return "CopyAssign";
      case MoveAssign: return "MoveAssign";
      case Visit: return "Visit";
      default: return "unknown action";
    }
  }

  friend bool operator==(State const& lhs, State const& rhs) { return (lhs.id == rhs.id) && (lhs.event == rhs.event); }
  friend void PrintTo(State const& obj, std::ostream* os) {
    *os << '{' << State::to_string(obj.event) << ", " << obj.id << '}';
  }
};

class LifetimeTracker {
private:
  LifetimeTracker()  = default;
  ~LifetimeTracker() = default;

  std::vector<State> states;

public:
  LifetimeTracker(LifetimeTracker const&)            = delete;
  LifetimeTracker(LifetimeTracker&&)                 = delete;
  LifetimeTracker& operator=(LifetimeTracker const&) = delete;
  LifetimeTracker& operator=(LifetimeTracker&&)      = delete;

  static LifetimeTracker& instance() {
    static LifetimeTracker obj{};
    return obj;
  }

  static std::vector<State> const& get() { return instance().states; }

  static void append(State state) { instance().states.push_back(state); }

  static void reset() { instance().states.clear(); }

  [[nodiscard]] static bool matches(std::vector<State> const& other) { return instance().states == other; }
  [[nodiscard]] static bool is_empty() { return instance().states.empty(); }

  static void assert_equal(std::vector<State> const& other) {
    ASSERT_THAT(get(), testing::ElementsAreArray(other));
    reset();
  }

  static void print() {
    for (auto&& [event, id] : instance().states) {
      fprintf(stderr, "%zu - %s\n", id, State::to_string(event));
    }
  }
};

template <std::size_t V>
struct Lifetime {
  static constexpr auto value = V;

  Lifetime() { LifetimeTracker::append({State::DefaultCtor, V}); }
  constexpr explicit Lifetime(int) { LifetimeTracker::append({State::Ctor, V}); }
  constexpr Lifetime(Lifetime const&) { LifetimeTracker::append({State::CopyCtor, V}); }
  constexpr Lifetime(Lifetime&&) { LifetimeTracker::append({State::MoveCtor, V}); }
  ~Lifetime() { LifetimeTracker::append({State::Dtor, V}); }
  constexpr Lifetime& operator=(Lifetime const&) {
    LifetimeTracker::append({State::CopyAssign, V});
    return *this;
  }
  constexpr Lifetime& operator=(Lifetime&&) {
    LifetimeTracker::append({State::MoveAssign, V});
    return *this;
  }
};