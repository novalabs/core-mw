/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>

NAMESPACE_CORE_MW_BEGIN


class StaticQueue_:
    private core::Uncopyable
{
public:
    struct Link {
        Link* nextp;
        void* datap;

        Link(
            void* datap
        ) :
            nextp(nullptr),
            datap(datap)
        {}
    };

private:
    Link* headp;
    Link* tailp;

public:
    const Link*
    get_head_unsafe() const;

    const Link*
    get_tail_unsafe() const;

    bool
    is_empty_unsafe() const;

    void
    post_unsafe(
        Link& link
    );

    bool
    peek_unsafe(
        const Link*& linkp
    ) const;

    bool
    fetch_unsafe(
        Link& link
    );

    bool
    skip_unsafe();

    const Link*
    get_head() const;

    const Link*
    get_tail() const;

    bool
    is_empty() const;

    void
    post(
        Link& link
    );

    bool
    peek(
        const Link*& linkp
    ) const;

    bool
    fetch(
        Link& link
    );

    bool
    skip();


public:
    StaticQueue_();
};


inline
bool
StaticQueue_::is_empty_unsafe() const
{
    return headp == nullptr;
}

NAMESPACE_CORE_MW_END
