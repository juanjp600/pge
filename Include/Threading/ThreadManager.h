#ifndef PGE_THREADMANAGER_H_INCLUDED
#define PGE_THREADMANAGER_H_INCLUDED

#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

namespace PGE {

class ThreadManager {
    public:
        ThreadManager();
        ~ThreadManager();

        void update();

        class MainThreadRequest {
            public:
                virtual void execute() =0;
        };
        
        class NewThreadRequest {
            public:
                ~NewThreadRequest();
                virtual void execute() =0;
                bool isDone() const;
                void requestExecutionOnMainThread(MainThreadRequest* request);
                void setThreadManager(ThreadManager* mgr);
                void startThread();
                bool isWaitingForMainThread();
                void executeMainThreadRequest();
            protected:
                void markAsDone();
            private:
                MainThreadRequest* mainThreadRequest;
                std::thread* thread;
                std::mutex mutex;
                std::condition_variable conditionVariable;
                std::atomic<bool> waitingForMainThread;
                ThreadManager* threadManager;
                std::atomic<bool> done;
        };
        void requestExecutionOnNewThread(NewThreadRequest* request);
    private:
        std::vector<NewThreadRequest*> newThreadRequests;

        std::mutex requestMutex;

        std::thread::id mainThreadId;
};

}

#endif
