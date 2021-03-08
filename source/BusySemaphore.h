//
// Created by lire on 3/7/21.
//

#ifndef SDP_PARALLELSOLUTION_BUSYSEMAPHORE_H
#define SDP_PARALLELSOLUTION_BUSYSEMAPHORE_H

#include <atomic>

class BusySemaphore
{
public:
    BusySemaphore(bool statoIniziale) { lock = statoIniziale; };
    void signal() { lock.store(false); }
    void wait() { while(lock.exchange(true)) {} }
    void designal() { lock.store(true); }

private:
    std::atomic<bool> lock;
};

#endif //SDP_PARALLELSOLUTION_BUSYSEMAPHORE_H
