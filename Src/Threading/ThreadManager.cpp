#include <Threading/ThreadManager.h>
#include "../Exception/Exception.h"

#include <chrono>

using namespace PGE;

struct ExceptionData {
    ThreadManager::NewThreadRequest* request;
    Exception exception;
};
static std::vector<ExceptionData> thrownExceptions;
static std::mutex exceptionMutex;

ThreadManager::ThreadManager() {
    mainThreadId = std::this_thread::get_id();
}

ThreadManager::~ThreadManager() {
    requestMutex.lock();
    int reqSize = (int)newThreadRequests.size();
    requestMutex.unlock();
    while (reqSize>0) {
        update();
        requestMutex.lock();
        reqSize = (int)newThreadRequests.size();
        requestMutex.unlock();
    }
}

ThreadManager::NewThreadRequest::NewThreadRequest() {
    waitingForMainThread = false;
    done = false;
}

ThreadManager::NewThreadRequest::~NewThreadRequest() {
    thread->join(); delete thread;
}

bool ThreadManager::NewThreadRequest::isDone() const {
    return done;
}

bool ThreadManager::NewThreadRequest::wasExceptionThrown() const {
    return exceptionThrown;
}

void ThreadManager::NewThreadRequest::notifyException() {
    exceptionThrown = true;
}

void ThreadManager::NewThreadRequest::requestExecutionOnMainThread(MainThreadRequest* request) {
    mainThreadRequest = request;
    std::unique_lock<std::mutex> lock(mutex);
    waitingForMainThread = true;
    conditionVariable.wait(lock);
}

void ThreadManager::NewThreadRequest::setThreadManager(ThreadManager* mgr) {
    done = false;
    threadManager = mgr;
}

static void _startThread(ThreadManager::NewThreadRequest* request) {
    try {
        request->execute();
    } catch (Exception& e) {
        exceptionMutex.lock();
        ExceptionData exceptionData;
        exceptionData.request = request;
        exceptionData.exception = e;
        thrownExceptions.push_back(exceptionData);
        request->notifyException();
        exceptionMutex.unlock();
    }
}

void ThreadManager::NewThreadRequest::startThread() {
    mainThreadRequest = nullptr;
    thread = new std::thread(_startThread, this);
}

bool ThreadManager::NewThreadRequest::isWaitingForMainThread() {
    return waitingForMainThread;
}

void ThreadManager::NewThreadRequest::executeMainThreadRequest() {
    mainThreadRequest->execute();
    waitingForMainThread = false;
    conditionVariable.notify_all();
}

void ThreadManager::NewThreadRequest::markAsDone() {
    done = true;
}

void ThreadManager::requestExecutionOnNewThread(NewThreadRequest* request) {
    request->setThreadManager(this);
    request->startThread();

    requestMutex.lock();
    newThreadRequests.push_back(request);
    requestMutex.unlock();
}

void ThreadManager::update() {
    requestMutex.lock();
    for (int i=0;i<newThreadRequests.size();i++) {
        if (newThreadRequests[i]->isWaitingForMainThread()) {
            newThreadRequests[i]->executeMainThreadRequest();
        }
        if (newThreadRequests[i]->wasExceptionThrown()) {
            exceptionMutex.lock();
            for (int j = 0; j < thrownExceptions.size(); j++) {
                if (thrownExceptions[j].request == newThreadRequests[i]) {
                    delete newThreadRequests[i];
                    newThreadRequests.erase(newThreadRequests.begin() + i);
                    i--;
                    throw thrownExceptions[j].exception;
                }
            }
            exceptionMutex.unlock();
        } else if (newThreadRequests[i]->isDone()) {
            delete newThreadRequests[i];
            newThreadRequests.erase(newThreadRequests.begin()+i);
            i--;
        }
        
    }
    requestMutex.unlock();
}
