

#include "functions.h"
using namespace std;


int main(int argc, char *argv[])
{
    if (argc != 2 || IpAddressValidation(argv[1]))
    {
        printf("input error\n");
        return EXIT_FAILURE;
    }
    double Time[3];
    string addr[3];  
    
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

    //adresowanie
    struct sockaddr_in recipient;
    bzero (&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &recipient.sin_addr);
    
    
    for (int ttl = 1; ttl < 31; ttl++)
    {
        send_3_packets(sockfd, recipient, ttl);
        receive_3_packets(sockfd, Time, addr, ttl);
        print(Time, addr, ttl);
        if(addr[0] == argv[1] && addr[1] == argv[1] && addr[2] == argv[1])
            break;
    }
	return EXIT_SUCCESS;
}
