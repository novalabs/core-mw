#include <Core/MW/namespace.hpp>
#include <Core/MW/Message.hpp>
#include <cstring>

NAMESPACE_CORE_MW_BEGIN


void
Message::acquire()
{
	SysLock::acquire();

	acquire_unsafe();
	SysLock::release();
}

bool
Message::release()
{
	SysLock::acquire();
	bool floating = release_unsafe();
	SysLock::release();

	return floating;
}

void
Message::reset()
{
	SysLock::acquire();

	reset_unsafe();
	SysLock::release();
}

void
Message::copy(
		Message&       to,
		const Message& from,
		size_t         type_size
)
{
	CORE_ASSERT(type_size >= sizeof(RefcountType));

	memcpy( // TODO: just copy the payload with payload_size instead of type_size
			&to.refcount + 1, &from.refcount + 1,
#if CORE_USE_BRIDGE_MODE
			type_size - (sizeof(Transport*) + sizeof(RefcountType))
#else
			type_size - sizeof(RefcountType)
#endif
	);
}

NAMESPACE_CORE_MW_END