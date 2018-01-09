#pragma once
#include "types.hpp"
#include <string>
#include <unordered_map>

namespace typ
{

// A lexically-scoped assignment of types to identifiers
class TypeEnvironment
{
public:
    TypeEnvironment();

    // Searches all scopes - returns nullptr if not found
    Type* lookup(const std::string& ident);

    // Does not check that the identifier is undefined in the current scope
    void insert(const std::string& ident, Type* type);

    // Is this identifier already defined in the current scope?
    bool checkScope(const std::string& ident);

    void enterScope();
    void exitScope();

private:
    std::vector<std::unordered_map<std::string, Type*>> _scopes;
};

} // namespace typ
