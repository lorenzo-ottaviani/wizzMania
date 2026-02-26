#pragma once

#include <mutex>
#include <string>
#include <vector>

#include <crow_all.h>

// Simple message structure
struct Message {
    std::string user;
    std::string text;
};

class ChatServer {
private:
    // Crow app
    crow::SimpleApp app;

    // Temporary chat history
    std::vector<Message> chat_history;

    // Mutex to prevent simultaneous calls of the server
    std::mutex chat_mutex;

public:
    // Constructor
    ChatServer();

    // Main run method
    void run(int port);

    //HTTP POST method to write message sent by a client in the history
    void write_message();

    //HTTP GET method to recover the chat history
    void read_history();

    //HTTP DELETE methode to clear the chat history
    void clear_history();
};
