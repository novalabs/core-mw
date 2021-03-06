/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/mw/impl/StaticQueue_.hpp>

NAMESPACE_CORE_MW_BEGIN


// TODO: ConstIterator
template <typename T>
class StaticQueue:
    private core::Uncopyable
{
public:
    struct Link {
        Link*    nextp;
        T* const itemp;

        Link() : nextp(nullptr), itemp(nullptr) {}

        Link(
            T& item
        ) : nextp(nullptr), itemp(&item) {}
    };

    class IteratorUnsafe
    {
    private:
        const Link* curp;

    public:
        IteratorUnsafe(
            const IteratorUnsafe& rhs
        ) : curp(rhs.curp) {}

    private:
        IteratorUnsafe(
            const Link* beginp
        ) : curp(beginp) {}

        friend class StaticQueue<T>;

    private:
        IteratorUnsafe&
        operator=(
            const IteratorUnsafe& rhs
        );


    public:
        const Link*
        operator->() const
        {
            return curp;
        }

        const Link&
        operator*() const
        {
            return *curp;
        }

        IteratorUnsafe&
        operator++()
        {
            curp = curp->nextp;
            return *this;
        }

        const IteratorUnsafe
        operator++(
            int
        )
        {
            IteratorUnsafe old(*this);

            curp = curp->nextp;
            return old;
        }

        bool
        operator==(
            const IteratorUnsafe& rhs
        ) const
        {
            return this->curp == rhs.curp;
        }

        bool
        operator!=(
            const IteratorUnsafe& rhs
        ) const
        {
            return this->curp != rhs.curp;
        }
    };

    class Iterator
    {
    private:
        const Link* curp;

    public:
        Iterator(
            const Iterator& rhs
        ) : curp(rhs.curp) {}

    private:
        Iterator(
            const Link* beginp
        ) : curp(beginp) {}

        friend class StaticQueue<T>;

    private:
        Iterator&
        operator=(
            const Iterator& rhs
        );


    public:
        const Link*
        operator->() const
        {
            return curp;
        }

        const Link&
        operator*() const
        {
            return *curp;
        }

        Iterator&
        operator++()
        {
            core::os::SysLock::acquire();

            curp = curp->nextp;
            core::os::SysLock::release();
            return *this;
        }

        const Iterator
        operator++(
            int
        )
        {
            core::os::SysLock::acquire();
            Iterator old(*this);

            curp = curp->nextp;
            core::os::SysLock::release();
            return old;
        }

        bool
        operator==(
            const Iterator& rhs
        ) const
        {
            return this->curp == rhs.curp;
        }

        bool
        operator!=(
            const Iterator& rhs
        ) const
        {
            return this->curp != rhs.curp;
        }
    };

private:
    typedef StaticQueue_::Link Entry_impl;

private:
    StaticQueue_ impl;

public:
    bool
    is_empty_unsafe() const
    {
        return impl.is_empty_unsafe();
    }

    void
    post_unsafe(
        Link& link
    )
    {
        impl.post_unsafe(reinterpret_cast<Entry_impl&>(link));
    }

    bool
    peek_unsafe(
        const Link*& linkp
    ) const
    {
        return impl.peek_unsafe(
            reinterpret_cast<const Entry_impl*&>(linkp)
        );
    }

    bool
    fetch_unsafe(
        Link& link
    )
    {
        return impl.fetch_unsafe(reinterpret_cast<Entry_impl&>(link));
    }

    bool
    skip_unsafe()
    {
        return impl.skip_unsafe();
    }

    const IteratorUnsafe
    begin_unsafe() const
    {
        return IteratorUnsafe(reinterpret_cast<const Link*>(
                                  impl.get_head_unsafe()
                              ));
    }

    const IteratorUnsafe
    end_unsafe() const
    {
        return IteratorUnsafe(nullptr);
    }

    bool
    is_empty() const
    {
        return impl.is_empty();
    }

    void
    post(
        Link& link
    )
    {
        impl.post(reinterpret_cast<Entry_impl&>(link));
    }

    bool
    peek(
        const Link*& linkp
    ) const
    {
        return impl.peek(reinterpret_cast<const Entry_impl*&>(linkp));
    }

    bool
    fetch(
        Link& link
    )
    {
        return impl.fetch(reinterpret_cast<Entry_impl&>(link));
    }

    bool
    skip()
    {
        return impl.skip();
    }

    const Iterator
    begin() const
    {
        return Iterator(reinterpret_cast<const Link*>(impl.get_head()));
    }

    const Iterator
    end() const
    {
        return Iterator(nullptr);
    }
};


NAMESPACE_CORE_MW_END
