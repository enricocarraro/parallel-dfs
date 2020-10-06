//
// Created by lire on 10/4/20.
//

#include "BusySemaphore.h"

BusySemaphore::BusySemaphore() noexcept: lock(false) {}

void BusySemaphore::signal() {
    lock.store(false);
}

void BusySemaphore::wait() {
    while(lock.exchange(true)) {}
}

