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
                virtual void execute() =0;
                bool isDone() const;
                void requestExecutionOnMainThread(MainThreadRequest* request);
                void setThreadManager(ThreadManager* mgr);
                std::atomic<bool> mainThreadRequestDone;
                std::mutex mutex0;
                std::mutex mutex1;
            protected:
                std::atomic<bool> done;
            private:
                ThreadManager* threadManager;
        };
        void requestExecutionOnMainThread(NewThreadRequest* newThread, MainThreadRequest* request);
        void requestExecutionOnNewThread(NewThreadRequest* request);
    private:
        struct NewThreadRequestData {
            NewThreadRequest* request;
            std::thread* thread;
            MainThreadRequest* mainThreadRequest;
        };
        std::vector<NewThreadRequestData> newThreadRequests;

        std::mutex requestMutex;

        std::thread::id mainThreadId;
};

}

#endif
