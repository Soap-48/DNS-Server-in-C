#ifndef PACKET_H
#define PACKET_H

//stores header info
typedef struct{
    //ARCOUNT
    int ID; //16 bit, ID assinged to packets //byte 0,1
    int QR; //1 bit, Query/Response Indicator, 1 for reply, 0 for query //byte 2
    int OPCODE; //4 bit, What kind of Query //byte 2
    int AA; //1 bit, 1 if the server replying is authorative //byte 2
    int TC; //1 bit, 1 if the message is larger than 512 bytes. Always 0 in UDP responses. //byte 2
    int RD; //1 bit, Sender sets this to 1 if the server should recursively resolve this query, 0 otherwise //byte 2
    int RA; //1 bit, Server sets this to 1 to indicate that recursion is available. //byte 3
    int Z; //3 bit, Reserved. Used by DNSSEC queries //byte 3
    int RCODE; //4 but, Response code indicating the status of the response //byte 3
    int QDCOUNT; //16 bit, Number of questions in the Question section ///byte 4,5
    int ANCOUNT; //16 bit, Number of questions in the Answer section ///byte 6,7
    int NSCOUNT; //16 bit, Number of questions in the Authority section ///byte 8,9
    int ARCOUNT; //16 bit, Number of questions in the Additional section ///byte 10,11
} header; //total 12 byte long

//stores question info
typedef struct{
    unsigned char *domain;   //variable bytes, stores domain name google.com -> \x06google\x03com\x00
    int type;   //2 bytes
    int class;  //2 bytes
    int len; //stores size of domain till where data is stored data[len-1]=\x00
} question;

//stores answer
typedef struct{
    unsigned char *domain;   //variable bytes, stores domain name google.com -> \x06google\x03com\x00
    int type;   //2 bytes
    int class;  //2 bytes
    int ttl;    //4 byte
    int length; //2 byte, stores size of data field, 4 for IPv4
    unsigned char* data; //4 byte for IPv4, stores IP, 8.8.8.8 -> \x08\x08\x08\x08
    int len; //stores size of domain till where data is stored data[len-1]=\x00
} answer;

//stores authority

//stores additional

//struct of packet

void print_hex(const unsigned char*, int);
int parse_header(header *,const unsigned char *,int);
int create_header(header *,unsigned char *);
int parse_question(question *, const unsigned char *);
int create_question(question *, unsigned char *);
int create_answer(answer *, unsigned char *, int);
int create_packet(header *, question *, answer *, unsigned char *);

#endif