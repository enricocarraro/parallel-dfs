//
//  FastSemaphore.hpp
//  parallel-dfs
//
//  Created by Enrico on 13/09/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#ifndef FastSemaphore_hpp
#define FastSemaphore_hpp

#include "Semaphore.hpp"
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <cassert>

class FastSemaphore
{
public:
        FastSemaphore() noexcept;
        void signal();
        void wait();

private:
        std::atomic<int> m_count;
        Semaphore m_semaphore;
};

#endif /* FastSemaphore_hpp */
