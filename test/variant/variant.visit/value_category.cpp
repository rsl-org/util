#include <utility>

#include <gtest/gtest.h>
#include <rsl/variant>

#include <common/assertions.h>
#include <common/util.h>

#include "forwarding_visitor.h"


TEST(VisitNoVariant, CallOperatorForwarding) {
  auto visitor              = ForwardingTestVisitor{};
  auto const& const_visitor = visitor;

  auto result0 = rsl::visit(visitor);
  result0.template verify<>({Qualifiers::CV::None, Qualifiers::Lvalue});

  auto result1 = rsl::visit(const_visitor);
  result1.template verify<>({Qualifiers::Const, Qualifiers::Lvalue});

  auto result2 = rsl::visit(std::move(visitor));
  result2.template verify<>({Qualifiers::CV::None, Qualifiers::Rvalue});

  auto result3 = rsl::visit(std::move(const_visitor));
  result3.template verify<>({Qualifiers::Const, Qualifiers::Rvalue});
}

TEST(Visit, CallOperatorForwardingSingleVariant) {
  auto visitor              = ForwardingTestVisitor{};
  auto const& const_visitor = visitor;

  auto obj = rsl::variant<int, char, float>{42};

  auto result0 = rsl::visit(visitor, obj);
  result0.template verify<int&>({Qualifiers::CV::None, Qualifiers::Lvalue});

  auto result1 = rsl::visit(const_visitor, obj);
  result1.template verify<int&>({Qualifiers::Const, Qualifiers::Lvalue});

  auto result2 = rsl::visit(std::move(visitor), obj);
  result2.template verify<int&>({Qualifiers::CV::None, Qualifiers::Rvalue});

  auto result3 = rsl::visit(std::move(const_visitor), obj);
  result3.template verify<int&>({Qualifiers::Const, Qualifiers::Rvalue});
}

TEST(Visit, CallOperatorForwardingMultiVariant) {
  auto visitor              = ForwardingTestVisitor{};
  auto const& const_visitor = visitor;

  auto obj  = rsl::variant<int, char, float>{42};
  auto obj2 = rsl::variant<float, double>{42.F};

  auto result0 = rsl::visit(visitor, obj, obj2);
  result0.template verify<int&, float&>({Qualifiers::CV::None, Qualifiers::Lvalue});

  auto result1 = rsl::visit(const_visitor, obj, obj2);
  result1.template verify<int&, float&>({Qualifiers::Const, Qualifiers::Lvalue});

  auto result2 = rsl::visit(std::move(visitor), obj, obj2);
  result2.template verify<int&, float&>({Qualifiers::CV::None, Qualifiers::Rvalue});

  auto result3 = rsl::visit(std::move(const_visitor), obj, obj2);
  result3.template verify<int&, float&>({Qualifiers::Const, Qualifiers::Rvalue});
}

TEST(Visit, ArgumentForwardingSingleVariant) {
  auto visitor = ForwardingTestVisitor{};

  auto obj              = rsl::variant<int, std::string, float>{42};
  auto const& const_obj = obj;

  auto result0 = rsl::visit(visitor, obj);
  result0.template verify<int&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  auto result1 = rsl::visit(visitor, const_obj);
  result1.template verify<int const&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  auto result2 = rsl::visit(visitor, std::move(obj));
  result2.template verify<int&&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  auto result3 = rsl::visit(visitor, std::move(const_obj));
  result3.template verify<int const&&>({Qualifiers::CV::None, Qualifiers::Lvalue});
}

TEST(Visit, ArgumentForwardingMultiVariant) {
  auto visitor = ForwardingTestVisitor{};

  auto obj              = rsl::variant<int, std::string, float>{42};
  auto const& const_obj = obj;

  auto obj2              = rsl::variant<double, float>{42.0};
  auto const& const_obj2 = obj2;

  rsl::visit(visitor, obj, obj2).template verify<int&, double&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, const_obj, obj2).template verify<int const&, double&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, obj, const_obj2).template verify<int&, double const&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, const_obj, const_obj2)
      .template verify<int const&, double const&>({Qualifiers::CV::None, Qualifiers::Lvalue});

  rsl::visit(visitor, std::move(obj), obj2).template verify<int&&, double&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, std::move(const_obj), obj2)
      .template verify<int const&&, double&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, std::move(obj), const_obj2)
      .template verify<int&&, double const&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, std::move(const_obj), const_obj2)
      .template verify<int const&&, double const&>({Qualifiers::CV::None, Qualifiers::Lvalue});

  rsl::visit(visitor, obj, std::move(obj2)).template verify<int&, double&&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, obj, std::move(const_obj2))
      .template verify<int&, double const&&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, const_obj, std::move(obj2))
      .template verify<int const&, double&&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, const_obj, std::move(const_obj2))
      .template verify<int const&, double const&&>({Qualifiers::CV::None, Qualifiers::Lvalue});

  rsl::visit(visitor, std::move(obj), std::move(obj2))
      .template verify<int&&, double&&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, std::move(const_obj), std::move(obj2))
      .template verify<int const&&, double&&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, std::move(obj), std::move(const_obj2))
      .template verify<int&&, double const&&>({Qualifiers::CV::None, Qualifiers::Lvalue});
  rsl::visit(visitor, std::move(const_obj), std::move(const_obj2))
      .template verify<int const&&, double const&&>({Qualifiers::CV::None, Qualifiers::Lvalue});
}

int dummy = 42;
struct ResultTestVisitor {
  int& operator()() & { return dummy; }
  int const& operator()() const& { return dummy; }
  int&& operator()() && { return std::move(dummy); }
  int const&& operator()() const&& { return std::move(std::as_const(dummy)); }
};

TEST(VisitNoVariant, ReturnValueCategory) {
  auto visitor              = ResultTestVisitor{};
  auto const& const_visitor = visitor;
  auto&& result0            = rsl::visit(visitor);
  ASSERT_SAME(decltype(result0), int&);

  auto&& result1 = rsl::visit(const_visitor);
  ASSERT_SAME(decltype(result1), int const&);

  auto&& result2 = rsl::visit(std::move(visitor));
  ASSERT_SAME(decltype(result2), int&&);

  auto&& result3 = rsl::visit(std::move(const_visitor));
  ASSERT_SAME(decltype(result3), int const&&);
}