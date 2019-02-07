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

bool ThreadManager::NewThreadRequest::isDone() const {
    return done;
}

void ThreadManager::NewThreadRequest::requestExecutionOnMainThread(MainThreadRequest* request) {
    threadManager->requestExecutionOnMainThread(this, request);
}

void ThreadManager::NewThreadRequest::setThreadManager(ThreadManager* mgr) {
    done = false;
    threadManager = mgr;
}

void ThreadManager::requestExecutionOnMainThread(NewThreadRequest* newThread, MainThreadRequest* request) {
    requestMutex.lock();
    for (int i=0;i<newThreadRequests.size();i++) {
        if (newThreadRequests[i].request == newThread) {
            //newThread->mainThreadRequestDone = false;
            newThreadRequests[i].mainThreadRequest = request;
            break;
        }
    }
    newThread->mutex1.lock();
    requestMutex.unlock();

    newThread->mutex0.lock();
    newThread->mutex1.unlock();
    newThread->mutex0.unlock();
}

static void startThread(ThreadManager::NewThreadRequest* request) {
    request->execute();
}

void ThreadManager::requestExecutionOnNewThread(NewThreadRequest* request) {
    request->setThreadManager(this);
    NewThreadRequestData requestData;
    requestData.request = request;

    request->mutex0.lock();
    requestData.mainThreadRequest = nullptr;
    requestData.thread = new std::thread(startThread, request);
    newThreadRequests.push_back(requestData);
}

void ThreadManager::update() {
    requestMutex.lock();
    for (int i=0;i<newThreadRequests.size();i++) {
        if (newThreadRequests[i].mainThreadRequest != nullptr) {
            newThreadRequests[i].mainThreadRequest->execute();
            newThreadRequests[i].mainThreadRequest = nullptr;
            //tell other thread that the task is done
            newThreadRequests[i].request->mutex0.unlock();
            newThreadRequests[i].request->mutex1.lock();
            newThreadRequests[i].request->mutex0.lock();
            newThreadRequests[i].request->mutex1.unlock();
        }
        if (newThreadRequests[i].request->isDone()) {
            newThreadRequests[i].request->mutex0.unlock();
            newThreadRequests[i].thread->join();
            delete newThreadRequests[i].thread;
            delete newThreadRequests[i].request;
            newThreadRequests.erase(newThreadRequests.begin()+i);
            i--;
        }
    }
    requestMutex.unlock();
}
