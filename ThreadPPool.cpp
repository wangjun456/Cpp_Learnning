#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    // 线程池中的工作线程
    std::vector<std::thread> workers;

    // 任务队列
    std::queue<std::function<void()>> tasks;

    // 互斥锁和条件变量，用于线程间的同步
    std::mutex queueMutex;
    std::condition_variable condition;

    // 是否关闭线程池
    bool stop;
};

// 构造函数，创建指定数量的工作线程
inline ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    // 使用互斥锁保护任务队列
                    std::unique_lock<std::mutex> lock(this->queueMutex);

                    // 等待条件变量通知或线程池关闭
                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });

                    // 如果线程池关闭且任务队列为空，则线程退出
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }

                    // 从队列中取出一个任务
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                // 执行任务
                task();
            }
        });
    }
}

// 析构函数，等待所有线程执行完毕，然后关闭线程池
inline ThreadPool::~ThreadPool() {
    {
        // 使用互斥锁保护任务队列
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }

    // 通知所有线程可以退出
    condition.notify_all();

    // 等待所有线程执行完毕
    for (std::thread& worker : workers) {
        worker.join();
    }
}

// 向线程池中添加任务
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    // 创建一个 packaged_task 封装函数和参数
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    // 获取任务的 future
    std::future<return_type> result = task->get_future();

    {
        // 使用互斥锁保护任务队列
        std::unique_lock<std::mutex> lock(queueMutex);

        // 如果线程池已关闭，则抛出异常
        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        // 将任务添加到队列中
        tasks.emplace([task]() {
            (*task)();
        });
    }

    // 通知一个工作线程取出任务执行
    condition.notify_one();

    return result;
}

// 示例用法
int main() {
    ThreadPool pool(4);

    // 向线程池添加任务
    for(int i = 0;i < 1000; i++)
    {
        auto result1 = pool.enqueue([](int x, int y) { return x + y; }, 2, 3);
        std::cout << "Result1: " << result1.get() << std::endl;
    }

    return 0;
}
