#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

enum class BaseType {
  Integer,
  Boolean,
  Float,
  String,
  Void,
};

struct DeclarationType {
  enum class Kind {
    FlatType,
    Array,
    Function,
  };

  Kind tag;

  BaseType base;

  union {
    std::vector<DeclarationType> argTypes;
    int arraySize;
  };
};

struct SymbolMap {
  std::unique_ptr<SymbolMap> parent = nullptr;
  std::map<std::string, DeclarationType> name_to_type = {};
};
