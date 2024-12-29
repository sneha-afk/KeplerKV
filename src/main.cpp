#include "environment.h"
#include "handler.h"
#include "terminal_colors.h"

#include <fstream>
#include <getopt.h>
#include <stdexcept>
#include <vector>

Store store;
Environment env;
Handler handler;

void printHelp();
void interactive();
void fromFile(std::vector<std::string> &);

int main(int argc, const char *argv[]) {
    store = Store();
    env = Environment(&store);
    handler = Handler(&store, &env);

    if (argc == 1) {
        interactive();
    } else {
        std::vector<std::string> files;
        for (int i = 1; i < argc; i++) {
            std::string arg(argv[i]);

            if (arg == "-h" || arg == "--help") {
                printHelp();
            } else if (arg == "-s" || arg == "--silent") {
                env.setSilentMode(true);
            } else {
                files.push_back(arg);
            }
        }

        fromFile(files);
    }

    return EXIT_SUCCESS;
}

void printHelp() {
    std::cout << "Usage: KeplerKV [options] [files...]\n"
              << "Options:\n"
              << "  -h, --help     Show this help menu\n"
              << "  -s, --silent   Run in silent mode (no output)\n"
              << "Files:\n"
              << "  List of input files to process\n"
              << "  If no files are passed in, KeplerKV will run in interactive mode" << std::endl;
}

void interactive() {
    std::string input;

    env.printToConsole(T_BBLUE "Welcome to KeplerKV! Type \\q to quit!" T_RESET);
    while (env.isRunning()) {
        std::cout << "> ";
        std::getline(std::cin, input);

        try {
            handler.handleQuery(input);
        } catch (std::exception &e) {
            std::cerr << T_BRED << e.what() << T_RESET << std::endl;
        }
    }
}

void fromFile(std::vector<std::string> &files) {
    for (std::string &filePath : files) {
        std::fstream file(filePath, std::ios::in);
        if (!file.is_open()) {
            throw std::runtime_error("Error: could not open file");
        }

        std::string fileExt = filePath.substr(filePath.size() - 3);
        if (fileExt != "kep") {
            std::cout << T_BYLLW << "Warning: " << filePath << " is not a valid .kep file, skipped"
                      << T_RESET << std::endl;
        }

        // Read each statement semicolon delimited at once
        std::string line;
        std::string query;
        while (std::getline(file, line)) {
            size_t pos = line.find(';');
            if (pos != std::string::npos) {
                query += line.substr(0, pos);

                try {
                    handler.handleQuery(query);
                } catch (std::exception &e) {
                    std::cerr << T_BRED << e.what() << T_RESET << std::endl;
                }
                query = "";
            } else {
                // Query may continue onto another line (or EOF)
                query += line + " ";
            }
        }

        file.close();
    }
}
