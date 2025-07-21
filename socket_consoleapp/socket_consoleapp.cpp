#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono> 
#include <thread> 
#include <mutex> 
#include <vector> 
#include <sstream>       
#include <iomanip>

// structure of statistics
struct Statistics {
    size_t total_messages = 0;  // total number of messages
    size_t info_count = 0;      // info messages
    size_t warning_count = 0;   // warning messages
    size_t error_count = 0;     // error messages
    size_t min_length = 0;      // minimum message length
    size_t max_length = 0;      // maximum message length
    double avg_length = 0.0;    // average message length
    size_t total_length = 0;    // sum of message lengths
    size_t messages_per_hour = 0; // messages in last hour
    std::vector<std::chrono::system_clock::time_point> hourly_messages; // timestamps of messages

    // check if statistics changed
    bool changed(const Statistics& other) const {
        return total_messages != other.total_messages ||
               info_count != other.info_count ||
               warning_count != other.warning_count ||
               error_count != other.error_count ||
               min_length != other.min_length ||
               max_length != other.max_length ||
               avg_length != other.avg_length ||
               messages_per_hour != other.messages_per_hour;
    }

    // remove messages older than 1 hour
    void clean_old_messages() {
        auto now = std::chrono::system_clock::now();
        // remove timestamps older than 3600 seconds (1 hour)
        while (!hourly_messages.empty() && std::chrono::duration_cast<std::chrono::seconds>(now - hourly_messages.front()).count() > 3600) {
            hourly_messages.erase(hourly_messages.begin());
            messages_per_hour--;
        }
    }
};

// print statistics to console
void print_statistics(const Statistics& stats) {
    std::cout << "statistics:\n"
              << "  total messages: " << stats.total_messages << "\n"
              << "  messages in last hour: " << stats.messages_per_hour << "\n"
              << "  info: " << stats.info_count << "\n"
              << "  warning: " << stats.warning_count << "\n"
              << "  error: " << stats.error_count << "\n"
              << "  minimum length: " << stats.min_length << "\n"
              << "  maximum length: " << stats.max_length << "\n"
              << "  average length: " << stats.avg_length << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "There is invalid arguments!"<<std::endl;
        return 1;
    }

    // parse arguments
    int port = std::atoi(argv[1]); // server port
    size_t N = std::atoi(argv[2]); // print stats every N messages
    int T = std::atoi(argv[3]);    // check stats every T seconds

    // create tcp socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "failed to create socket\n";
        return 1;
    }

    // set up server address
    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;// accept from any IP
    server_addr.sin_port = htons(port);// port in byte structure

    // bind socket to port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "failed to bind port\n";
        close(server_fd);
        return 1;
    }

    // listen for connections
    if (listen(server_fd, 5) == -1) {
        std::cerr << "failed to listen\n";
        close(server_fd);
        return 1;
    }

    std::cout << "server started on port " << port << "\n";
    // accept client connection
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        std::cerr << "failed to accept client\n";
        close(server_fd);
        return 1;
    }

    // variables for processing
    Statistics stats;            // current statistics
    Statistics last_stats;       // last printed statistics
    size_t message_count = 0;    // counter for N messages
    std::mutex stats_mutex;      // protect statistics
    bool running = true;         // server running flag
    char buffer[1024];           // buffer for receiving data
    std::string partial_message; // accumulate partial messages

    // start thread to check stats every T seconds
    std::thread timer_thread([&]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(T));
            std::lock_guard<std::mutex> lock(stats_mutex);
            stats.clean_old_messages(); // remove messages older than 1 hour
            if (stats.changed(last_stats)) {
                print_statistics(stats);
                last_stats = stats;
            }
        }
    });

    // main loop: receive messages
    while (running) {
        // read data from client
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            running = false; // client disconnected
            break;
        }

        buffer[bytes] = '\0'; // terminate string
        partial_message += buffer;

        // process messages separated by \n
        size_t pos;
        while ((pos = partial_message.find('\n')) != std::string::npos) { //finding first "\n" becouse are messages divided by "\n"
            std::string message = partial_message.substr(0, pos);
            partial_message.erase(0, pos + 1);

            if (message.empty()) {
                continue; // skip empty messages
            }

            std::cout << "received: " << message << "\n";

            // parsing: timestamp;level;data
            size_t first = message.find(';');
            size_t second = message.find(';', first + 1);
            std::string timestamp = message.substr(0, first);
            std::string level = message.substr(first + 1, second - first - 1);
            std::string data = message.substr(second + 1);

            // parse timestamp for hourly counter
            std::tm time = {};
            std::istringstream iss(timestamp);
            iss >> std::get_time(&time, "%Y-%m-%d %H:%M:%S");
            auto timeForm = std::chrono::system_clock::from_time_t(std::mktime(&time));

            // update statistics
            {
                std::lock_guard<std::mutex> lock(stats_mutex);
                stats.total_messages++;
                if (level == "INFO") {
                    stats.info_count++;
                } else if (level == "WARNING") {
                    stats.warning_count++;
                } else if (level == "ERROR") {
                    stats.error_count++;
                }

                size_t data_length = data.size();
                stats.total_length += data_length;
                if (stats.total_messages == 1) {
                    stats.min_length = data_length;
                    stats.max_length = data_length;
                } else {
                    if (data_length < stats.min_length) {
                        stats.min_length = data_length;
                    }
                    if (data_length > stats.max_length) {
                        stats.max_length = data_length;
                    }
                }
                stats.avg_length = static_cast<double>(stats.total_length) / stats.total_messages;

                // add timestamp for hourly counter
                stats.hourly_messages.push_back(timeForm);
                stats.messages_per_hour++;
                stats.clean_old_messages(); // remove old messages
				// check if stats should be printed
                message_count++;
                if (message_count >= N) {
                    print_statistics(stats);
                    last_stats = stats;
                    message_count = 0;
                }
            }
        }
    }

    // cleanup and exit
    running = false;
    timer_thread.join();
    close(client_fd);
    close(server_fd);
    print_statistics(stats); // final statistics
    return 0;
}
