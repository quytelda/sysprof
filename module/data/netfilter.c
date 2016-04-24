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
static unsigned int udp_in_count = 0, tcp_in_count = 0, icmp_in_count = 0;
static unsigned int udp_out_count = 0, tcp_out_count = 0, icmp_out_count = 0;
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
    
    unsigned int dest_ip = 0;
    unsigned int src_ip = 0;

    packets_in_count++; // keep track of incoming packet count

    //Get the protocol, length, source IP, and destination IP of a packet caught in the hook:
    struct iphdr *ip_header = (struct iphdr *) skb_network_header(skb);
    
    //struct  iphdr *mac_header = (struct iphdr *) skb_mac_header(skb);
    //struct  iphdr *trans_header = (struct iphdr *) skb_transport_header(skb); 
   
    //What does the mac_header do???????????

    unsigned int prot = (unsigned int)ip_header->protocol;
    unsigned int len = (unsigned int)ip_header->tot_len;
    unsigned int src_ip = (unsigned int)ip_header->saddr;
    unsigned int dest_ip = (unsigned int)ip_header->daddr;

    // the iphdr also has parameters check, frag_off, id, tot_len, and ttl. What do they do? How can we use them?

	//Anything we can learn from ttl (time to live) value???????????

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
    
    //The following protocols need to be treated a certain way
    if(prot == 6){		// TCP
	// get source/destination data (special considerations)
	tcp_header = (struct tcphdr *)skb_transport_header(skb); //Special handler for TCP packets
	src_port = (unsigned int)ntohs(tcp_header->source);
	dest_port = (unsigned int)ntohs(tcp_header->dest);

	src_ip = tcp_header->src_ip;
    	dest_ip = tcp_header->dest_ip;

	//Port analysis/count?????????? Perhaps its bad if all packets are from or going to the same port???????????????

	tcp_in_count++;
    }
    else if(prot == 17){   //UDP

	udp_header = (struct udphdr *)skb_transport_header(skb);
	src_port = (unsigned int)ntohs(udp_header->source);
	dest_port = (unsigned int)ntohs(udp_header->dest);

	src_ip = udp_header->src_ip;
    	dest_ip = udp_header->dest_ip;

	//Port analysis/count????????

	udp_in_count++;
    }
    else if(prot == 1){	//ICMP: very common in DDoS attacks
  	
  	icmp_header = (struct icmphdr *)skb_transport_header(skb);
	src_port = (unsigned int)ntohs(icmp_header->source);
	dest_port = (unsigned int)ntohs(icmp_header->dest);
  	
  	src_ip = icmp_header->src_ip;
    	dest_ip = icmp_header->dest_ip;
  	
  	//Port analysis/count????????
  	
	icmp_in_count++;
 
    }
    else{	//No special treatment
    
    	src_ip = ip_header->src_ip;
    	dest_ip = ip_header->dest_ip;
    
    	//Port analysis/count????????
    
    }
    
  /*switch(dest_ip)
    {
    	//Keep track of where the packets are going to. If we send an anomalous amount of packets to a bunch of very distant
    	//addresses, something might be wrong.
    	//Sending to a lot of IPs might be more suspicious than sending to very few--A chat session with another machine
    	//may cause a lot of traffic to a specific IP address, but there is little reason to send a single packet to 10,000
    	//different IP addresses
    }*/
    
    //Send counts to shared buffer?????????????

    //Let the packet through. We're just observing
    return NF_ACCEPT;			 
}

static unsigned int hook_func_in(void * priv,
				 struct sk_buff * skb,
				 const struct nf_hook_state * state)
{
    struct udphdr * udp_header = NULL;
    struct tcphdr * tcp_header = NULL;
    
    unsigned int dest_ip = 0;
    unsigned int src_ip = 0;

    packets_out_count++; // keep track of incoming packet count

    //Get the protocol, length, source IP, and destination IP of a packet caught in the hook:
    struct iphdr *ip_header = (struct iphdr *) skb_network_header(skb);

    unsigned int prot = (unsigned int)ip_header->protocol;
    unsigned int len = (unsigned int)ip_header->tot_len;
    unsigned int src_ip = (unsigned int)ip_header->saddr;
    unsigned int dest_ip = (unsigned int)ip_header->daddr;

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
    
    //The following protocols need to be treated a certain way
    if(prot == 6){		// TCP
	// get source/destination data (special considerations)
	tcp_header = (struct tcphdr *)skb_transport_header(skb); //Special handler for TCP packets
	src_port = (unsigned int)ntohs(tcp_header->source);
	dest_port = (unsigned int)ntohs(tcp_header->dest);

	src_ip = tcp_header->src_ip;
    	dest_ip = tcp_header->dest_ip;

	tcp_out_count++;
    }
    else if(prot == 17){   //UDP

	udp_header = (struct udphdr *)skb_transport_header(skb);
	src_port = (unsigned int)ntohs(udp_header->source);
	dest_port = (unsigned int)ntohs(udp_header->dest);

	src_ip = udp_header->src_ip;
    	dest_ip = udp_header->dest_ip;

	udp_out_count++;
    }
    else if(prot == 1){	//ICMP: very common in DDoS attacks
  	
  	icmp_header = (struct icmphdr *)skb_transport_header(skb);
	src_port = (unsigned int)ntohs(icmp_header->source);
	dest_port = (unsigned int)ntohs(icmp_header->dest);
  	
  	src_ip = icmp_header->src_ip;
    	dest_ip = icmp_header->dest_ip;
  	
	icmp_out_count++;
 
    }
    else{	//No special treatment
    
    	src_ip = ip_header->src_ip;
    	dest_ip = ip_header->dest_ip;

    }   
    
    /*switch(src_ip)
    {
    	//Keep track of where the packets are coming from. If we get an anomalous amount of packets from a bunch of very distant
    	//addresses, something might be wrong. Use geolocator to determine countries from IP addresses?????????????????
    	//Also check if a certain IP address(es) is/are sending a disproportional amount of packets (perhaps 1 machine is
    	//trying to do a DOS attack)
    }*/
    
    /*if(dest_ip != ????????????){
	Is there any reason the destination IP address wouldn't be the current machine's IP address?????
	Also, is it possible to set a variable equal to the machine's IP address?????
    }*/
    
    //Send counts to shared buffer?????????????

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

static struct nf_hook_ops nfho_out = 
{
    .hook 	= hook_func_out;
    .hooknum    = NF_INET_LOCAL_OUT;
    .pf 	= PF_INET;
    .priority   = NF_IP_PRI_FIRST, 	//Can they both be first????????
};

//Via example at http://pleviaka.pp.fi/netfilter_module.c

/*
 */
void init_netfilter(void)
{
    nf_register_hook(&nfho);         // Register the hook
    nf_register_hook(&nfho_out);
}


void exit_netfilter(void)
{
    nf_unregister_hook(&nfho);
    nf_unregister_hook(&nfho_out);
}
