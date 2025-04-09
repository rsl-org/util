#pragma once
#include <iostream>

struct Qualifiers {
  enum CV {
    None     = 0,
    Const    = 1,
    Volatile = 2
  } qualifiers {None};

  enum Category {
    Lvalue,
    Rvalue
  } category {Lvalue};

  friend auto operator<=>(Qualifiers const&, Qualifiers const&) noexcept = default;
  friend std::ostream& operator<<(std::ostream& stream, Qualifiers const& self){
    if ((self.qualifiers & Const) != 0){
      stream << "const ";
    } 
    if ((self.qualifiers & Volatile) != 0){
      stream << "volatile ";
    }

    if (self.category == Lvalue) {
      stream << "lvalue";
    } else if (self.category == Rvalue) {
      stream << "rvalue";
    }

    return stream;
  }
};