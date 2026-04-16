#pragma once

#include <condition_variable>
#include <queue>
#include <mutex>
#include <thread>
#include <functional>

class threadPoolClass {
    private:

    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable cv;
    bool stop = false;
    int activeTasks = 0;
    std::condition_variable waitCv;

    public:

    threadPoolClass() {
        int threadCount = std::thread::hardware_concurrency();

        for (int i = 0; i < threadCount; i++) {

            threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        cv.wait(lock, [this] {
                            return !tasks.empty() || stop;
                        });

                        if (stop && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    ~threadPoolClass() {

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }

        cv.notify_all();

        for (auto& thread : threads) {
            thread.join();
        }
    }

    void enqueue(std::function<void()> task) {

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace([this, task]() {
                task();
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    activeTasks--;
                    waitCv.notify_one();
                }
            });
            activeTasks++;
        }

        cv.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(queueMutex);
        waitCv.wait(lock, [this] {
            return activeTasks == 0;
        });
    }
};