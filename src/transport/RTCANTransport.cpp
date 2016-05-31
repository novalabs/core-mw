/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <Core/MW/namespace.hpp>
#include <Core/MW/transport/RTCANTransport.hpp>
#include <Core/MW/transport/RTCANPublisher.hpp>
#include <Core/MW/transport/RTCANSubscriber.hpp>
#include <Core/MW/Middleware.hpp>

#include <rtcan.h>

//FIXME needed for header pool, should use abstract allocator
#include <ch.h>
#include <hal.h>

NAMESPACE_CORE_MW_BEGIN

bool
RTCANTransport::send(
   Message*         msgp,
   RTCANSubscriber* rsubp
)
{
   rtcan_msg_t* rtcan_msg_p;

#if CORE_USE_BRIDGE_MODE
   CORE_ASSERT(msgp->get_source() != this);
#endif

   // Allocate RTCAN msg header
   rtcan_msg_p = header_pool.alloc_unsafe();

   if (rtcan_msg_p == NULL) {
      return false;
   }

   rtcan_msg_p->id       = rsubp->rtcan_id;
   rtcan_msg_p->callback = reinterpret_cast<rtcan_msgcallback_t>(send_cb);
   rtcan_msg_p->params   = rsubp;
   rtcan_msg_p->size     = rsubp->get_topic()->get_payload_size();
   rtcan_msg_p->data     = msgp->get_raw_data();
   rtcan_msg_p->status   = RTCAN_MSG_READY;

   rtcanTransmitI(&rtcan, rtcan_msg_p, 50);
   return true;
} // RTCANTransport::send

void
RTCANTransport::send_cb(
   rtcan_msg_t& rtcan_msg
)
{
   RTCANSubscriber* rsubp     = reinterpret_cast<RTCANSubscriber*>(rtcan_msg.params);
   RTCANTransport*  transport = reinterpret_cast<RTCANTransport*>(rsubp->get_transport());
   Message&         msg       = const_cast<Message&>(Message::get_msg_from_raw_data(rtcan_msg.data));

   rsubp->release_unsafe(msg);
   transport->header_pool.free_unsafe(&rtcan_msg);
}

void
RTCANTransport::recv_cb(
   rtcan_msg_t& rtcan_msg
)
{
   RTCANPublisher* pubp = static_cast<RTCANPublisher*>(rtcan_msg.params);
   Message*        msgp = const_cast<Message*>(&Message::get_msg_from_raw_data(rtcan_msg.data));

   if (rtcan_msg.status == RTCAN_MSG_BUSY) {
#if CORE_USE_BRIDGE_MODE
      CORE_ASSERT(pubp->get_transport() != NULL);
      {
         MessageGuardUnsafe guard(*msgp, *pubp->get_topic());
         msgp->set_source(pubp->get_transport());
         pubp->publish_locally_unsafe(*msgp);

         if (pubp->get_topic()->is_forwarding()) {
            pubp->publish_remotely_unsafe(*msgp);
         }
      }
#else
      pubp->publish_locally_unsafe(*msgp);
#endif
   }

   // allocate again for next message from RTCAN
   if (pubp->alloc_unsafe(msgp)) {
      rtcan_msg.data   = msgp->get_raw_data();
      rtcan_msg.status = RTCAN_MSG_READY;
   } else {
      rtcan_msg.status = RTCAN_MSG_ERROR;
//		CORE_ASSERT(false);
   }
} // RTCANTransport::recv_cb

void
RTCANTransport::free_header(
   rtcan_msg_t& rtcan_msg
)
{
   RTCANTransport* transport = (RTCANTransport*)rtcan_msg.params;

   transport->header_pool.free_unsafe(&rtcan_msg);
}

RemotePublisher*
RTCANTransport::create_publisher(
   Topic&        topic,
   const uint8_t raw_params[]
) const
{
   RTCANPublisher* rpubp      = new RTCANPublisher(*const_cast<RTCANTransport*>(this));
   Topic&          mgmt_topic = Middleware::instance.get_mgmt_topic();
   Message*        msgp;
   bool success;

   CORE_ASSERT(rpubp != NULL);

   success = topic.alloc(msgp);
   CORE_ASSERT(success);

   rtcan_msg_t* rtcan_headerp = &(rpubp->rtcan_header);
   rtcan_headerp->id       = *(rtcan_id_t*)raw_params;
   rtcan_headerp->callback = reinterpret_cast<rtcan_msgcallback_t>(recv_cb);
   rtcan_headerp->params   = rpubp;
   rtcan_headerp->size     = topic.get_payload_size();
   rtcan_headerp->data     = msgp->get_raw_data();

   rtcan_headerp->status = RTCAN_MSG_READY;

   if (&topic == &mgmt_topic) {
      rtcanReceiveMask(&RTCAND1, rtcan_headerp, 0xFF00);
   } else {
      rtcanReceiveMask(&RTCAND1, rtcan_headerp, 0xFFFF);
   }

   return rpubp;
} // RTCANTransport::create_publisher

RemoteSubscriber*
RTCANTransport::create_subscriber(
   Topic&                        topic,
   TimestampedMsgPtrQueue::Entry queue_buf[],
   size_t                        queue_length
) const
{
   RTCANSubscriber* rsubp = new RTCANSubscriber(*const_cast<RTCANTransport*>(this), queue_buf, queue_length);

   // TODO: dynamic ID arbitration

   rsubp->rtcan_id = topic_id(topic);

   return rsubp;
}

void
RTCANTransport::fill_raw_params(
   const Topic& topic,
   uint8_t      raw_params[]
)
{
   *reinterpret_cast<rtcan_id_t*>(raw_params) = topic_id(topic);
}

void
RTCANTransport::initialize(
   const RTCANConfig& rtcan_config
)
{
   Topic&     mgmt_topic = Middleware::instance.get_mgmt_topic();
   rtcan_id_t rtcan_id   = topic_id(mgmt_topic);

   rtcanInit();
   rtcanStart(&rtcan, &rtcan_config);

   mgmt_rsub = reinterpret_cast<RTCANSubscriber*>(create_subscriber(mgmt_topic, mgmt_msgqueue_buf, MGMT_BUFFER_LENGTH));
   subscribe(*mgmt_rsub, "R2P", mgmt_msgbuf, MGMT_BUFFER_LENGTH, sizeof(MgmtMsg));

   mgmt_rpub = reinterpret_cast<RTCANPublisher*>(create_publisher(mgmt_topic, reinterpret_cast<const uint8_t*>(&rtcan_id)));
   advertise(*mgmt_rpub, "R2P", Time::INFINITE, sizeof(MgmtMsg));

   Middleware::instance.add(*this);
}

RTCANTransport::RTCANTransport(
   RTCANDriver& rtcan
) :
   Transport("rtcan"), rtcan(rtcan), header_pool(header_buffer, 10), mgmt_rsub(NULL), mgmt_rpub(NULL) {}

RTCANTransport::~RTCANTransport() {}

rtcan_id_t
RTCANTransport::topic_id(
   const Topic& topic
) const
{
   const StaticList<Topic>& topic_list = Middleware::instance.get_topics();
   Topic&     mgmt_topic = Middleware::instance.get_mgmt_topic();
   int        index      = topic_list.index_of(topic) + 1; // DAVIDE
   rtcan_id_t rtcan_id;

   // id 0 reserved to management topic
   if (&topic == &mgmt_topic) {
      return((0x00 << 8) | stm32_id8());
   }

   // DAVIDE
   // id 253 reserved to bootloader topic
   if (Topic::has_name(topic, "bootloader")) {
      return((0xFD << 8) | stm32_id8());
   }

   // id 254 reserved to test topic
   if (Topic::has_name(topic, "test")) {
      return((0xFE << 8) | stm32_id8());
   }

   if (index < 0) {
      return(255 << 8);
   }

   rtcan_id = ((index & 0xFF) << 8) | stm32_id8();

   return rtcan_id;
} // RTCANTransport::topic_id

NAMESPACE_CORE_MW_END
