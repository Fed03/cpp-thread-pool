#ifndef THREAD_POOL_TASK_H
#define THREAD_POOL_TASK_H

class FuncWrapper {
    struct baseImplementation {
        virtual void exec() = 0;
        virtual ~baseImplementation() = default;
    };

    template <typename Func>
    struct implementation : baseImplementation {
        Func f;

        explicit implementation(Func&& f_) : f(std::move(f_)) {}
        void exec() override {
            f();
        }
    };

    std::unique_ptr<baseImplementation> impl;

public:
    FuncWrapper() = default;

    FuncWrapper(FuncWrapper&& other) : impl(std::move(other.impl)) {}
    FuncWrapper& operator=(FuncWrapper&& other) {
        impl = std::move(other.impl);
        return *this;
    }

    FuncWrapper(const FuncWrapper&) = delete;
    FuncWrapper& operator=(const FuncWrapper&) = delete;
    FuncWrapper(FuncWrapper&) = delete;

    template <typename Func>
    FuncWrapper(Func&& f_) : impl(new implementation<Func>(std::move(f_))) {}

    void operator()() {
        impl->exec();
    }
};

#endif //THREAD_POOL_TASK_H
