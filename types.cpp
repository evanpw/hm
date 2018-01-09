#include "types.hpp"
#include <cassert>
#include <stdexcept>

// References:
// 1. https://github.com/tomprimozic/type-systems/tree/master/algorithm_w
// 2. http://okmij.org/ftp/ML/generalization.html

namespace typ
{

int Var::s_nextIndex = 0;

bool occurs(Var* lhs, int level, Type* rhs)
{
    rhs = rhs->root();

    switch (rhs->tag())
    {
        // Type constants contain no type variables
        case kConstant:
            return false;

        // Search recursively for arrow types
        case kArrow:
        {
            Arrow* arrow = dynamic_cast<Arrow*>(rhs);

            if (occurs(lhs, level, arrow->output))
                return true;

            for (auto* input : arrow->inputs)
            {
                if (occurs(lhs, level, input))
                    return true;
            }

            return false;
        }

        case kVar:
        {
            Var* var = dynamic_cast<Var*>(rhs);
            assert(!var->link);
            assert(!var->isGeneric());

            // Found a match
            if (var->index == lhs->index)
            {
                return true;
            }

            // We're going to bind lhs to var, so var's level moves up to lhs's
            var->level = std::min(level, var->level);

            return false;
        }

        default:
            assert(false);
    }
}

void bind(Var* lhs, Type* rhs)
{
    rhs = rhs->root();
    assert(!lhs->link && !lhs->isGeneric());

    // If the variable lhs appeared in the rhs, then the assignment would yield
    // an infinite, recursive type
    if (occurs(lhs, lhs->level, rhs))
    {
        throw std::runtime_error("infinite type");
    }

    lhs->link = rhs;
}

Type* generalize(Type* type, int level)
{
    type = type->root();

    switch (type->tag())
    {
        // Type constants contain no variables
        case kConstant:
            return type;

        // Generalize recursively for arrow types
        case kArrow:
        {
            Arrow* arrow = dynamic_cast<Arrow*>(type);

            Type* output = generalize(arrow->output, level);

            std::vector<Type*> inputs;
            for (auto* input : arrow->inputs)
            {
                inputs.push_back(generalize(input, level));
            }

            return new Arrow(inputs, output);
        }

        case kVar:
        {
            Var* var = dynamic_cast<Var*>(type);
            assert(!var->link);

            // Generic type variables are unchanged
            if (var->isGeneric())
            {
                return var;
            }

            // Unbound variables are generalized only if they are from a deeper level
            if (var->level > level)
            {
                return Var::makeGeneric(var->index);
            }
            else
            {
                return var;
            }
        }

        default:
            assert(false);
    }
}

Type* instantiate(Type* type, int level, std::unordered_map<int, Type*>& replaced)
{
    type = type->root();

    switch (type->tag())
    {
        // Type constants contain no variables
        case kConstant:
            return type;

        // Generalize recursively for arrow types
        case kArrow:
        {
            Arrow* arrow = dynamic_cast<Arrow*>(type);

            Type* output = instantiate(arrow->output, level, replaced);

            std::vector<Type*> inputs;
            for (auto* input : arrow->inputs)
            {
                inputs.push_back(instantiate(input, level, replaced));
            }

            return new Arrow(inputs, output);
        }

        case kVar:
        {
            Var* var = dynamic_cast<Var*>(type);
            assert(!var->link);

            // Replace linked variables with their value and continue instantiation
            if (var->link)
            {
                return instantiate(var->link, level, replaced);
            }

            // Unbound variables are unchanged
            if (!var->isGeneric())
            {
                return var;
            }

            // Generic variables are replaced with fresh unbound ones
            auto i = replaced.find(var->index);
            if (i == replaced.end())
            {
                i = replaced.emplace(var->index, Var::makeUnbound(level)).first;
            }

            return i->second;
        }

        default:
            assert(false);
    }
}

Type* instantiate(Type* type, int level)
{
    std::unordered_map<int, Type*> replaced;
    return instantiate(type, level, replaced);
}

Type* unify(Type* lhs, Type* rhs)
{
    lhs = lhs->root();
    rhs = rhs->root();

    // Type constants: name must be equal
    if (lhs->tag() == kConstant && rhs->tag() == kConstant)
    {
        Constant* lhsConst = dynamic_cast<Constant*>(lhs);
        Constant* rhsConst = dynamic_cast<Constant*>(rhs);

        if (lhsConst->name != rhsConst->name)
        {
            return nullptr;
        }

        return lhs;
    }
    // Arrow types: must unify recursively
    else if (lhs->tag() == kArrow && rhs->tag() == kArrow)
    {
        Arrow* lhsArrow = dynamic_cast<Arrow*>(lhs);
        Arrow* rhsArrow = dynamic_cast<Arrow*>(rhs);

        Type* output = unify(lhsArrow->output, rhsArrow->output);

        if (!output || lhsArrow->inputs.size() != rhsArrow->inputs.size())
        {
            return nullptr;
        }

        std::vector<Type*> inputs;
        for (size_t i = 0; i < lhsArrow->inputs.size(); ++i)
        {
            Type* input = unify(lhsArrow->inputs[i], rhsArrow->inputs[i]);
            if (!input)
            {
                return nullptr;
            }

            inputs.push_back(input);
        }

        return new Arrow(inputs, output);
    }

    if (lhs->tag() == kVar)
    {
        Var* lhsVar = dynamic_cast<Var*>(lhs);
        assert(!lhsVar->link);

        // Unifying an unbound variable binds the variable to the other type
        if (!lhsVar->isGeneric())
        {
            bind(lhsVar, rhs);
            return rhs;
        }
    }

    if (rhs->tag() == kVar)
    {
        Var* rhsVar = dynamic_cast<Var*>(rhs);
        assert(!rhsVar->link);

        // Unifying an unbound variable binds the variable to the other type
        if (!rhsVar->isGeneric())
        {
            bind(rhsVar, lhs);
            return lhs;
        }
    }

    return nullptr;
}

void print(std::ostream& out, Type* type, std::unordered_map<int, char>& varNames)
{
    type = type->root();

    switch (type->tag())
    {
        case kConstant:
        {
            Constant* constant = dynamic_cast<Constant*>(type);
            out << constant->name;

            break;
        }

        // Search recursively for arrow types
        case kArrow:
        {
            Arrow* arrow = dynamic_cast<Arrow*>(type);

            bool brackets = (arrow->inputs.size() != 1) || arrow->inputs[0]->root()->tag() == kArrow;
            if (brackets) out << "|";
            for (size_t i = 0; i < arrow->inputs.size(); ++i)
            {
                if (i != 0) out << ", ";
                print(out, arrow->inputs[i], varNames);
            }
            if (brackets) out << "|";

            out << " -> ";

            bool parens = arrow->output->root()->tag() == kArrow;
            if (parens) out << "(";
            print(out, arrow->output, varNames);
            if (parens) out << ")";

            break;
        }

        case kVar:
        {
            Var* var = dynamic_cast<Var*>(type);
            assert(!var->link);

            // Refer to type variables by sequential lowercase characters as encountered
            auto i = varNames.find(var->index);
            if (i == varNames.end())
            {
                i = varNames.emplace(var->index, char('a' + varNames.size())).first;
            }

            out << i->second;

            break;
        }

        default:
            assert(false);
    }
}

std::ostream& operator<<(std::ostream& out, Type* type)
{
    std::unordered_map<int, char> varNames;
    print(out, type, varNames);
    return out;
}

Type* Var::root()
{
    if (!link)
        return this;

    // Find the root of the chain
    Var* t = this;
    while (t->link)
    {
        if (t->link->tag() == kVar)
        {
            t = dynamic_cast<Var*>(t->link);
        }
        else
        {
            break;
        }
    }

    Type* root = t->link ? t->link : t;

    // Path compression: point every link in the chain directly to the root
    // (improves time complexity of some algorithms)
    t = this;
    while (t->link)
    {
        if (t->link->tag() == kVar)
        {
            Var* next = dynamic_cast<Var*>(t->link);
            t->link = root;
            t = next;
        }
        else
        {
            break;
        }
    }

    return root;
}

} // namespace typ
