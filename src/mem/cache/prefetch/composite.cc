/*
 * Copyright (c) 2013-2014 ARM Limited
 * All rights reserved.
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Ron Dreslinski
 *          Mitch Hayenga
 */

/**
 * @file
 * Hardware Prefetcher Definition.
 */

#include "mem/cache/prefetch/composite.hh"

#include "debug/HWPrefetch.hh"
#include "params/TaggedPrefetcher.hh"
#include "params/StridePrefetcher.hh"

CompositePrefetcher::CompositePrefetcher(const CompositePrefetcherParams *p)
    : QueuedPrefetcher(p),
      strideEnabled(p->stride_enabled),
      taggedEnabled(p->tagged_enabled),
      stride(p->stride),
      tagged(p->tagged)
{
}

CompositePrefetcher::~CompositePrefetcher()
{
    delete stride;
    delete tagged;
}

Tick
CompositePrefetcher::notify(const PacketPtr &pkt)
{
    std::list<Tick> tickList;
    std::list<Tick>::iterator itr;

    if(strideEnabled)
        tickList.push_front(stride->notify(pkt));
    if(taggedEnabled)
        tickList.push_front(tagged->notify(pkt));

    Tick minTick = -1;
    for(itr = tickList.begin(); itr != tickList.end(); ++itr){
        if(minTick > *itr || minTick == -1)
            minTick = *itr;
    }

    return minTick;
}

PacketPtr
CompositePrefetcher::getPacket()
{
    if(strideEnabled && stride->getPacket() != NULL){
        return stride->getPacket();
    }
    else if(taggedEnabled && tagged->getPacket() != NULL){
        return tagged->getPacket();
    }

    return NULL;
}

void
CompositePrefetcher::calculatePrefetch(const PacketPtr &pkt,
                                    std::vector<AddrPriority> &addresses)
{
    assert(0);
}

void
CompositePrefetcher::regStats()
{
    BasePrefetcher::regStats(); // TODO
}

CompositePrefetcher*
CompositePrefetcherParams::create()
{
   return new CompositePrefetcher(this);
}
