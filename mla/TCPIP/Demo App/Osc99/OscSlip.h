/**
 * @file OscSlip.h
 * @author Seb Madgwick
 * @brief Functions and structures for encoding and decoding OSC packets using
 * the SLIP protocol.
 * @see http://en.wikipedia.org/wiki/Serial_Line_Internet_Protocol
 */

#ifndef OSC_SLIP_H
#define OSC_SLIP_H

//------------------------------------------------------------------------------
// Includes

#include "OscPacket.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief OSC SLIP decoder buffer size.  If a packet size exceeds the buffer
 * size then the packet will be truncated with the oldest bytes discarded.
 */
#define OSC_SLIP_DECODER_BUFFER_SIZE MAX_TRANSPORT_SIZE

/**
 * @brief OSC SLIP decoder structure.  Must be initialised using
 * OscSlipDecoderInitialise.
 */
typedef struct {
    char buffer[OSC_SLIP_DECODER_BUFFER_SIZE];
    int bufferIndex;
    void (*processPacket)(OscPacket * const oscPacket);
} OscSlipDecoder;

//------------------------------------------------------------------------------
// Function prototypes

void OscSlipDecoderInitialise(OscSlipDecoder * const oscSlipDecoder);
int OscSlipDecoderProcessByte(OscSlipDecoder * const oscSlipDecoder, const char byte);
int OscSlipEncodePacket(const OscPacket * const oscPacket, size_t * const slipPacketSize, char* const destination, const size_t destinationSize);

#endif

//------------------------------------------------------------------------------
// End of file
