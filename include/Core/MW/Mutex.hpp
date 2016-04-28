#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/impl/Mutex_.hpp>

NAMESPACE_CORE_MW_BEGIN


class Mutex:
	private Uncopyable
{
private:
	Mutex_ impl;

public:
	void
	initialize();

	void
	acquire_unsafe();

	void
	release_unsafe();

	void
	acquire();

	void
	release();


public:
	Mutex();
	explicit
	Mutex(
			bool initialize
	);
};


inline
void
Mutex::initialize()
{
	impl.initialize();
}

inline
void
Mutex::acquire_unsafe()
{
	impl.acquire_unsafe();
}

inline
void
Mutex::release_unsafe()
{
	impl.release_unsafe();
}

inline
void
Mutex::acquire()
{
	impl.acquire();
}

inline
void
Mutex::release()
{
	impl.release();
}

inline
Mutex::Mutex()
	:
	impl()
{}


inline
Mutex::Mutex(
		bool initialize
)
	:
	impl(initialize)
{}


NAMESPACE_CORE_MW_END