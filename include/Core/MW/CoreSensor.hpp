#pragma once

#include <Core/MW/namespace.hpp>

NAMESPACE_CORE_MW_BEGIN

template <typename _T>
class CoreSensor
{
public:
	using DataType = _T;

	virtual bool
	init() = 0;

	virtual bool
	start() = 0;

	virtual bool
	stop() = 0;

	virtual bool
	waitUntilReady() = 0;

	virtual bool
	update() = 0;

	virtual void
	get(
			DataType& data
	) = 0;
};

NAMESPACE_CORE_MW_END