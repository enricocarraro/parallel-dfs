//
//  ThreadWorker.hpp
//  parallel-dfs
//
//  Created by Enrico on 22/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#ifndef ThreadWorker_hpp
#define ThreadWorker_hpp


#include <functional>
#include "SafeQueue.cpp"

class ThreadWorker {
        SafeQueue<std::function<void()>> queue;
public:
        void addTask(const std::function<void()>& task);
        void processTasks();
};

#endif /* ThreadWorker_hpp */
