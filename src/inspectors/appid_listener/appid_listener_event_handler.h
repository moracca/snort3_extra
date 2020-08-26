//--------------------------------------------------------------------------
// Copyright (C) 2020-2020 Cisco and/or its affiliates. All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------
// appid_listener_event_handler.h author Shravan Rangaraju <shrarang@cisco.com>

#ifndef APPID_LISTENER_EVENT_HANDLER_H
#define APPID_LISTENER_EVENT_HANDLER_H

#include "framework/counts.h"
#include "framework/data_bus.h"
#include "helpers/json_stream.h"
#include "log/messages.h"
#include "network_inspectors/appid/application_ids.h"
#include "pub_sub/appid_events.h"
#include "appid_listener.h"

namespace snort
{
class AppIdSessionApi;
class Flow;
struct Packet;
}

class AppIdListenerEventHandler : public snort::DataHandler
{
public:
    AppIdListenerEventHandler(const AppIdListenerConfig& config) :
        DataHandler(MOD_NAME), config(config) { }

    void handle(snort::DataEvent& event, snort::Flow* flow) override;

private:
    const AppIdListenerConfig& config;

    void print_message(const char*, const char*, const snort::Flow&, PegCount,
        AppId, AppId, AppId, AppId, AppId) const;
    void print_json_message(snort::JsonStream&, const char*, const char*, const snort::Flow&,
        PegCount, const snort::AppIdSessionApi&, AppId, AppId, AppId, AppId, AppId, bool, uint32_t,
        const snort::Packet*) const;

    bool appid_changed(const AppidChangeBits& ac_bits) const
    {
        if (ac_bits.test(APPID_RESET_BIT) or ac_bits.test(APPID_SERVICE_BIT) or
            ac_bits.test(APPID_CLIENT_BIT) or ac_bits.test(APPID_MISC_BIT) or
            ac_bits.test(APPID_PAYLOAD_BIT) or ac_bits.test(APPID_REFERRED_BIT))
            return true;

        return false;
    }

    std::string get_proto_str(uint8_t ip_proto) const
    {
        switch(ip_proto)
        {
        case 1:
            return "ICMP";
        case 2:
            return "IGMP";
        case 6:
            return "TCP";
        case 17:
            return "UDP";
        default:
            return std::to_string(ip_proto);
        }
    }

    void print_json_header(snort::JsonStream& js, const char* cli_ip_str,
        const char* srv_ip_str, uint16_t client_port, uint16_t server_port, uint8_t ip_proto,
        PegCount packet_number) const
    {
        js.put("client_ip", cli_ip_str);
        js.put("client_port", client_port);
        js.put("server_ip", srv_ip_str);
        js.put("server_port", server_port);
        js.put("proto", get_proto_str(ip_proto));
        js.put("packet_num", packet_number);
    }

    void print_header(const char* cli_ip_str, const char* srv_ip_str, uint16_t client_port,
        uint16_t server_port, uint8_t ip_proto, PegCount packet_number) const
    {
        snort::LogMessage("%s:%d<->%s:%d proto: %d packet: " STDu64,
            cli_ip_str, client_port, srv_ip_str, server_port, ip_proto, packet_number);
    }

};

#endif