/*
* Reverced udpxy. Grab stream from tcp(http) to udp
* Copyright 20014 Alexandr Barabash s1z@ukr.net
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void error(const char *err){
    fprintf(stdout,"error: %s\n", err);
}

char* append(const char* ch1, const char* ch2);
int check200 (char* response);

int main(int argc, char *argv[]){
    if(argc < 6 || argc > 8){
        fprintf(stdout,"Usage: %s hostname port httprequest udpaddr udpport [ps/frame][int]\n", argv[0]);
        fprintf(stdout,"Example: %s 192.168.1.1 80 /udpxy/238.1.1.1:1234 238.1.1.1 1234 7 eth0\n", argv[0]);
        return 1;
    }
    int TS_PACKET_SIZE  = 188;
    int UDP_SIZE = TS_PACKET_SIZE * 7;
    int BUF_SIZE = 65535;
    int sockfd, sockfd_m;
    int offset, rc, reuse = 1;
    int packets = 7;
    char buffer[BUF_SIZE];
    char ts_packet[TS_PACKET_SIZE];
    char udp_packet[UDP_SIZE];
    char *devname = '\0';
    char *request;
    sockaddr_in addr;
    sockaddr_in addr_m;

    if(argc == 7){
        packets = atoi(argv[6]);
        if(packets < 1 || packets > 7){
            packets = 7;
        }
    }
    if(argc == 8){devname = argv[7];fprintf(stdout,"Stream to dev: %s\n", devname);}

    addr.sin_family = AF_INET;
    addr_m.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr_m.sin_addr.s_addr = inet_addr(argv[4]);
    addr.sin_port = htons(atoi(argv[2]));
    addr_m.sin_port = htons(atoi(argv[5]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {error("socket tcp error");return 1;}
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0){error("tcp reuse error");}

    sockfd_m = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd_m < 0) {error("socket udp error");close(sockfd);return 1;}
    if (setsockopt(sockfd_m, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0){error("udp reuse error");}
    if (devname){
        if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, devname, strlen(devname)) < 0) {
            fprintf(stdout,"Cannot bind to %s\n",devname);
            close(sockfd);
            close(sockfd_m);
            return 0;
        }
    }

    rc = connect(sockfd, (sockaddr *)&addr, sizeof(struct sockaddr_in));
    if(rc < 0) {error("connect tcp error");close(sockfd);close(sockfd_m);return 1;}

    request = (char *)&"GET ";
    request = append(request,argv[3]);
    request = append(request," HTTP/1.0\r\n\r\n");

    rc = write(sockfd,request,strlen(request));
    rc = recv(sockfd, buffer, BUF_SIZE, 0);
    if(rc <= 0){close(sockfd);close(sockfd_m);return 1;}

    rc = check200(buffer);
    if(rc < 0){
        fprintf(stderr, "Error response from server,\nExpecting \"200 OK\" and \"application/octet-stream\"\n");
        return 1;
    }

    for(offset = 0; offset < BUF_SIZE; offset++){
        if(buffer[offset] == 0x0d && buffer[offset+1] == 0x0a && buffer[offset+2] == 0x0d && buffer[offset+3] == 0x0a){
            offset = offset + 4;
            break;
        }
    }
    int j = 0;
    while(rc > 0){
        for(int i = offset; i < rc; i++){
            udp_packet[j] = buffer[i];
            j++;
            if(j == TS_PACKET_SIZE * packets){
                j = 0;
                sendto(sockfd_m, udp_packet, TS_PACKET_SIZE * packets, 0, (struct sockaddr *)&addr_m, sizeof(struct sockaddr_in));
            }
        }
        offset = 0;
        rc = recv(sockfd, buffer, BUF_SIZE, 0);
	if (rc == 0){
		break;
	}
    }
    close(sockfd);
    close(sockfd_m);
    return 0;
}

char* append(const char* ch1, const char* ch2){
        int len = strlen(ch1) + strlen(ch2);
        char* ret = new char[len + 1];
        int i = 0;
        for(i; i < strlen(ch1); i++ ){
                ret[i] = ch1[i];
        }
        for(int j = 0; j < strlen(ch2); j++ ){
                ret[i+j] = ch2[j];
        }
        return ret;
}

int check200 (char* response){
    char* ok = strstr(response,"200 OK");
    char* oc = strstr(response,"application/octet-stream");
    if(oc == NULL){
        oc = strstr(response,"video/mpeg");
    }
    if(ok == NULL || oc == NULL){
        return -1;
    }
    return 1;
}
