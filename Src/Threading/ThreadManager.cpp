#include <Threading/ThreadManager.h>

#include <SDL.h>

#include <chrono>

using namespace PGE;

ThreadManager::ThreadManager() {
    mainThreadId = std::this_thread::get_id();
}

ThreadManager::~ThreadManager() {
    while (newThreadRequests.size()>0) {
        update();
    }
}

ThreadManager::NewThreadRequest::~NewThreadRequest() {
    thread->join(); delete thread;
}

bool ThreadManager::NewThreadRequest::isDone() const {
    return done;
}

void ThreadManager::NewThreadRequest::requestExecutionOnMainThread(MainThreadRequest* request) {
    mainThreadRequest = request;
    waitingForMainThread = true;

    while (waitingForMainThread) { std::this_thread::yield(); }
}

void ThreadManager::NewThreadRequest::setThreadManager(ThreadManager* mgr) {
    done = false;
    threadManager = mgr;
}

static void _startThread(ThreadManager::NewThreadRequest* request) {
    request->execute();
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
}

void ThreadManager::NewThreadRequest::markAsDone() {
    done = true;
}

void ThreadManager::requestExecutionOnNewThread(NewThreadRequest* request) {
    request->setThreadManager(this);
    request->startThread();

    newThreadRequests.push_back(request);
}

void ThreadManager::update() {
    requestMutex.lock();
    for (int i=0;i<newThreadRequests.size();i++) {
        if (newThreadRequests[i]->isWaitingForMainThread()) {
            newThreadRequests[i]->executeMainThreadRequest();
        }
        if (newThreadRequests[i]->isDone()) {
            delete newThreadRequests[i];
            newThreadRequests.erase(newThreadRequests.begin()+i);
            i--;
        }
    }
    requestMutex.unlock();
}
