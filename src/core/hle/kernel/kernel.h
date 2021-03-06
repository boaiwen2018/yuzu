// Copyright 2014 Citra Emulator Project / PPSSPP Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include "core/hle/kernel/object.h"

template <typename T>
class ResultVal;

namespace CoreTiming {
struct EventType;
}

namespace Kernel {

class HandleTable;
class Process;
class ResourceLimit;
class Thread;
class Timer;

enum class ResourceLimitCategory : u8;

/// Represents a single instance of the kernel.
class KernelCore {
public:
    KernelCore();
    ~KernelCore();

    KernelCore(const KernelCore&) = delete;
    KernelCore& operator=(const KernelCore&) = delete;

    KernelCore(KernelCore&&) = delete;
    KernelCore& operator=(KernelCore&&) = delete;

    /// Resets the kernel to a clean slate for use.
    void Initialize();

    /// Clears all resources in use by the kernel instance.
    void Shutdown();

    /// Provides a reference to the handle table.
    Kernel::HandleTable& HandleTable();

    /// Provides a const reference to the handle table.
    const Kernel::HandleTable& HandleTable() const;

    /// Retrieves a shared pointer to a ResourceLimit identified by the given category.
    SharedPtr<ResourceLimit> ResourceLimitForCategory(ResourceLimitCategory category) const;

    /// Retrieves a shared pointer to a Thread instance within the thread wakeup handle table.
    SharedPtr<Thread> RetrieveThreadFromWakeupCallbackHandleTable(Handle handle) const;

    /// Retrieves a shared pointer to a Timer instance within the timer callback handle table.
    SharedPtr<Timer> RetrieveTimerFromCallbackHandleTable(Handle handle) const;

    /// Adds the given shared pointer to an internal list of active processes.
    void AppendNewProcess(SharedPtr<Process> process);

private:
    friend class Object;
    friend class Process;
    friend class Thread;
    friend class Timer;

    /// Creates a new object ID, incrementing the internal object ID counter.
    u32 CreateNewObjectID();

    /// Creates a new process ID, incrementing the internal process ID counter;
    u32 CreateNewProcessID();

    /// Creates a new thread ID, incrementing the internal thread ID counter.
    u32 CreateNewThreadID();

    /// Creates a timer callback handle for the given timer.
    ResultVal<Handle> CreateTimerCallbackHandle(const SharedPtr<Timer>& timer);

    /// Retrieves the event type used for thread wakeup callbacks.
    CoreTiming::EventType* ThreadWakeupCallbackEventType() const;

    /// Retrieves the event type used for timer callbacks.
    CoreTiming::EventType* TimerCallbackEventType() const;

    /// Provides a reference to the thread wakeup callback handle table.
    Kernel::HandleTable& ThreadWakeupCallbackHandleTable();

    /// Provides a const reference to the thread wakeup callback handle table.
    const Kernel::HandleTable& ThreadWakeupCallbackHandleTable() const;

    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Kernel
