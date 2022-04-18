#pragma once

// C/C++ Includes
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <atomic>
#include <stdexcept>
#include <iostream>

// Linux Includes
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/futex.h>

namespace cpe631
{
class FuzzableMutex
{
public:
    // As per the Linux API, the third argument of the Futex system call is interpreted as a timespec structure pointer
    // or uint32 value depending on the operation being performed.
    // We define a union here to aid the developers in enterpreting which version of the argument is used for each specific operation.

    union TimeoutArgument
    {
        TimeoutArgument() noexcept { memset(this, 0u, sizeof(*this)); };

        timespec* timeout;
        uint32_t val2;
    };

    struct FuzzedLockInputs
    {
        FuzzedLockInputs() noexcept { memset(this, 0u, sizeof(*this)); };

        uint32_t* Uaddr2_Ignored;
        uint32_t Val3_Ignored;
    };

    struct FuzzedUnlockInputs
    {
        FuzzedUnlockInputs() noexcept { memset(this, 0u, sizeof(*this)); };

        TimeoutArgument Timeout_Ignored;
        uint32_t* Uaddr2_Ignored;
        uint32_t Val3_Ignored;
    };

    struct FuzzedAtomicStateInputs
    {
        FuzzedAtomicStateInputs() noexcept { memset(this, 0u, sizeof(*this)); };

        uint32_t atomicState;
    };

    void SetAtomicState(const FuzzedAtomicStateInputs& fuzzedInput) noexcept { atomicState_ = fuzzedInput.atomicState; }

    void Lock(const FuzzedLockInputs& fuzzedInput)
    {
        uint32_t previoudFutexState;

        // If the previous state was UNLOCKED, update to LOCKED_NO_WAITERS and proceed with normal execution.

        if ((previoudFutexState = AtomicCompareAndExchange(UNLOCKED_, LOCKED_NO_WAITERS_)) != UNLOCKED_)
        {
            do
            {
                // If the previous state was LOCKED_ONE_OR_MORE_WAITERS, sleep the thread.
                // If the current state is LOCKED_NO_WAITERS, update to LOCKED_ONE_OR_MORE_WAITERS and sleep the thread.
                // If the current state is UNLOCKED, proceed with normal execution.

                if (previoudFutexState == LOCKED_ONE_OR_MORE_WAITERS_ || (AtomicCompareAndExchange(LOCKED_NO_WAITERS_, LOCKED_ONE_OR_MORE_WAITERS_) != UNLOCKED_))
                {
                    TimeoutArgument timeoutArgument;

                    timeoutArgument.timeout = nullptr;

                    const long int result{
                        FutexSystemCall(
                            reinterpret_cast<uint32_t*>(&atomicState_), // Futex word pointer.
                            FUTEX_WAIT,                                 // Futex operation to perform.
                            LOCKED_ONE_OR_MORE_WAITERS_,                // Expected futex value.
                            timeoutArgument,                            // Fuzzed timeout input.
                            fuzzedInput.Uaddr2_Ignored,                 // Fuzzed ignored input,
                            fuzzedInput.Val3_Ignored)};                 // Fuzzed ignored input.

                    switch(result)
                    {
                        case -1:

                            // As per the Linux Futex API, a -1 indicates an error and updates errno to indicate the error code.

                            // As per the Linux API, an error code of EAGAIN could indicate a spurious wakeup.
                            // In order to not lose a wakeup, the FUTEX_WAIT system call must be reatempted in order to sleep the thread.
                            // If the error code is not equal to EAGAIN, an exception is thrown to indicate abnormal behavior.
                            
                            if (errno != EAGAIN)
                                throw std::runtime_error{std::string{"Lock() : An error occured during the FUTEX_WAIT system call, errno = "} + strerror(errno) + "..."};

                        case 0: // Intentional fallthrough in order to continue with normal execution.
                            // As per the Linux Futex API, a 0 indicates that the thread was woken.

                            break;

                        default:
                            // As per the Linux Futex API, the FUTEX_WAIT system call should never return a value other than 0 or -1.

                            throw std::runtime_error{"Lock() : An unexpected value was returned by the FUTEX_WAIT system call..."};
                    }
                }
            
                // If the previous state was UNLOCKED, update to LOCKED_ONE_OR_MORE_WAITERS and proceed with normal execution.
                // If the previous state was UNLOCKED, repeat until the lock is released.

            } while ((previoudFutexState = AtomicCompareAndExchange(UNLOCKED_, LOCKED_ONE_OR_MORE_WAITERS_)) != UNLOCKED_);
        }
    }

    void Unlock(const FuzzedUnlockInputs& fuzzedInput)
    {
        if (atomicState_.load() == UNLOCKED_)
            throw std::runtime_error{"Unlock() : Unlock cannot be called since the mutex is already unlocked..."};

        // If the previous state was not LOCKED_NO_WAITERS, update to UNLOCKED and wake one of the sleeping threads.

        if (atomicState_.fetch_sub(1u) != LOCKED_NO_WAITERS_)
        {
            atomicState_ = UNLOCKED_;

            const long int result{
                FutexSystemCall(
                    reinterpret_cast<uint32_t*>(&atomicState_), // Futex word pointer.
                    FUTEX_WAKE,                                 // Futex operation to perform.
                    WAKE_SINGLE_THREAD_,                        // Expected futex value.
                    fuzzedInput.Timeout_Ignored,                // Fuzzed timeout input.
                    fuzzedInput.Uaddr2_Ignored,                 // Fuzzed ignored input,
                    fuzzedInput.Val3_Ignored)};                 // Fuzzed ignored input.

            switch(result)
            {
                case -1:

                    // As per the Linux Futex API, a -1 indicates an error and updates errno to indicate the error code.

                    throw std::runtime_error{std::string{"Lock() : An error occured during the FUTEX_WAKE system call, errno = "} + strerror(errno) + "..."};

                case 1:
                    // One thread was woken.

                case 0: // Intentional fallthrough
                    // No threads were woken

                    return;
                default:
                    throw std::runtime_error{"Lock() : Multiple threads were woken by the FUTEX_WAKE system call, but the function was instructed to wake only a single thread..."};

            }
    
        }
    }

protected:
private:
    uint32_t AtomicCompareAndExchange(uint32_t expectedValue, const uint32_t desiredValue) noexcept
    {
        // Perform atomic operation until successful

        while(!std::atomic_compare_exchange_strong(&atomicState_, &expectedValue, desiredValue))
            break;
        
        return expectedValue;
    }

    long int FutexSystemCall(
        uint32_t* uaddr,
        int futexOp,
        uint32_t val,
        TimeoutArgument timeout,
        uint32_t* uaddr2,
        uint32_t val3) noexcept
        {
            return syscall(
                SYS_futex, // Required - As per the Linux System Call API, this identifies the system call that is to be performed.
                uaddr,     // Required - As per the Linux Futex API, this is a pointer to the first futex word.
                futexOp,   // Required - As per the Linux Futex API, this identifies the futex operation that is to be performed.
                val,       // Required - As per the Linux Futex API, the value and meaning of this variable are operation-dependent.
                timeout,   // Optional - As per the Linux Futex API, this is a pointer to a timeout structure or integer depending on the operation being performed.
                uaddr2,    // Optional - As per the Linux Futex API, this is a pointer to the second futex word.
                val3);     // Optional - As per the Linux Futex API, the value and meaning of this variable are operation-dependent.
        }

    static constexpr uint32_t WAKE_SINGLE_THREAD_{1u};
    static constexpr uint32_t UNLOCKED_{0u};
    static constexpr uint32_t LOCKED_NO_WAITERS_{1u};
    static constexpr uint32_t LOCKED_ONE_OR_MORE_WAITERS_{2u};

    std::atomic_uint32_t atomicState_{UNLOCKED_};
};
} 