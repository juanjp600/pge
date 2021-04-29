#ifndef PGE_THREADMANAGER_H_INCLUDED
#define PGE_THREADMANAGER_H_INCLUDED

#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

#include <String/String.h>
#include <Exception/Exception.h>

namespace PGE {

class ThreadManager {
    public:
        ThreadManager();
        ~ThreadManager();

        void update();

        class MainThreadRequest {
            public:
                virtual void execute() = 0;
        };

        class NewThreadRequest {
            public:
                NewThreadRequest();
                virtual ~NewThreadRequest();

                //execution
                virtual void execute() = 0;
                bool isDone() const;

                //exception handling
                bool wasExceptionThrown() const;
                void notifyException();

                //methods called by threadmanager
                void setThreadManager(ThreadManager* mgr);
                void startThread();

                //requests for main thread
                void requestExecutionOnMainThread(MainThreadRequest* request);
                bool isWaitingForMainThread();
                void executeMainThreadRequest();

            protected:
                void markAsDone();

            private:
                ThreadManager* threadManager;

                MainThreadRequest* mainThreadRequest;
                std::thread* thread;

                std::mutex condVarMutex;
                std::condition_variable conditionVariable;

                std::atomic<bool> waitingForMainThread;
                std::atomic<bool> done;
                std::atomic<bool> exceptionThrown;
        };

        void requestExecutionOnNewThread(NewThreadRequest* request);
        void handleException(NewThreadRequest* request, Exception& e);

    private:
        std::vector<NewThreadRequest*> newThreadRequests;

        std::mutex requestMutex;

        std::mutex exceptionMutex;
        struct ExceptionData {
            ThreadManager::NewThreadRequest* request;
            Exception exception;
        };
        std::vector<ExceptionData> thrownExceptions;

        std::thread::id mainThreadId;
};

}

#endif // PGE_THREADMANAGER_H_INCLUDED
