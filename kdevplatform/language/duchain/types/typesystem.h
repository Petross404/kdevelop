/* This file is part of KDevelop
    Copyright 2006 Roberto Raggi <roberto@kdevelop.org>
    Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
    Copyright 2007-2008 David Nolden <david.nolden.kdevelop@art-master.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#ifndef KDEVPLATFORM_TYPESYSTEM_H
#define KDEVPLATFORM_TYPESYSTEM_H

#include "typepointer.h"
#include "../identifier.h"
#include "abstracttype.h"

namespace KDevelop {
class AbstractTypeDataRequest;

class AbstractType;
class IntegralType;
class PointerType;
class ReferenceType;
class FunctionType;
class StructureType;
class ArrayType;

class TypeExchanger;

class KDEVPLATFORMLANGUAGE_EXPORT TypeVisitor
{
public:
    virtual ~TypeVisitor ();

    virtual bool preVisit (const AbstractType*) = 0;
    virtual void postVisit (const AbstractType*) = 0;

    ///Return whether sub-types should be visited(same for the other visit functions)
    virtual bool visit(const AbstractType*) = 0;

    virtual void visit (const IntegralType*) = 0;

    virtual bool visit (const PointerType*) = 0;
    virtual void endVisit (const PointerType*) = 0;

    virtual bool visit (const ReferenceType*) = 0;
    virtual void endVisit (const ReferenceType*) = 0;

    virtual bool visit (const FunctionType*) = 0;
    virtual void endVisit (const FunctionType*) = 0;

    virtual bool visit (const StructureType*) = 0;
    virtual void endVisit (const StructureType*) = 0;

    virtual bool visit (const ArrayType*) = 0;
    virtual void endVisit (const ArrayType*) = 0;
};

class KDEVPLATFORMLANGUAGE_EXPORT SimpleTypeVisitor
    : public TypeVisitor
{
public:
    /// When using SimpleTypeVisitor, the visit taking an AbstractType is the only function
    /// you must override to collect all types.
    using TypeVisitor::visit;

    bool preVisit (const AbstractType*) override;
    void postVisit (const AbstractType*) override;

    void visit (const IntegralType*) override;

    bool visit (const PointerType*) override;
    void endVisit (const PointerType*) override;

    bool visit (const ReferenceType*) override;
    void endVisit (const ReferenceType*) override;

    bool visit (const FunctionType*) override;
    void endVisit (const FunctionType*) override;

    bool visit (const StructureType*) override;
    void endVisit (const StructureType*) override;

    bool visit (const ArrayType*) override;
    void endVisit (const ArrayType*) override;
};

/**
 * A class that can be used to walk through all types that are references from one type, and exchange them with other types.
 * Examples for such types: Base-classes of a class, function-argument types of a function, etc.
 * */
class KDEVPLATFORMLANGUAGE_EXPORT TypeExchanger
{
public:
    virtual ~TypeExchanger()
    {
    }

    /**
     * By default should return the given type, and can return another type that the given should be replaced with.
     * Types should allow replacing all their held types using this from within their exchangeTypes function.
     * The default-implementation recurses the exchange, so should be called from within the derived implementation if that is wished.
     * */
    virtual AbstractType::Ptr exchange(const AbstractType::Ptr&);
};

///A simple type-exchanger that replaces one type with another
class KDEVPLATFORMLANGUAGE_EXPORT SimpleTypeExchanger
    : public TypeExchanger
{
public:
    SimpleTypeExchanger(const AbstractType::Ptr& replace, const AbstractType::Ptr& replaceWith);
    AbstractType::Ptr exchange(const AbstractType::Ptr&) override;

private:
    AbstractType::Ptr m_replace, m_replaceWith;
};
}

#endif // KDEVPLATFORM_TYPESYSTEM_H
