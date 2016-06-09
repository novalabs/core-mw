/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/Message.hpp>
#include <Core/MW/NamingTraits.hpp>

NAMESPACE_CORE_MW_BEGIN

class BootMsg:
   public Message
{
public:
   uint8_t command;
   uint8_t seqId;
   uint8_t payload[48 - 2];
}

CORE_PACKED;

NAMESPACE_CORE_MW_END
