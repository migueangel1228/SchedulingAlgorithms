#include "Scheduler.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace utils {
std::string trim(const std::string& value) {
    const std::string whitespace = " \t\r\n";
    const size_t start = value.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    const size_t end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
}

std::vector<std::string> splitSemicolonLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    std::stringstream ss(line);
    while (getline(ss, field, ';')) {
        fields.push_back(trim(field));
    }
    return fields;
}

int parseIntField(const std::string& value, const std::string& fieldName, const std::string& filePath, int lineNumber) {
    try {
        size_t pos = 0;
        const int parsed = std::stoi(value, &pos);
        if (pos != value.size()) {
            throw std::invalid_argument("extra");
        }
        return parsed;
    } catch (const std::exception&) {
        throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) +
                            " invalid " + fieldName + " value: '" + value + "'");
    }
}

std::vector<Process> loadProcessesFromFile(const std::string& filePath) {
    std::ifstream input(filePath);
    if (!input.is_open()) {
        throw std::runtime_error("Could not open input file: " + filePath);
    }

    std::vector<Process> processes;
    std::string rawLine;
    int lineNumber = 0;

    while (getline(input, rawLine)) {
        ++lineNumber;
        const std::string line = trim(rawLine);

        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::vector<std::string> fields = splitSemicolonLine(line);
        if (fields.size() < 3 || fields.size() > 5) {
            throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) +
                                " expected 3 to 5 fields separated by ';'");
        }

        if (fields[0].empty()) {
            throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) + " process label cannot be empty");
        }

        std::string id = fields[0];
        int burst = parseIntField(fields[1], "burst", filePath, lineNumber);
        int arrival = parseIntField(fields[2], "arrival", filePath, lineNumber);
        int queue = fields.size() >= 4 ? parseIntField(fields[3], "queue", filePath, lineNumber) : 0;
        int priority = fields.size() >= 5 ? parseIntField(fields[4], "priority", filePath, lineNumber) : 0;
        
        if (burst <= 0) {
            throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) + " burst must be greater than 0");
        }
        if (arrival < 0) {
            throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) + " arrival must be greater than or equal to 0");
        }
        if (queue < 0) {
            throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) + " queue must be greater than or equal to 0");
        }
        if (priority < 0) {
            throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) + " priority must be greater than or equal to 0");
        }

        processes.emplace_back(id, burst, arrival, queue, priority, processes.size());
    }

    if (processes.empty()) {
        throw std::runtime_error("No valid processes found in file: " + filePath);
    }

    return processes;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " <input_file> [--verbose|-v] [-q N|--quantum N]\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " input_example1.txt\n";
    std::cout << "  " << programName << " input_example2.txt --verbose\n";
    std::cout << "  " << programName << " -q 4 input_example3.txt\n";
}
} // namespace utils

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            utils::printUsage(argv[0]);
            return 1;
        }

        bool verbose = false;
        int quantum = 3;
        std::string inputFile;

        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];

            if (arg == "--help") {
                utils::printUsage(argv[0]);
                return 0;
            }

            if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (arg == "--quantum" || arg == "-q") {
                if (i + 1 < argc) {
                    quantum = utils::parseIntField(argv[++i], "quantum", "CLI", 0);
                    if (quantum <= 0) {
                        throw std::runtime_error("Quantum must be greater than 0");
                    }
                } else {
                    throw std::runtime_error("Missing value for quantum option");
                }
            } else if (!arg.empty() && arg[0] == '-') {
                throw std::runtime_error("Unknown option: " + arg);
            } else {
                if (inputFile.empty()) {
                    inputFile = arg;
                } else {
                    throw std::runtime_error("Only one input file is supported");
                }
            }
        }

        if (inputFile.empty()) {
            utils::printUsage(argv[0]);
            return 1;
        }

        std::vector<Process> processes = utils::loadProcessesFromFile(inputFile);
        
        if (verbose) {
            std::cout << "Archivo: " << inputFile << '\n';
        }

        Scheduler scheduler(processes, quantum, verbose);
        scheduler.runAll();

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        std::cerr << "Use --help to see usage.\n";
        return 1;
    }
}
