/*
 * xbee_packet_parser.c
 *
 * this is where we take the data stream from the xbee and actually extract the
 * data into packets (using the xbee state machine)
 *
 * author:		Alex Shenfield
 * date:			10/11/2017
 */
#include "stm32f7xx.h"
#include "xbee_packet_parser.h"
#include "xbee.h"

// include stdio for sprintf
#include <stdio.h>
#include "itm_debug.h"

// XBEE METHODS

// calculate the xbee packet checksum
int validate_packet(void);

// initialise the parser state machine and the niumber of elements to parse
static parse_state state;
static int xbee_remain;

// initialise a buffer
volatile buffer_typedef xbee_buffer = {{0}, 0, 0, 0};

// initialise the parser
void init_parser(void)
{
	state = INIT;
	xbee_remain = 0;
}

// parse an xbee api packet
int xbee_parse_packet(uint8_t c)
{
	// debugging code to diagnose packet reception problems ...
	//char buf[5];
	//sprintf(buf, "%02X ", c);
	//print_debug(buf, 3);
	
	// whilst there is data in the rx buffer and the xbee buffer isn't full ...
	while((xbee_buffer.num_bytes < RING_SIZE))
	{
		// check if it is an api frame header
		if(state == INIT && c == 0x7e)
		{			
			// add data to the buffer
			xbee_buffer.data[xbee_buffer.ring_head] = c;
			xbee_buffer.ring_head = (xbee_buffer.ring_head + 1) % RING_SIZE;
			xbee_buffer.num_bytes++;

			state = PACKETLENGTH_HI;
			break;
		}

		// read high byte of data field length
		if(state == PACKETLENGTH_HI)
		{			
			// add data to the buffer
			xbee_buffer.data[xbee_buffer.ring_head] = c;
			xbee_buffer.ring_head = (xbee_buffer.ring_head + 1) % RING_SIZE;
			xbee_buffer.num_bytes++;

			xbee_remain += c * 10;

			state = PACKETLENGTH_LO;
			break;
		}

		// read low byte of data field length
		if(state == PACKETLENGTH_LO)
		{			
			// add data to the buffer
			xbee_buffer.data[xbee_buffer.ring_head] = c;
			xbee_buffer.ring_head = (xbee_buffer.ring_head + 1) % RING_SIZE;
			xbee_buffer.num_bytes++;

			xbee_remain += c;

			state = DATAFIELD;
			break;
		}

		// read datafield
		if(state == DATAFIELD && xbee_remain > 0)
		{			
			// add data to the buffer
			xbee_buffer.data[xbee_buffer.ring_head] = c;
			xbee_buffer.ring_head = (xbee_buffer.ring_head + 1) % RING_SIZE;
			xbee_buffer.num_bytes++;
			xbee_remain--;

			// if we've read all the data field, move on to the checksum
			if(xbee_remain == 0)
			{
				state = CHECKSUM;
			}
			break;
		}

		// read checksum
		if(state == CHECKSUM)
		{
			// add data to the buffer
			xbee_buffer.data[xbee_buffer.ring_head] = c;
			xbee_buffer.ring_head = (xbee_buffer.ring_head + 1) % RING_SIZE;
			xbee_buffer.num_bytes++;

			state = COMPLETE;
		}

		// if the xbee packet is done then print it ...
		if(state == COMPLETE)
		{
			// verify packet
			if(!validate_packet())
			{
				// if things have gone wrong, dump the packet to the itm terminal to 
				// help diagnose the problem
				print_debug("CORRUPTED  ", 11);
				while(xbee_buffer.num_bytes > 0)
				{
					// get char from xbee buffer
					uint8_t c = xbee_buffer.data[xbee_buffer.ring_tail];
					xbee_buffer.ring_tail = (xbee_buffer.ring_tail + 1) % RING_SIZE;
					xbee_buffer.num_bytes--;

					char buf[5];
					sprintf(buf, "%02X ", c);
					print_debug(buf, 3);
				}
				state = INIT;
				return 0;
			}
			
			// set the state to INIT ready to parse the next packet
			state = INIT;
			return xbee_buffer.num_bytes;
		}
	}
	return 0;
}

// validate an xbee packet by calculating the checksum and comparing it to the
// one that the xbee sends
int validate_packet(void)
{
	int i;
	int sum = 0;
	for(i = 3; i < (xbee_buffer.num_bytes - 1); i++)
	{
		sum += xbee_buffer.data[(xbee_buffer.ring_tail + i) % RING_SIZE];
	}
	return ((0xFF - (sum & 0xFF)) == xbee_buffer.data[xbee_buffer.ring_head - 1]);
}

// get the xbee frame in a packet buffer
void get_packet(uint8_t * packet_buffer)
{
	int i = 0;
	while(xbee_buffer.num_bytes > 0)
	{
		// get char from xbee buffer
		uint8_t c = xbee_buffer.data[xbee_buffer.ring_tail];
		xbee_buffer.ring_tail = (xbee_buffer.ring_tail + 1) % RING_SIZE;
		xbee_buffer.num_bytes--;

		// write into packet buffer
		packet_buffer[i++] = c;
	}
}
