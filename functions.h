

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include <string>
#include <unistd.h>
#include <iostream>
using namespace std;

u_int16_t compute_icmp_checksum (const void *buff, int length);
void print_as_bytes (unsigned char* buff, ssize_t length);
bool IpAddressValidation(char* ip);
void send_3_packets(int sockfd, struct sockaddr_in recipient, int ttl);
int receive_3_packets(int sockfd, double Time[3], std::string addr[3], int ttl);
void print(double Time[3], std::string addr[3], int i);
