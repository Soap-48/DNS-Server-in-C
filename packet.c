#include "packet.h"
#include <stdio.h>

void print_hex(const unsigned char *data, int len) {
    for (int i = 0; i < len; i++) {
        printf("%02x ", data[i]);
    }
    printf("\n");
}

int parse_header(header *info, const unsigned char *data, int len) {
    if (len < 12) {
        return -1;
    }

    // each char is 8 bit long
    //  ID (16-bit, big-endian)
    info->ID = (data[0] << 8) | data[1];

    // Byte 2: QR(1), OPCODE(4), AA(1), TC(1), RD(1)
    unsigned char byte2 = data[2];
    info->QR = (byte2 >> 7) & 0x01;
    info->OPCODE = (byte2 >> 3) & 0x0F;
    info->AA = (byte2 >> 2) & 0x01;
    info->TC = (byte2 >> 1) & 0x01;
    info->RD = byte2 & 0x01;

    // Byte 3: RA(1), Z(3), RCODE(4)
    unsigned char byte3 = data[3];
    info->RA = (byte3 >> 7) & 0x01;
    info->Z = (byte3 >> 4) & 0x07;
    info->RCODE = byte3 & 0x0F;

    // Counts (16-bit each, big-endian)
    info->QDCOUNT = (data[4] << 8) | data[5];
    info->ANCOUNT = (data[6] << 8) | data[7];
    info->NSCOUNT = (data[8] << 8) | data[9];
    info->ARCOUNT = (data[10] << 8) | data[11];

    return 0;
}

int parse_question(question *q, const unsigned char *data) {
    //domain parsing
    int len = 12;
    int siz = (int)data[12];
    int i=0;
    while (siz != 0) {
        q->domain[i++]=data[len++];
        while(siz>0){
            siz-=1;
            q->domain[i++]=data[len++];
        }
        siz=(int)data[len];
    }
    q->domain[i++]='\x00';
    q->len=i;
    len++;
    //type parsing
    q->type=(((int)data[len++])<<8)|((int)data[len++]);

    //class parsing
    q->class=(((int)data[len++])<<8)|((int)data[len++]);

    return 0;
}

int create_header(header *info, unsigned char *out) {
    info->QR = 1;

    // ID (16 bits, big-endian)
    out[0] = (info->ID >> 8) & 0xFF;
    out[1] = info->ID & 0xFF;

    // Byte 2: QR (1 bit), OPCODE (4 bits), AA (1 bit), TC (1 bit), RD (1 bit)
    out[2] = ((info->QR & 0x01) << 7) |
             ((info->OPCODE & 0x0F) << 3) |
             ((info->AA & 0x01) << 2) |
             ((info->TC & 0x01) << 1) |
             (info->RD & 0x01);

    // Byte 3: RA (1 bit), Z (3 bits), RCODE (4 bits)
    out[3] = ((info->RA & 0x01) << 7) |
             ((info->Z & 0x07) << 4) |
             (info->RCODE & 0x0F);

    // QDCOUNT (16 bits, big-endian)
    out[4] = (info->QDCOUNT >> 8) & 0xFF;
    out[5] = info->QDCOUNT & 0xFF;

    // ANCOUNT (16 bits, big-endian)
    out[6] = (info->ANCOUNT >> 8) & 0xFF;
    out[7] = info->ANCOUNT & 0xFF;

    // NSCOUNT (16 bits, big-endian)
    out[8] = (info->NSCOUNT >> 8) & 0xFF;
    out[9] = info->NSCOUNT & 0xFF;

    // ARCOUNT (16 bits, big-endian)
    out[10] = (info->ARCOUNT >> 8) & 0xFF;
    out[11] = info->ARCOUNT & 0xFF;
    return 0;
}

int create_question(question *q, unsigned char *out) {
    int i = 12;
    for (int j = 0; j < q->len; j++)
        out[i++] = q->domain[j];

    out[i++] = ((q->type >> 8) & 0xFF);
    out[i++] = (q->type & 0xFF);
    out[i++] = ((q->class >> 8) & 0xFF);
    out[i] = (q->class & 0xFF);

    return i;
}

int create_answer(answer *a, unsigned char *out, int offset){
     int i = offset+1;
    for (int j = 0; j < a->len; j++)
        out[i++] = a->domain[j];

    out[i++] = ((a->type >> 8) & 0xFF);
    out[i++] = (a->type & 0xFF);
    out[i++] = ((a->class >> 8) & 0xFF);
    out[i++] = (a->class & 0xFF);

    out[i++] = ((a->ttl >> 24) & 0xFF);
    out[i++] = ((a->ttl >> 16) & 0xFF);
    out[i++] = ((a->ttl >> 8) & 0xFF);
    out[i++] = ((a->ttl) & 0xFF);

    out[i++] = ((a->length >> 8) & 0xFF);
    out[i++] = (a->length & 0xFF);

    for (int j = 0; j < 4;j++)
        out[i++] = a->data[j];
    
    return i;    
}

int create_packet(header *h, question *q, answer *a, unsigned char *out) {
    h->QR = 1;
    h->AA = 0;
    h->TC = 0;
    h->RA = 0;
    h->Z = 0;
    h->ANCOUNT = 1;
    h->RCODE = (h->OPCODE) ? 4 : 0;
    create_header(h, out);

    int offset = create_question(q, out);
    
    a->domain=q->domain;
    a->type=q->type;
    a->class=q->class;
    a->ttl=78;
    a->length=4;
    a->data="\x08\x08\x08\x08";
    a->len=q->len;
    offset = create_answer(a, out, offset);

    return offset;
}