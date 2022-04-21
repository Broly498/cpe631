#pragma once

#ifndef _NO_KAFL
// KAFL Includes
#include "kafl_user.h"
#endif

// CPE-631 Includes
#include "cpe631/futexTarget.h"
#include "cpe631/utilities.h"

namespace cpe631
{
int RunSingleThreadedFutexLock() noexcept
{
    FuzzableMutex fuzzableMutex;

#ifndef _NO_KAFL
    // Initialize KAFL Payload Buffer

    hprintf("Beginning fuzzer harness execution...");
    hprintf("Allocating kAFL payload buffer...");

    kAFL_payload* payload_buffer = (kAFL_payload*)mmap((void*)NULL, PAYLOAD_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    hprintf("Memset kAFL payload at address %lx (size %d)\n", (uint64_t)payload_buffer, PAYLOAD_SIZE);

    memset(payload_buffer, 0xff, PAYLOAD_SIZE);

    hprintf("Submitting kAFL payload buffer address to the hypervisor...");

    kAFL_hypercall(HYPERCALL_KAFL_GET_PAYLOAD, (uint64_t)payload_buffer);

    hprintf("Submitting current CR3 value to the hypervisor...");

    kAFL_hypercall(HYPERCALL_KAFL_SUBMIT_CR3, 0);

    hprintf("Beginning kAFL loop...");

    while(true)
    {
        hprintf("Reading payload...");

        kAFL_hypercall(HYPERCALL_KAFL_NEXT_PAYLOAD, 0);
        kAFL_hypercall(HYPERCALL_KAFL_ACQUIRE, 0);

        hprintf("Injecting payload data into: FuzzableMutex::Lock()...");

        FuzzableMutex::FuzzedLockInputs mutexLockPayload;

        memcpy(&mutexLockPayload, payload_buffer->data, sizeof(FuzzableMutex::FuzzedLockInputs));

        hprintf("Payload::Uaddr2_Ignored = %lx", mutexLockPayload.Uaddr2_Ignored);
        hprintf("Payload::Val3_Ignored = %lx", mutexLockPayload.Val3_Ignored);

        fuzzableMutex.Lock(mutexLockPayload);
    
        hprintf("Payload injection finished: FuzzableMutex::Lock()...");
        hprintf("Restoring the mutex state: FuzzableMutex::Unlock()...");

        FuzzableMutex::FuzzedUnlockInputs mutexUnlockPayload;

        memcpy(&mutexUnlockPayload, payload_buffer->data, sizeof(FuzzableMutex::FuzzedUnlockInputs));

        fuzzableMutex.Unlock(FuzzableMutex::FuzzedUnlockInputs{});

        hprintf("Read payload...");

        kAFL_hypercall(HYPERCALL_KAFL_RELEASE, 0);
	}

#else
        fuzzableMutex.Lock(FuzzableMutex::FuzzedLockInputs{});
        fuzzableMutex.Unlock(FuzzableMutex::FuzzedUnlockInputs{});
#endif

    return 0;
}
}