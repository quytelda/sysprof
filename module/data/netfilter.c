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
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

static unsigned int ipv4_count = 0, ipv6_count = 0;
static unsigned int udp_count = 0, tcp_count = 0;
static unsigned int packets_in_count = 0, packets_out_count = 0;

static unsigned int hook_func_in(void * priv,
				 struct sk_buff * skb,
				 const struct nf_hook_state * state)
{
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
