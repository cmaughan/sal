#include <sal/buf_ptr.hpp>
#include <sal/common.test.hpp>


namespace {


constexpr size_t size = 4;


template <typename T>
auto *pointer (const sal::buf_ptr &) noexcept //{{{1
{
  static T data_[size] = {};
  return &data_[0];
}


template <typename T>
auto *pointer (const sal::const_buf_ptr &) noexcept //{{{1
{
  static const T data_[size] = {};
  return &data_[0];
}


template <typename T>
auto &array (const sal::buf_ptr &) noexcept //{{{1
{
  static T data_[size] = {};
  return data_;
}


template <typename T>
auto &array (const sal::const_buf_ptr &) noexcept //{{{1
{
  static const T data_[size] = {};
  return data_;
}


template <typename T>
auto &std_array (const sal::buf_ptr &) noexcept //{{{1
{
  static std::array<T, size> data_{};
  return data_;
}


template <typename T>
auto &std_array (const sal::const_buf_ptr &) noexcept //{{{1
{
  static const std::array<T, size> data_{};
  return data_;
}


template <typename T>
auto &std_vector (const sal::buf_ptr &) noexcept //{{{1
{
  static std::vector<T> data_(size);
  return data_;
}


template <typename T>
auto &std_vector (const sal::const_buf_ptr &) noexcept //{{{1
{
  static const std::vector<T> data_(size);
  return data_;
}


auto &string (const sal::buf_ptr &) //{{{1
{
  static std::string data_{"test"};
  return data_;
}


auto &string (const sal::const_buf_ptr &) //{{{1
{
  static const std::string data_{"test"};
  return data_;
}


template <typename BufferType>
using buf_ptr = sal_test::with_type<BufferType>;


using buf_ptr_types = testing::Types<
  sal::buf_ptr,
  sal::const_buf_ptr
>;

TYPED_TEST_CASE(buf_ptr, buf_ptr_types);


TYPED_TEST(buf_ptr, ctor) //{{{1
{
  TypeParam ptr;
  EXPECT_EQ(nullptr, ptr.data());
  EXPECT_EQ(0U, ptr.size());
}


TYPED_TEST(buf_ptr, ctor_pointer) //{{{1
{
  TypeParam ptr(pointer<char>(TypeParam()), size);
  EXPECT_EQ(pointer<char>(TypeParam()), ptr.data());
  EXPECT_EQ(size, ptr.size());
}


TYPED_TEST(buf_ptr, inc) //{{{1
{
  TypeParam ptr(pointer<char>(TypeParam()), size);
  ptr += size / 2;
  EXPECT_EQ(pointer<char>(TypeParam()) + 2, ptr.data());
  EXPECT_EQ(size / 2, ptr.size());
}


TYPED_TEST(buf_ptr, inc_invalid) //{{{1
{
  TypeParam ptr(pointer<char>(TypeParam()), size);
  ptr += size * 2;
  EXPECT_EQ(pointer<char>(TypeParam()) + size, ptr.data());
  EXPECT_EQ(0U, ptr.size());
}


TYPED_TEST(buf_ptr, add_ptr_and_size) //{{{1
{
  TypeParam a(pointer<char>(TypeParam()), size);
  TypeParam b = a + 2;
  EXPECT_EQ(pointer<char>(TypeParam()) + 2, b.data());
  EXPECT_EQ(size / 2, b.size());
}


TYPED_TEST(buf_ptr, add_ptr_and_size_invalid) //{{{1
{
  TypeParam a(pointer<char>(TypeParam()), size);
  TypeParam b = a + 2 * size;
  EXPECT_EQ(pointer<char>(TypeParam()) + size, b.data());
  EXPECT_EQ(0U, b.size());
}


TYPED_TEST(buf_ptr, add_size_and_ptr) //{{{1
{
  TypeParam a(pointer<char>(TypeParam()), size);
  TypeParam b = 2 + a;
  EXPECT_EQ(pointer<char>(TypeParam()) + 2, b.data());
  EXPECT_EQ(size / 2, b.size());
}


TYPED_TEST(buf_ptr, add_size_and_ptr_invalid) //{{{1
{
  TypeParam a(pointer<char>(TypeParam()), size);
  TypeParam b = 2 * size + a;
  EXPECT_EQ(pointer<char>(TypeParam()) + size, b.data());
  EXPECT_EQ(0U, b.size());
}


TYPED_TEST(buf_ptr, from_char_pointer) //{{{1
{
  auto *d = pointer<char>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size, ptr.size());
}


TYPED_TEST(buf_ptr, from_int_pointer) //{{{1
{
  auto *d = pointer<int>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size, ptr.size());
}


TYPED_TEST(buf_ptr, from_ptr) //{{{1
{
  auto &d = array<char>(TypeParam());
  TypeParam a = sal::make_buf(d);
  TypeParam ptr = sal::make_buf(a);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size, ptr.size());
}


TYPED_TEST(buf_ptr, from_ptr_half) //{{{1
{
  auto &d = array<char>(TypeParam());
  TypeParam a = sal::make_buf(d);
  TypeParam ptr = sal::make_buf(a, a.size() / 2);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size / 2, ptr.size());
}


TYPED_TEST(buf_ptr, from_ptr_overflow) //{{{1
{
  auto &d = array<char>(TypeParam());
  TypeParam a = sal::make_buf(d);
  TypeParam ptr = sal::make_buf(a, a.size() * 2);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size, ptr.size());
}


TYPED_TEST(buf_ptr, from_char_array) //{{{1
{
  auto &d = array<char>(TypeParam());
  TypeParam ptr = sal::make_buf(d);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size, ptr.size());
}


TYPED_TEST(buf_ptr, from_char_array_half) //{{{1
{
  auto &d = array<char>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size / 2);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size / 2, ptr.size());
}


TYPED_TEST(buf_ptr, from_char_array_overflow) //{{{1
{
  auto &d = array<char>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size * 1024);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size, ptr.size());
}


TYPED_TEST(buf_ptr, from_int_array) //{{{1
{
  auto &d = array<int>(TypeParam());
  TypeParam ptr = sal::make_buf(d);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size * sizeof(int), ptr.size());
}


TYPED_TEST(buf_ptr, from_int_array_half) //{{{1
{
  auto &d = array<int>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size / 2);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size / 2, ptr.size());
}


TYPED_TEST(buf_ptr, from_int_array_overflow) //{{{1
{
  auto &d = array<int>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size * 1024);
  EXPECT_EQ(d, ptr.data());
  EXPECT_EQ(size * sizeof(int), ptr.size());
}


TYPED_TEST(buf_ptr, from_std_char_array) //{{{1
{
  auto &d = std_array<char>(TypeParam());
  TypeParam ptr = sal::make_buf(d);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(d.size(), ptr.size());
}


TYPED_TEST(buf_ptr, from_std_char_array_half) //{{{1
{
  auto &d = std_array<char>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size / 2);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(size / 2, ptr.size());
}


TYPED_TEST(buf_ptr, from_std_char_array_overflow) //{{{1
{
  auto &d = std_array<char>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size * 1024);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(size, ptr.size());
}


TYPED_TEST(buf_ptr, from_std_int_array) //{{{1
{
  auto &d = std_array<int>(TypeParam());
  TypeParam ptr = sal::make_buf(d);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(d.size() * sizeof(int), ptr.size());
}


TYPED_TEST(buf_ptr, from_std_int_array_half) //{{{1
{
  auto &d = std_array<int>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size / 2);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(size / 2, ptr.size());
}


TYPED_TEST(buf_ptr, from_std_int_array_overflow) //{{{1
{
  auto &d = std_array<int>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size * 1024);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(size * sizeof(int), ptr.size());
}


TYPED_TEST(buf_ptr, from_std_char_vector) //{{{1
{
  auto &d = std_vector<char>(TypeParam());
  TypeParam ptr = sal::make_buf(d);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(d.size(), ptr.size());
}


TYPED_TEST(buf_ptr, from_std_char_vector_half) //{{{1
{
  auto &d = std_vector<char>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size / 2);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(size / 2, ptr.size());
}


TYPED_TEST(buf_ptr, from_std_char_vector_overflow) //{{{1
{
  auto &d = std_vector<char>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size * 1024);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(size, ptr.size());
}


TYPED_TEST(buf_ptr, from_std_int_vector) //{{{1
{
  auto &d = std_vector<int>(TypeParam());
  TypeParam ptr = sal::make_buf(d);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(d.size() * sizeof(int), ptr.size());
}


TYPED_TEST(buf_ptr, from_std_int_vector_half) //{{{1
{
  auto &d = std_vector<int>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size / 2);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(size / 2, ptr.size());
}


TYPED_TEST(buf_ptr, from_std_int_vector_overflow) //{{{1
{
  auto &d = std_vector<int>(TypeParam());
  TypeParam ptr = sal::make_buf(d, size * 1024);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(size * sizeof(int), ptr.size());
}


TYPED_TEST(buf_ptr, from_string) //{{{1
{
  auto &d = string(TypeParam());
  TypeParam ptr = sal::make_buf(d);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(d.size(), ptr.size());
}


TYPED_TEST(buf_ptr, from_string_half) //{{{1
{
  auto &d = string(TypeParam());
  TypeParam ptr = sal::make_buf(d, size / 2);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(size / 2, ptr.size());
}


TYPED_TEST(buf_ptr, from_string_overflow) //{{{1
{
  auto &d = string(TypeParam());
  TypeParam ptr = sal::make_buf(d, size * 1024);
  EXPECT_EQ(d.data(), ptr.data());
  EXPECT_EQ(d.size(), ptr.size());
}


} // namespace
