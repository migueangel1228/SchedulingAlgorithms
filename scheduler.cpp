#include "Scheduler.h"
#include <algorithm>
#include <queue>
#include <iomanip>

Process::Process(std::string pid, int b, int a, int q, int p, int order)
    : id(pid), burst(b), arrival(a), queue(q), priority(p),
      remaining(b), inputOrder(order), completionTime(0), startTime(-1) {}

Scheduler::Scheduler(const std::vector<Process>& procs, int q, bool v)
    : processes(procs), quantum(q), verbose(v) {}

void Scheduler::runAll() {
    if (verbose) {
        std::cout << "Procesos cargados: " << processes.size() << "\n\n";
    }

    printMetrics(fcfs(), "FCFS");
    std::cout << '\n';
    printMetrics(sjf(), "SJF");
    std::cout << '\n';
    printMetrics(stcf(), "STCF");
    std::cout << '\n';
    printMetrics(roundRobin(), "Round Robin");
}

Metrics Scheduler::calculateMetrics(const std::vector<Process>& finishedProcesses) {
    long long totalTurnaround = 0;
    long long totalWaiting = 0;
    for (const auto& p : finishedProcesses) {
        int turnaround = p.completionTime - p.arrival;
        totalTurnaround += turnaround;
        totalWaiting += turnaround - p.burst;
    }
    return {
        static_cast<double>(totalTurnaround) / finishedProcesses.size(),
        static_cast<double>(totalWaiting) / finishedProcesses.size()
    };
}

void Scheduler::printMetrics(const Metrics& metrics, const std::string& algorithmName) {
    std::cout << algorithmName << ":\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Turnaround avg: " << metrics.turnaroundAvg << '\n';
    std::cout << "Waiting avg: " << metrics.waitingAvg << '\n';
    std::cout.unsetf(std::ios::floatfield);
    std::cout << std::setprecision(6);
}

bool Scheduler::compareByArrival(const Process& a, const Process& b) {
    if (a.arrival != b.arrival) return a.arrival < b.arrival;
    if (a.queue != b.queue) return a.queue < b.queue;
    if (a.priority != b.priority) return a.priority < b.priority;
    return a.inputOrder < b.inputOrder;
}

Metrics Scheduler::fcfs() {
    std::vector<Process> procs = processes;
    std::sort(procs.begin(), procs.end(), compareByArrival);
    std::vector<Process> finishedProcs;
    int time = 0;

    for (auto& p : procs) {
        if (time < p.arrival) {
            time = p.arrival;
        }
        p.startTime = time;
        p.completionTime = time + p.burst;
        std::cout << p.id << "  Desde: " << p.startTime << "  Hasta: " << p.completionTime << '\n';
        time = p.completionTime;
        finishedProcs.push_back(p);
    }
    return calculateMetrics(finishedProcs);
}

bool Scheduler::isBetterSjfCandidate(const Process& candidate, const Process& current) {
    if (candidate.burst != current.burst) return candidate.burst < current.burst;
    if (candidate.arrival != current.arrival) return candidate.arrival < current.arrival;
    if (candidate.queue != current.queue) return candidate.queue < current.queue;
    if (candidate.priority != current.priority) return candidate.priority < current.priority;
    return candidate.inputOrder < current.inputOrder;
}

Metrics Scheduler::sjf() {
    std::vector<Process> procs = processes;
    std::vector<Process> finishedProcs;
    int time = 0;
    int completed = 0;
    const int n = procs.size();
    std::vector<bool> done(n, false);

    while (completed < n) {
        int best_idx = -1;
        for (int i = 0; i < n; ++i) {
            if (!done[i] && procs[i].arrival <= time) {
                if (best_idx == -1 || isBetterSjfCandidate(procs[i], procs[best_idx])) {
                    best_idx = i;
                }
            }
        }

        if (best_idx != -1) {
            procs[best_idx].startTime = time;
            procs[best_idx].completionTime = time + procs[best_idx].burst;
            std::cout << procs[best_idx].id << "  Desde: " << procs[best_idx].startTime << "  Hasta: " << procs[best_idx].completionTime << '\n';
            time = procs[best_idx].completionTime;
            done[best_idx] = true;
            finishedProcs.push_back(procs[best_idx]);
            completed++;
        } else {
            time++;
        }
    }
    return calculateMetrics(finishedProcs);
}

bool Scheduler::isBetterStcfCandidate(const Process& candidate, const Process& current) {
    if (candidate.remaining != current.remaining) return candidate.remaining < current.remaining;
    if (candidate.arrival != current.arrival) return candidate.arrival < current.arrival;
    if (candidate.queue != current.queue) return candidate.queue < current.queue;
    if (candidate.priority != current.priority) return candidate.priority < current.priority;
    return candidate.inputOrder < current.inputOrder;
}

Metrics Scheduler::stcf() {
    std::vector<Process> procs = processes;
    std::vector<Process> finishedProcs;
    int time = 0;
    int completed = 0;
    const int n = procs.size();
    int last_idx = -1;
    int segment_start = 0;

    while (completed < n) {
        int best_idx = -1;
        for (int i = 0; i < n; ++i) {
            if (procs[i].remaining > 0 && procs[i].arrival <= time) {
                if (best_idx == -1 || isBetterStcfCandidate(procs[i], procs[best_idx])) {
                    best_idx = i;
                }
            }
        }

        if (best_idx != -1) {
            if (last_idx != best_idx) {
                if (last_idx != -1) {
                    std::cout << procs[last_idx].id << "  Desde: " << segment_start << "  Hasta: " << time << '\n';
                }
                segment_start = time;
                last_idx = best_idx;
            }

            procs[best_idx].remaining--;
            if (procs[best_idx].remaining == 0) {
                procs[best_idx].completionTime = time + 1;
                finishedProcs.push_back(procs[best_idx]);
                completed++;
                std::cout << procs[best_idx].id << "  Desde: " << segment_start << "  Hasta: " << time + 1 << '\n';
                last_idx = -1;
            }
        } else {
             if (last_idx != -1) {
                std::cout << procs[last_idx].id << "  Desde: " << segment_start << "  Hasta: " << time << '\n';
                last_idx = -1;
            }
        }
        time++;
    }
     std::sort(finishedProcs.begin(), finishedProcs.end(), [](const Process& a, const Process& b){
        return a.inputOrder < b.inputOrder;
    });
    return calculateMetrics(finishedProcs);
}

void Scheduler::enqueueArrivals(const std::vector<Process>& p, std::vector<bool>& added, std::queue<int>& q, int time) {
    for (int i = 0; i < static_cast<int>(p.size()); ++i) {
        if (!added[i] && p[i].arrival <= time) {
            q.push(i);
            added[i] = true;
        }
    }
}

Metrics Scheduler::roundRobin() {
    std::vector<Process> procs = processes;
    std::vector<Process> finishedProcs;
    std::queue<int> q;
    int time = 0;
    const int n = procs.size();
    std::vector<bool> added(n, false);
    
    std::sort(procs.begin(), procs.end(), compareByArrival);

    int completed_count = 0;
    while(completed_count < n) {
        enqueueArrivals(procs, added, q, time);
        if (!q.empty()) {
            int idx = q.front();
            q.pop();

            int exec_time = std::min(quantum, procs[idx].remaining);
            std::cout << procs[idx].id << "  Desde: " << time << "  Hasta: " << time + exec_time << '\n';
            procs[idx].remaining -= exec_time;
            time += exec_time;

            enqueueArrivals(procs, added, q, time);

            if (procs[idx].remaining > 0) {
                q.push(idx);
            } else {
                procs[idx].completionTime = time;
                finishedProcs.push_back(procs[idx]);
                completed_count++;
            }
        } else {
            time++;
        }
    }
     std::sort(finishedProcs.begin(), finishedProcs.end(), [](const Process& a, const Process& b){
        return a.inputOrder < b.inputOrder;
    });
    return calculateMetrics(finishedProcs);
}
