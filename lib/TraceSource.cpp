/*
 * TraceManager.cpp
 *
 *  Created on: 8 Aug 2014
 *      Author: harry
 */

/*
#include "gensim/gensim_decode.h"
#include "gensim/gensim_processor.h"

#include "tracing/TraceTypes.h"
*/

#include "TraceSink.h"
#include "TraceSource.h"
#include "ArchInterface.h"

#include <cstdint>
#include <cassert>
#include <errno.h>
#include <map>
#include <string>
#include <streambuf>
#include <string.h>
#include <stdio.h>
#include <vector>

using namespace libtrace;

TraceSource::TraceSource(uint32_t BufferSize)
	:
	Tracing_Packet_Count(0),
	packet_open(false),
	is_terminated(false),
	suppressed(false),
	sink_(nullptr),
	aggressive_flushing_(true)
{
	packet_buffer = (TraceRecord*)malloc(PACKET_BUFFER_SIZE * sizeof(TraceRecord));
	packet_buffer_end = packet_buffer+PACKET_BUFFER_SIZE;
	packet_buffer_pos = packet_buffer;
}

TraceSource::~TraceSource()
{
	assert(is_terminated);
}

void TraceSource::EmitPackets()
{
	sink_->SinkPackets(packet_buffer, packet_buffer_pos);
	packet_buffer_pos = packet_buffer;
}

void TraceSource::Terminate()
{
	is_terminated = true;
}



void TraceSource::Flush()
{
	EmitPackets();
	sink_->Flush();
}
