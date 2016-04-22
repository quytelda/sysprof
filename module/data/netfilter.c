/*
 * netfilter.c - Collect data about packets and network operations
 *
 * Copyright (C) 2016 Quytelda Kahja, Roger Xiao
 * This file is part of Sysprof.
 *
 * Sysprof is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sysprof is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Sysprof.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

static unsigned int ipv4_count = 0, ipv6_count = 0;
static unsigned int udp_count = 0, tcp_count = 0;
static unsigned int packets_in_count = 0, packets_out_count = 0;

static unsigned int hook_func_in(void * priv,
				 struct sk_buff * skb,
				 const struct nf_hook_state * state)
{
    // for interpreting udp and tcp packets
    struct udphdr * udp_header = NULL;
    struct tcphdr * tcp_header = NULL;

    // TODO: not sure if we need these
    // struct list_head *p;			//Not quite sure what this does
    // struct mf_rule *a_rule;			//List of firewall rules

    packets_in_count++; // keep track of incoming packet count

    //Get the protocol, length, source IP, and destination IP of a packet caught in the hook:
    struct iphdr *ip_header = (struct iphdr *) skb_network_header(skb);

    unsigned int prot = (unsigned int)ip_header->protocol;
    unsigned int len = (unsigned int)ip_header->tot_len;
    unsigned int src_ip = (unsigned int)ip_header->saddr;
    unsigned int dest_ip = (unsigned int)ip_header->daddr;

    // the iphdr also has parameters check, frag_off, id, tot_len, and ttl. What do they do? How can we use them?

    switch(ip_header->protocol)
    {
    case IPPROTO_IPIP : // IPv4 packet
	ipv4_count++;
	break;
    case IPPROTO_IPV6 : // IPv6 packet
	ipv6_count++;
	break;
    default :
	;
    }


    if(prot == 6){		// TCP
	// get source/destination data (special considerations)
	// tcp_header = (struct tcphdr *)skb_transport_header(skb); //Special handler for TCP packets
	// src_port = (unsigned int)ntohs(tcp_header->source);
	// dest_port = (unsigned int)ntohs(tcp_header->dest);

	tcp_count++;
    }
    else if(prot == 17){   //UDP

	/* udp_header = (struct udphdr *)skb_transport_header(skb); */
	/* src_port = (unsigned int)ntohs(udp_header->source); */
	/* dest_port = (unsigned int)ntohs(udp_header->dest); */

	udp_count++;
    }

    //Let the packet through. We're just observing
    return NF_ACCEPT;			 
}

static struct nf_hook_ops nfho =
{
    .hook     = hook_func_in,
    .hooknum  = NF_INET_LOCAL_IN,
    .pf       = PF_INET,
    .priority = NF_IP_PRI_FIRST,
};

/*
 */
void init_netfilter(void)
{
    nf_register_hook(&nfho);         // Register the hook
}


void exit_netfilter(void)
{
    nf_unregister_hook(&nfho);
}
