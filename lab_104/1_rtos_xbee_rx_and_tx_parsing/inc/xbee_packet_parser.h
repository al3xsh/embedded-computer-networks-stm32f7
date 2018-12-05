/*
 * xbee_packet_parser.h
 *
 * this is where we take the data stream from the xbee and actually extract the
 * data into packets (using the xbee state machine)
 *
 * author:		Alex Shenfield
 * date:			10/11/2017
 */

// define to prevent recursive inclusion
#ifndef __XBEE_PARSER_H
#define __XBEE_PARSER_H

// include the basic headers
#include "stm32f7xx.h"

// if we've not defined RING_SIZE elsewhere ...
#ifndef RING_SIZE
	// make the buffer size 1024 (buffer sizes that are a power of two make code 
	// much more efficient as compilers can optimise better)
	#define RING_SIZE 1024
#endif

// define a uart buffer type that implements a ring buffer implementation
// of a fifo queue to store sent and received data
typedef struct
{
	uint8_t		data[RING_SIZE];
	uint16_t	ring_head;
	uint16_t	ring_tail;
	uint16_t	num_bytes;
} 
buffer_typedef;

// states for the xbee parser
typedef enum state
{
	INIT,
	PACKETLENGTH_HI,
	PACKETLENGTH_LO,
	DATAFIELD,
	CHECKSUM,
	COMPLETE
} 
parse_state;

// 
// global methods:
//
void init_parser(void);
int  xbee_parse_packet(uint8_t c);
void get_packet(uint8_t * packet_buffer);
void xbee_send_packet(uint8_t * packet, int length);

#endif // XBEE_PARSER_H

