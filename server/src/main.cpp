#include "Server.hpp"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<Server> server;

/**
 * @brief Signal handler for graceful shutdown
 * @param signum The signal number received
 */
void signal_handler(int signum) {
    std::cout << "\nReceived signal " << signum
              << ", shutting down server..." << std::endl;
    if (server) {
        server.reset();  // Clean up server instance
    }
    exit(signum);
}

/**
 * @brief Main application entry point
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit status code
 */
int main(int argc, char** argv) {
    // Register signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGTERM, signal_handler);  // kill command

    try {
        // Default configuration values
        std::string db_path = "pet_feeder.db";
        uint16_t port = 8080;

        // Parse command line arguments
        if (argc >= 2) {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        }
        if (argc >= 3) {
            db_path = argv[2];
        }

        std::cout << "Starting Pet Feeder Server\n"
                  << "Database: " << db_path << "\n"
                  << "Port: " << port << std::endl;

        // Create and start server instance
        server = std::make_unique<Server>(db_path, port);
        server->run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
