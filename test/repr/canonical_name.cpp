#include <string>
#include <gtest/gtest.h>
#include <common/assertions.h>

#include <string_view>
#include <vector>
#include <unordered_map>

#include <rsl/repr>

namespace {
namespace TestNS {
struct HasADL {};
consteval auto canonical_name(std::type_identity<HasADL>) {
  return "foo";
}

struct HasFriend {
  friend consteval auto canonical_name(std::type_identity<HasFriend>) { return "foo"; }
};

struct[[= rsl::canonical_name("foo")]] HasAnnotation {};

struct HasSpecialization {};
struct HasMemberConstant {
  static constexpr auto canonical_name = "foo";
};
  struct HasMemberFunction {
    static consteval auto canonical_name() { return "foo"; }
  };
}  // namespace TestNS


}  // namespace

template <>
constexpr inline auto rsl::canonical_name_of<TestNS::HasSpecialization> = "foo";


TEST(CanonicalName, Basic) {
  ASSERT_STREQ(rsl::canonical_name_of<int>, "int");
  ASSERT_STREQ(rsl::canonical_name_of<int const>, "int const");
  ASSERT_STREQ(rsl::canonical_name_of<int const&>, "int const&");
  ASSERT_STREQ(rsl::canonical_name_of<int&>, "int&");
  ASSERT_STREQ(rsl::canonical_name_of<int&&>, "int&&");
  ASSERT_STREQ(rsl::canonical_name_of<int*>, "int*");
  ASSERT_STREQ(rsl::canonical_name_of<int const*>, "int const*");
  ASSERT_STREQ(rsl::canonical_name_of<int const* const>, "int const* const");
  ASSERT_STREQ(rsl::canonical_name_of<int* const>, "int* const");
}

TEST(CanonicalName, StdAliases) {
  ASSERT_STREQ(rsl::canonical_name_of<std::string>, "string");
  ASSERT_STREQ(rsl::canonical_name_of<std::string_view>, "string_view");
}

TEST(CanonicalName, TemplateDefaults) {
  ASSERT_STREQ(rsl::canonical_name_of<std::vector<std::string>>, "vector<string>");
  ASSERT_STREQ(
      (rsl::canonical_name_of<std::unordered_map<std::string_view, std::vector<std::string>>>),
      "unordered_map<string_view, vector<string>>");

  ASSERT_STREQ(rsl::canonical_name_of<std::vector<std::string> const>, "vector<string> const");
  ASSERT_STREQ(rsl::canonical_name_of<std::vector<std::string> const&>, "vector<string> const&");
  ASSERT_STREQ(rsl::canonical_name_of<std::vector<std::string> const&&>, "vector<string> const&&");
  ASSERT_STREQ(rsl::canonical_name_of<std::vector<std::string>&>, "vector<string>&");
  ASSERT_STREQ(rsl::canonical_name_of<std::vector<std::string>&&>, "vector<string>&&");
  ASSERT_STREQ(rsl::canonical_name_of<std::vector<std::string>*>, "vector<string>*");
}

TEST(CanonicalName, Annotation) {
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasAnnotation>, "foo");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasAnnotation const>, "foo const");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasAnnotation const&>, "foo const&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasAnnotation const&&>, "foo const&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasAnnotation&>, "foo&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasAnnotation&&>, "foo&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasAnnotation*>, "foo*");
}

TEST(CanonicalName, MemberConstant) {
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberConstant>, "foo");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberConstant const>, "foo const");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberConstant const&>, "foo const&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberConstant const&&>, "foo const&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberConstant&>, "foo&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberConstant&&>, "foo&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberConstant*>, "foo*");
}

TEST(CanonicalName, MemberFunction) {

  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberFunction>, "foo");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberFunction const>, "foo const");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberFunction const&>, "foo const&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberFunction const&&>, "foo const&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberFunction&>, "foo&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberFunction&&>, "foo&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasMemberFunction*>, "foo*");
}

TEST(CanonicalName, ADL) {
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasADL>, "foo");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasADL const>, "foo const");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasADL const&>, "foo const&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasADL const&&>, "foo const&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasADL&>, "foo&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasADL&&>, "foo&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasADL*>, "foo*");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasFriend>, "foo");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasFriend const>, "foo const");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasFriend const&>, "foo const&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasFriend const&&>, "foo const&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasFriend&>, "foo&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasFriend&&>, "foo&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasFriend*>, "foo*");
}

TEST(CanonicalName, Specialization) {
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasSpecialization>, "foo");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasSpecialization const>, "foo const");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasSpecialization const&>, "foo const&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasSpecialization const&&>, "foo const&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasSpecialization&>, "foo&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasSpecialization&&>, "foo&&");
  ASSERT_STREQ(rsl::canonical_name_of<TestNS::HasSpecialization*>, "foo*");
}

// TODO test precedence

TEST(QualifiedName, Basic) {
  ASSERT_STREQ(rsl::qualified_name_of<int>, "int");
}

TEST(QualifiedName, StdAliases) {
  ASSERT_STREQ(rsl::qualified_name_of<std::string>, "std::string");
}

TEST(QualifiedName, TemplateDefaults) {
  ASSERT_STREQ(rsl::qualified_name_of<std::vector<std::string>>, "std::vector<std::string>");
  ASSERT_STREQ(
      (rsl::qualified_name_of<std::unordered_map<std::string_view, std::vector<std::string>>>),
      "std::unordered_map<std::string_view, std::vector<std::string>>");
}

TEST(QualifiedName, Annotation) {
  ASSERT_STREQ(rsl::qualified_name_of<TestNS::HasAnnotation>, "TestNS::foo");
  ASSERT_STREQ(rsl::fully_qualified_name_of<TestNS::HasAnnotation>, "::TestNS::foo");
}

TEST(QualifiedName, MemberConstant) {
  ASSERT_STREQ(rsl::qualified_name_of<TestNS::HasMemberConstant>, "TestNS::foo");
  ASSERT_STREQ(rsl::fully_qualified_name_of<TestNS::HasMemberConstant>, "::TestNS::foo");
}

TEST(QualifiedName, MemberFunction) {
  ASSERT_STREQ(rsl::qualified_name_of<TestNS::HasMemberFunction>, "TestNS::foo");
  ASSERT_STREQ(rsl::fully_qualified_name_of<TestNS::HasMemberFunction>, "::TestNS::foo");
  
}

TEST(QualifiedName, ADL) {
  ASSERT_STREQ(rsl::qualified_name_of<TestNS::HasADL>, "TestNS::foo");
  ASSERT_STREQ(rsl::qualified_name_of<TestNS::HasFriend>, "TestNS::foo");
  ASSERT_STREQ(rsl::fully_qualified_name_of<TestNS::HasADL>, "::TestNS::foo");
  ASSERT_STREQ(rsl::fully_qualified_name_of<TestNS::HasFriend>, "::TestNS::foo");
  
}

TEST(QualifiedName, Specialization) {
  ASSERT_STREQ(rsl::qualified_name_of<TestNS::HasSpecialization>, "TestNS::foo");
  ASSERT_STREQ(rsl::fully_qualified_name_of<TestNS::HasSpecialization>, "::TestNS::foo");
}