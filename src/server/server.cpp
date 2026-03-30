#include <iostream>
#include <winsock2.h>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")

std::map<SOCKET, std::string> clients;
std::mutex clients_mutex;

std::string create_json(const std::string& type, const std::string& pseudo = "", const std::string& content = "") {
    std::string json = "{\"type\":\"" + type + "\"";
    if (!pseudo.empty()) json += ",\"pseudo\":\"" + pseudo + "\"";
    if (!content.empty()) json += ",\"content\":\"" + content + "\"";
    json += "}";
    return json;
}

void broadcast(const std::string& msg, SOCKET exclude = INVALID_SOCKET) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto& [sock, pseudo] : clients) {
        if (sock != exclude) {
            send(sock, msg.c_str(), msg.length(), 0);
            send(sock, "\n", 1, 0);
        }
    }
}

void handle_client(SOCKET client_socket) {
    char buffer[4096];
    std::string pseudo = "";

    while (true) {
        int recv_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (recv_size <= 0) {
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                if (!pseudo.empty()) {
                    broadcast(create_json("LEAVE", pseudo), INVALID_SOCKET);
                    std::cout << pseudo << " déconnecté\n";
                }
                clients.erase(client_socket);
            }
            closesocket(client_socket);
            break;
        }

        buffer[recv_size] = '\0';
        std::string data(buffer);

        // Simple JSON parsing
        if (data.find("\"type\":\"JOIN\"") != std::string::npos) {
            size_t pseudo_start = data.find("\"pseudo\":\"") + 10;
            size_t pseudo_end = data.find("\"", pseudo_start);
            pseudo = data.substr(pseudo_start, pseudo_end - pseudo_start);

            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients[client_socket] = pseudo;
            }

            send(client_socket, create_json("JOIN_OK", pseudo).c_str(), 
                 create_json("JOIN_OK", pseudo).length(), 0);
            send(client_socket, "\n", 1, 0);

            broadcast(create_json("JOIN", pseudo), client_socket);

            std::string user_list = "{\"type\":\"USER_LIST\",\"users\":[";
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                bool first = true;
                for (auto& [s, p] : clients) {
                    if (!p.empty()) {
                        if (!first) user_list += ",";
                        user_list += "\"" + p + "\"";
                        first = false;
                    }
                }
            }
            user_list += "]}";
            send(client_socket, user_list.c_str(), user_list.length(), 0);
            send(client_socket, "\n", 1, 0);

            std::cout << pseudo << " connecté. Total: " << clients.size() << "\n";
        }
        else if (data.find("\"type\":\"MESSAGE\"") != std::string::npos && !pseudo.empty()) {
            size_t content_start = data.find("\"content\":\"") + 11;
            size_t content_end = data.find("\"", content_start);
            std::string content = data.substr(content_start, content_end - content_start);
            
            broadcast(create_json("MESSAGE", pseudo, content));
            std::cout << pseudo << ": " << content << "\n";
        }
        else if (data.find("\"type\":\"GAME\"") != std::string::npos && !pseudo.empty()) {
            size_t choice_start = data.find("\"choice\":\"") + 10;
            size_t choice_end = data.find("\"", choice_start);
            std::string choice = data.substr(choice_start, choice_end - choice_start);
            
            std::string choices[] = {"pierre", "papier", "ciseaux"};
            int server_choice_idx = rand() % 3;
            std::string server_choice = choices[server_choice_idx];
            
            std::string result = "égalité";
            if (choice == server_choice) {
                result = "égalité";
            } else if ((choice == "pierre" && server_choice == "ciseaux") ||
                       (choice == "papier" && server_choice == "pierre") ||
                       (choice == "ciseaux" && server_choice == "papier")) {
                result = "gagné";
            } else {
                result = "perdu";
            }
            
            std::string game_result = "{\"type\":\"GAME_RESULT\",\"player\":\"" + pseudo + 
                                     "\",\"player_choice\":\"" + choice + 
                                     "\",\"server_choice\":\"" + server_choice + 
                                     "\",\"result\":\"" + result + "\"}";
            broadcast(game_result);
            std::cout << pseudo << " joue au jeu: " << choice << " vs " << server_choice << " = " << result << "\n";
        }
    }
}

int main() {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "WSAStartup échoué\n";
        return 1;
    }

    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET) {
        std::cerr << "socket() échoué\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    if (bind(listen_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "bind() échoué\n";
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen() échoué\n";
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "🌊 WIZZ Mania Serveur lancé sur 127.0.0.1:8080\n";

    while (true) {
        SOCKET client_socket = accept(listen_socket, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "accept() échoué\n";
            continue;
        }

        std::thread t(handle_client, client_socket);
        t.detach();
    }

    closesocket(listen_socket);
    WSACleanup();
    return 0;
}
