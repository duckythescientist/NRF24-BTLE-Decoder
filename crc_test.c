#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

/* Calcualte CRC8-CCITT for the NRF packets. */
uint8_t CRC8(const uint8_t* data, size_t data_len, uint8_t init_crc)
{
    uint8_t i;
    bool bit;
    uint8_t c;
	uint_fast8_t crc=init_crc;
    while (data_len--) {
        c = *data++;
        for (i = 0x80; i > 0; i >>= 1) {
            bit = crc & 0x80;
            if (c & i) {
                bit = !bit;
            }
            crc <<= 1;
            if (bit) {
                crc ^= 0x07;
            }
        }
        crc &= 0xff;
    }
    return (uint8_t)(crc & 0xff);
}

/* Calcualte CRC16-CCITT for the NRF packets. */
uint32_t CRC16(const uint8_t* data, size_t data_len, uint16_t init_crc)
{
    uint8_t i;
    bool bit;
    uint8_t c;
    uint_fast16_t crc=init_crc;
    while (data_len--) {
        c = *data++;
        for (i = 0x80; i > 0; i >>= 1) {
            bit = crc & 0x8000;
            if (c & i) {
                bit = !bit;
            }
            crc <<= 1;
            if (bit) {
                crc ^= 0x1021;
            }
        }
        crc &= 0xffff;
    }
    return (uint16_t)(crc & 0xffff);
}

uint32_t NRFCrc(const uint8_t* data, size_t data_len)
{
    /* using custom start value to compensate for non byte aligned message (due to 9 bit PCF) */
    return CRC16(data, data_len, 0x3C18);
}

void PackPacket(uint64_t packet_addr_l, uint16_t pcf, uint8_t* packet_data, int packet_length, uint8_t* packet_packed){
    int c;
    uint64_t packet_header=packet_addr_l;
    packet_header<<=9;
    packet_header|=pcf;
    for (c=0;c<7;c++){
        packet_packed[c]=(packet_header>>((6-c)*8))&0xFF;
    }

    for (c=0;c<packet_length;c++){
        packet_packed[c+7]=packet_data[c];
    }
}



int main()
{
    // pid:0, no_ack:1, calcCRC: 0x0CF2

    uint64_t packet_addr_l = 0xC111C6673ELL;
    uint16_t pcf = 0x0049;
    uint8_t packet_data[] = {0x00, 0x80, 0x40, 0x80, 0x80, 0x40, 0x40, 0x00, 0xBF};
    int packet_length = 9;
    uint8_t tmp_buf[50];
    PackPacket(packet_addr_l, pcf, packet_data, packet_length, tmp_buf);
    int i=0;
    for(i=0; i<=0xFF; i++)
    {
        uint16_t calced_crc=CRC8(tmp_buf, 7+packet_length, i);
        if(calced_crc == 0x46)
            printf("%X\n", i);


    }
    //printf("0x%04X\n", calced_crc);

}