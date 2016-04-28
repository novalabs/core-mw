#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/Message.hpp>
#include <Core/MW/NamingTraits.hpp>

NAMESPACE_CORE_MW_BEGIN

class Transport;


class MgmtMsg:
	public Message
{
public:
	enum TypeEnum {
//		RAW = 0x00,

		// Module messages
		ALIVE    = 0x11,
		STOP     = 0x12,
		REBOOT   = 0x13,
		BOOTLOAD = 0x14,

		// PubSub messages
		ADVERTISE          = 0x21,
		SUBSCRIBE_REQUEST  = 0x22,
		SUBSCRIBE_RESPONSE = 0x23,

		// Path messages
		PATH = 0x31,
	};

	enum {
		MAX_PAYLOAD_LENGTH = 31
	};

	struct Path {
		char module[NamingTraits < Middleware > ::MAX_LENGTH];
		char node[NamingTraits < Node > ::MAX_LENGTH];
		char topic[NamingTraits < Topic > ::MAX_LENGTH];
	}
	CORE_PACKED;

	struct PubSub {
		enum {
			MAX_RAW_PARAMS_LENGTH = 10
		};

		char     topic[NamingTraits < Topic > ::MAX_LENGTH];
		uint16_t payload_size;
		uint16_t queue_length;
		uint8_t  raw_params[MAX_RAW_PARAMS_LENGTH];
	}
	CORE_PACKED;

	struct Module {
		char    name[NamingTraits < Middleware > ::MAX_LENGTH];
		uint8_t reserved_;
		struct {
			unsigned stopped   : 1;
			unsigned rebooted  : 1;
			unsigned boot_mode : 1;
			unsigned reserved_ : 5;
		}
		CORE_PACKED flags;
	}
	CORE_PACKED;

public:
	union {
		uint8_t payload[MAX_PAYLOAD_LENGTH];
		Path    path;
		PubSub  pubsub;
		Module  module;
	}
	        CORE_PACKED;
	uint8_t type;
}
CORE_PACKED;


NAMESPACE_CORE_MW_END