#include <cassert>
#include <cstdint>

class Type {
public:
  enum class TypeKind : uint8_t {
    Void = 0,
    Integer,
    Float,
    String,
  };

private:
  TypeKind kind;
};

class Type1 {
public:
  enum TypeKind : uint8_t {
    Void = 0,
    Integer,
    Float,
    String,
  };

private:
  TypeKind kind;
};

static_assert(sizeof(Type) == sizeof(Type1), "same size types");