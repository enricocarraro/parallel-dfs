//
//  ThreadWorker.cpp
//  parallel-dfs
//
//  Created by Enrico on 22/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#include "ThreadWorker.hpp"

void ThreadWorker::processTasks() {
    while(true) {
        auto m = queue.pop();
        m();
    }
}

void ThreadWorker::addTask(const std::function<void()>& task) {
    queue.push(task);
}
