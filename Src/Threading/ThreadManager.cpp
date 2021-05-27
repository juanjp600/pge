#include <PGE/Threading/ThreadManager.h>

#include <chrono>

using namespace PGE;

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
    std::unique_lock<std::mutex> lock(condVarMutex);
    waitingForMainThread = true;
    conditionVariable.wait(lock);
}

void ThreadManager::NewThreadRequest::setThreadManager(ThreadManager* mgr) {
    done = false;
    threadManager = mgr;
}

static void _startThread(ThreadManager::NewThreadRequest* request, ThreadManager* threadManager) {
    try {
        request->execute();
    } catch (const Exception& e) {
        threadManager->handleException(request, e);
    }
}

void ThreadManager::NewThreadRequest::startThread() {
    mainThreadRequest = nullptr;
    thread = new std::thread(_startThread, this, threadManager);
}

void ThreadManager::handleException(ThreadManager::NewThreadRequest* request, const Exception& e) {
    exceptionMutex.lock();
    ExceptionData data;
    data.request = request;
    data.exception = e;
    thrownExceptions.push_back(data);
    request->notifyException();
    exceptionMutex.unlock();
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
    for (int i = 0; i < (int)newThreadRequests.size(); i++) {
        if (newThreadRequests[i]->isWaitingForMainThread()) {
            newThreadRequests[i]->executeMainThreadRequest();
        }
        if (newThreadRequests[i]->wasExceptionThrown()) {
            exceptionMutex.lock();
            for (int j = 0; j < (int)thrownExceptions.size(); j++) {
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
