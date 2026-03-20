#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <vector>
#include <string>
#include <queue>

class Process {
public:
    std::string id;
    int burst;
    int arrival;
    int queue;
    int priority;
    int remaining;
    int inputOrder;
    int completionTime;
    int startTime;

    Process(std::string id, int burst, int arrival, int queue, int priority, int order);
};

struct Metrics {
    double turnaroundAvg = 0.0;
    double waitingAvg = 0.0;
};

class Scheduler {
private:
    std::vector<Process> processes;
    int quantum;
    bool verbose;

    Metrics calculateMetrics(const std::vector<Process>& finishedProcesses);
    void printMetrics(const Metrics& metrics, const std::string& algorithmName);
    static bool compareByArrival(const Process& a, const Process& b);
    static bool isBetterSjfCandidate(const Process& candidate, const Process& current);
    static bool isBetterStcfCandidate(const Process& candidate, const Process& current);
    void enqueueArrivals(const std::vector<Process>& p, std::vector<bool>& added, std::queue<int>& q, int time);


public:
    Scheduler(const std::vector<Process>& procs, int q = 3, bool v = false);
    void runAll();
    Metrics fcfs();
    Metrics sjf();
    Metrics stcf();
    Metrics roundRobin();
};

#endif // SCHEDULER_H
