/*structure for firewall policies*/ 
struct mf_rule {
    unsigned char in_out;        //0: neither in nor out, 1: in, 2: out
    unsigned int src_ip;        //
    unsigned int src_netmask;        //
    unsigned int src_port;        //0~2^32
    unsigned int dest_ip; 
    unsigned int dest_netmask;
    unsigned int dest_port;
    unsigned char proto;        //0: all, 1: tcp, 2: udp
    unsigned char action;        //0: for block, 1: for unblock
    struct list_head list;
};

//Register a hook:
inti nit_module(){
	struct nf_hook_ops nfho;
	nfho.hook = hook_func_in;     
	nfho.hooknum = NF_INET_LOCAL_IN;
	nfho.pf = PF_INET;     
	nfho.priority = NF_IP_PRI_FIRST;    
	nf_register_hook(&nfho);         // Register the hook
	return 0;
}

void cleanup_module(){
	nf_unregister_hook(&nfho);
}

unsigned int hook_func_in(unsigned int hooknum, 
                struct sk_buff *skb,  		//the structure used by the network stack to describe packets
                const struct net_device *in, 	//the interface that a packet is arrived on
                const struct net_device *out,         //the interface that a packet is leaving on
                int (*okfn)(struct sk_buff *)){

	struct udphdr *udp_header;			//For interpreting udp and tcp packets
    struct tcphdr *tcp_header;			
	
    struct list_head *p;			//Not quite sure what this does
    struct mf_rule *a_rule;			//List of firewall rules
				
	packets_in_counter++;		//Keep track of incoming packet count
	
	//Get the protocol, length, source IP, and destination IP of a packet caught in the hook:
	struct iphdr *ip_header = (struct iphdr *)skb_network_header(skb);

	unsigned int prot = (unsigned int)ip_header->protocol;
	unsigned int len = (unsigned int)ip_header->tot_len;
	unsigned int src_ip = (unsigned int)ip_header->saddr;
	unsigned int dest_ip = (unsigned int)ip_header->daddr;

	//the iphdr also has parameters check, frag_off, id, tot_len, and ttl. What do they do? How can we use them?
	
	if(prot == 4){ 		//IPv4
		//Do something
		ipv4_count++;		//Perhaps have universal counters for the occurances of types of packets
	}
	
	if(prot == 6){		//TCP
		
	   tcp_header = (struct tcphdr *)skb_transport_header(skb);		//Special handler for TCP packets
       src_port = (unsigned int)ntohs(tcp_header->source);
       dest_port = (unsigned int)ntohs(tcp_header->dest);
	   
	   tcp_count++;
	
	}
	
	else if(prot == 17){		//UDP
	
	   udp_header = (struct udphdr *)skb_transport_header(skb);
       src_port = (unsigned int)ntohs(udp_header->source);
       dest_port = (unsigned int)ntohs(udp_header->dest);
	   
	   udp_count++;
	   
	}
	
	else if(prot == 41){		//IPv6
		//Do something
		
		ipv6_count++;
		
	}
	//What other protocols to keep track of??????????????
	
	//Are we going to record IP addresses and message sizes?????????????
	
	return NF_ACCEPT;			 //Let the packet through. We're just observing
								//We can also block, queue, or redirect the packet

}













//Check packets against firewall policies
   list_for_each(p, &policy_list.list) {
 
       i++;
 
       a_rule = list_entry(p, struct mf_rule, list);		//What does list_entry do????????????
       printk(KERN_INFO "rule %d: a_rule->in_out = %u; a_rule->src_ip = %u; a_rule->src_netmask=%u; a_rule->src_port=%u; a_rule->dest_ip=%u; a_rule->dest_netmask=%u; a_rule->dest_port=%u; a_rule->proto=%u; a_rule->action=%un", i, a_rule->in_out, a_rule->src_ip, a_rule->src_netmask, a_rule->src_port, a_rule->dest_ip, a_rule->dest_netmask, a_rule->dest_port, a_rule->proto, a_rule->action);
 
       //if a rule doesn't specify as "out", skip it
       if (a_rule->in_out != 2) {	//in_out == 2 means packet is going out out
 
           printk(KERN_INFO "rule %d (a_rule->in_out: %u) not match: out packet, rule doesn't specify as outn", i, a_rule->in_out); 
           continue;

       } else {
           //check the protocol
           if ((a_rule->proto==1) && (ip_header->protocol != 6)) {	//A_rule is TCP but ipheader of packet isn't 
               printk(KERN_INFO "rule %d not match: rule-TCP, packet->not TCPn", i);
               continue;
           } 
		   else if ((a_rule->proto==2) && (ip_header->protocol != 17)) {	//A_rule is UDP but ipheader of packet isn't 
               printk(KERN_INFO "rule %d not match: rule-UDP, packet->not UDPn", i);
               continue;
           }
		   
		   //check the ip address
           if (a_rule->src_ip==0) {	//No src IP available. Do nothing
           } 
		   else {
              if (!check_ip(src_ip, a_rule->src_ip, a_rule->src_netmask)) {
 
                  printk(KERN_INFO "rule %d not match: src ip mismatchn", i);
                  continue;
 
              }
           }
 
           if (a_rule->dest_ip == 0) {  //No destination IP
           }
		   else {
               if (!check_ip(dest_ip, a_rule->dest_ip, a_rule->dest_netmask)) {
 
                  printk(KERN_INFO "rule %d not match: dest ip mismatchn", i);                  
                  continue;
 
               }
           }
 
           //check the port number
           if (a_rule->src_port==0) {
               //rule doesn't specify src port: match
           } 
		   else if (src_port!=a_rule->src_port) {
 
               printk(KERN_INFO "rule %d not match: src port mismatchn", i);
               continue;
 
           }
 
           if (a_rule->dest_port == 0) {
               //rule doens't specify dest port: match
           }
 
           else if (dest_port!=a_rule->dest_port) {
 
               printk(KERN_INFO "rule %d not match: dest port mismatchn", i);
               continue;
 
           }
 
           //a match is found: take action
 
           if (a_rule->action==0) {		//0 for block. In this case, drop
 
               printk(KERN_INFO "a match is found: %d, drop the packetn", i);
               printk(KERN_INFO "---------------------------------------n");
               return NF_DROP;
 
           } else {
 
               printk(KERN_INFO "a match is found: %d, accept the packetn", i);
               printk(KERN_INFO "---------------------------------------n");
               return NF_ACCEPT;
 
           }
 
       }
 
   }