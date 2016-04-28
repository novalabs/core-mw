#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/SubscriberExtBuf.hpp>

NAMESPACE_CORE_MW_BEGIN

class Time;
class Message;
class Node;


template <typename MessageType, unsigned QUEUE_LENGTH>
class Subscriber:
	public SubscriberExtBuf<MessageType>
{
	friend class Node;

public:
	typedef typename SubscriberExtBuf<MessageType>::Callback Callback;

private:
	MessageType  msgpool_buf[QUEUE_LENGTH];
	MessageType* queue_buf[QUEUE_LENGTH];

public:
	Subscriber(
			Callback callback = NULL
	);
	~Subscriber();
};


template <typename MT, unsigned QL>
inline
Subscriber<MT, QL>::Subscriber(
		Callback callback
)
	:
	SubscriberExtBuf<MT>(queue_buf, QL, callback)
{}


template <typename MT, unsigned QL>
inline
Subscriber<MT, QL>::~Subscriber() {}


NAMESPACE_CORE_MW_END