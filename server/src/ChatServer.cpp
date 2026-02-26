#include "ChatServer.hpp"

#include <crow_all.h>

ChatServer::ChatServer () {
    write_message();
    read_history();
    clear_history();
}

void ChatServer::run(int port) {
    this->app.port(port).run();
}

void ChatServer::write_message() {
    CROW_ROUTE(app, "/chat/send").methods(crow::HTTPMethod::Post) ([this](const crow::request& req){
        // Parse the JSON sent by the client with POST method
        auto x = crow::json::load(req.body);

        if (!x) return crow::response(400, " HTTP POST Method ERROR : Bad JSON format");

        // Lock the server
        std::lock_guard<std::mutex> lock(chat_mutex);

        // Save the message in the history
        this->chat_history.push_back({x["user"].s(), x["message"].s()});

        // Send a response to the client
        return crow::response(200, "Message correctly saved in the history");
    });
}

void ChatServer::read_history() {
    CROW_ROUTE(app, "/chat/history") ([this](){
        // Lock the server
        std::lock_guard<std::mutex> lock(chat_mutex);

        // Create a response JSON
        crow::json::wvalue result;

        // Retrieve the chat history and write it in the response JSON
        for (int i = 0; i < this->chat_history.size(); i++) {
            result[i]["user"] = this->chat_history[i].user;
            result[i]["message"] = this->chat_history[i].text;
        }

        // Send the response JSON to the client
        return result;
    });
}

void ChatServer::clear_history() {
    CROW_ROUTE(app, "/chat/clear").methods(crow::HTTPMethod::Delete) ([this](){
        // Lock the server
        std::lock_guard<std::mutex> lock(chat_mutex);

        // Clear the temporary history
        this->chat_history.clear();

        // Create a response JSON
        crow::json::wvalue resp;
        resp["status"] = "success";
        resp["message"] = "History correctly cleaned !";

        // Send the response JSON to the client
        return crow::response(200, resp);
    });
}
