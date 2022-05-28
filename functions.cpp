

#include "functions.h"
using namespace std;

u_int16_t compute_icmp_checksum (const u_int16_t *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

void print_as_bytes (unsigned char* buff, ssize_t length)
{
	for (ssize_t i = 0; i < length; i++, buff++)
		printf ("%.2x ", *buff);	
}

bool IpAddressValidation(char* ip)
{
    struct sockaddr_in sa_in;
    return inet_pton(AF_INET, ip, &(sa_in.sin_addr)) == 0;
}

void send_3_packets(int sockfd, struct sockaddr_in recipient, int ttl){

    int pid = getpid();
    for(int i=0; i<3; i++)
    {
        //tworzenie danych do wysyki
        struct icmp header;
        header.icmp_type = ICMP_ECHO;
        header.icmp_code = 0;
        header.icmp_hun.ih_idseq.icd_id = htons(pid);
        header.icmp_hun.ih_idseq.icd_seq = htons(10*ttl + i); 
        header.icmp_cksum = 0;
        header.icmp_cksum = compute_icmp_checksum (
                (u_int16_t*)&header, sizeof(header));

        //ustawienie opcji gniazda
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

        //wysylanie pakietu przez gniazdo
        sendto (
            sockfd,
            &header,
            sizeof(header),
            0,
            (struct sockaddr*)&recipient,
            sizeof(recipient));

        
    }
}


int receive_3_packets(int sockfd, double Time[3], std::string addr[3], int ttl)
{    
    int pid = getpid();
    
    //ustawienie danych dla selecta
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000000;

    int ready;
    int counter = 0;

    for(int i = 0; i<3; i++){
        Time[i]=0;
        addr[i]="";
    }
    
    while((ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv))!=0)
    {    
        if (ready < 0)
        {
            printf("select error\n");
            return EXIT_FAILURE;
        }
        if (ready > 0)
        {
            struct sockaddr_in 	sender;	
		    socklen_t 			sender_len = sizeof(sender);
		    u_int8_t 			buffer[IP_MAXPACKET];

            //odbieranie pakietu z gniazda
		    ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, 
            (struct sockaddr*)&sender, &sender_len);
        
            if (packet_len < 0) 
            {
		    	fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
		    	return EXIT_FAILURE;
		    }

            //rzutowanie pakietu 
            struct ip* 	ip_header = (struct ip*) buffer;
		    ssize_t		ip_header_len = 4 * ip_header->ip_hl;
            struct icmp *icmp_header = (struct icmp *)((uint8_t *)
            ip_header+ ip_header_len);

            //przetwarzanie gdy dostalismy odpowiedz ktora chcielismy
            if (icmp_header->icmp_type == ICMP_ECHOREPLY &&
                ntohs(icmp_header->icmp_id) == pid &&
                ntohs(icmp_header->icmp_seq)/10 == ttl)
            {
                counter++;

                //zapisanie numeru ip i czasu
                char sender_ip_str[20]; 
		        inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str,
                sizeof(sender_ip_str));

                int n = ntohs(icmp_header->icmp_seq) % 10;
                addr[n] = std::string (sender_ip_str);
                Time[n] = (1000000.0 - tv.tv_usec)/1000;
            }

            //kolejne rzutowanie
            struct ip* 	ip_header2 = (struct ip*)((uint8_t *)
            icmp_header + 8);
		    ssize_t		ip_header_len2 = 4 * ip_header2->ip_hl;
            struct icmp *icmp_header2 = (struct icmp *)((uint8_t *)
            ip_header2 + ip_header_len2);
                

            //przekroczony czas
            if (icmp_header->icmp_type == ICMP_TIME_EXCEEDED &&
                ntohs(icmp_header2->icmp_id) == pid &&
                ntohs(icmp_header2->icmp_seq)/10 == ttl)
            {
                counter++;

                //zapisanie numeru ip i czasu
                char sender_ip_str[20]; 
		        inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, 
                sizeof(sender_ip_str));

                int n = ntohs(icmp_header2->icmp_seq) % 10;
                addr[n] = std::string (sender_ip_str);
                Time[n] = (1000000.0 - tv.tv_usec)/1000;
            } 
        }
        if(counter == 3)
        {
            break;
        }
    }
    return 0;
}

void print(double Time[3], std::string addr[3], int i)
{
    std::cout << i << ". ";
    if(addr[0] == "" || addr[1] == "" || addr[2] == "")
    {
        std::cout << "*\n";
    }

    else
    {
        std::cout << addr[0];
    
        if(addr[1] != addr[0] )
        {
            std::cout << addr[1];
        }
        if(addr[2] != addr[0] && addr[2] != addr[1])
        {
            std::cout << addr[2];
        }
        if(Time[0] == 0 || Time[1] == 0 || Time[2] == 0)
        {
            std::cout << "   ???\n";
        }
        else
        {
            double avr = (Time[0] + Time[1] + Time[2])/3;
            std::cout << "   " << avr << "ms\n";
        }
    }
}
