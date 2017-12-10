#ifndef THREAD_POOL_EXECUTORSERVICE_H
#define THREAD_POOL_EXECUTORSERVICE_H

#include <type_traits>
#include <future>
#include <vector>

class ExecutorService {
protected:
    template <typename FunctionType>
    using funcResultType = typename std::result_of_t<FunctionType()>;

public:
    virtual ~ExecutorService() = 0;

    template <typename FunctionType>
    virtual std::future<funcResultType<FunctionType>> submit(FunctionType function) = 0;

    virtual void shutdown() = 0;
    virtual bool isShutdown() = 0;
    virtual bool isTerminated() = 0;

    template <typename FunctionType>
    virtual std::vector<std::future<funcResultType<FunctionType>>> invokeAll(std::vector<FunctionType> funcList) = 0;
};

#endif //THREAD_POOL_EXECUTORSERVICE_H
