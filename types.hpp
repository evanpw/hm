#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace typ {

enum Tag { kConstant, kArrow, kVar };

// Generic base class for all types
class Type
{
public:
    virtual Tag tag() const = 0;

    // Returns this except in the case of a linked type variable, in which case
    // it returns the root of the chain
    virtual Type* root() { return this; }
};

class Constant;
class Arrow;
class Var;

// Determines if the type variable lhs appears anywhere in the type rhs
// Also adjusts the level of unbound type variables in rhs to prepare for binding lhs to rhs
bool occurs(Var* lhs, int level, Type* rhs);

// Assign a value to a type variable
void bind(Var* lhs, Type* rhs);

// Replace all unbound type variables in type, with level > the given one with generic type vars
Type* generalize(Type* type, int level);

// Replace all generic type variables with unbound variables with the given level
Type* instantiate(Type* type, int level);

// Find an assignment of type variables that makes lhs and rhs equal
// Returns nullptr if none exists (variables may have already been assigned)
Type* unify(Type* lhs, Type* rhs);

std::ostream& operator<<(std::ostream& out, Type* type);

// Type constant: Int, Bool, ...
class Constant : public Type
{
public:
    Constant(const std::string& name)
    : name(name)
    {}

    virtual Tag tag() const { return kConstant; }

    std::string name;
};

// Function type: |Int, Bool| -> String
class Arrow : public Type
{
public:
    Arrow(const std::vector<Type*>& inputs, Type* output)
    : inputs(inputs), output(output)
    {}

    virtual Tag tag() const { return kArrow; }

    std::vector<Type*> inputs;
    Type* output;
};

// Type variable (may be generic or not; if not, may be linked / assigned to another type)
class Var : public Type
{
public:
    static Var* makeUnbound(int level)
    {
        Var* var = new Var;
        var->level = level;
        var->index = s_nextIndex++;
        return var;
    }

    static Var* makeGeneric(int index)
    {
        Var* var = new Var;
        var->level = -1;
        var->index = index;
        return var;
    }

    virtual Type* root();

    Type* link = nullptr;
    int index; // uniquely identifies this variable
    int level; // only for unbound variables (-1 means generic)

    virtual Tag tag() const { return kVar; }
    bool isGeneric() const { return level == -1; }

private:
    Var() {}
    static int s_nextIndex;
};

} // namespace typ
