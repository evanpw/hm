#include "type_env.hpp"

namespace typ
{

TypeEnvironment::TypeEnvironment()
{
    enterScope();
}

Type* TypeEnvironment::lookup(const std::string& ident)
{
    for (auto itr = _scopes.rbegin(); itr != _scopes.rend(); ++itr)
    {
        auto& scope = *itr;
        auto i = scope.find(ident);
        if (i != scope.end())
        {
            return i->second;
        }
    }

    return nullptr;
}

bool TypeEnvironment::checkScope(const std::string& ident)
{
    return _scopes.back().count(ident) != 0;
}

void TypeEnvironment::insert(const std::string& ident, Type* type)
{
    _scopes.back()[ident] = type;
}

void TypeEnvironment::enterScope()
{
    _scopes.push_back({});
}

void TypeEnvironment::exitScope()
{
    _scopes.pop_back();
}

} // namespace typ
