#include "List.h"
#include "ZmqManager.h"
#include <fstream>
#include <map>
#include <string>

int main(int argc, char** argv) {
    if (argc != 3) {
        perror("Not enough arguments");
        exit(EXIT_FAILURE);
    }

    Node task(atoi(argv[1]), atoi(argv[2]));

    std::map<std::string, int> map;
    
    while (1) {
        std::string message;
        std::string command = " ";
        if (auto msg = ReceiveMessage(&(task.parent)); msg.has_value()) {
            message = *msg;
        }
        std::istringstream request(message);
        request >> command;

        if (command == "create") {
            int idChild;
            request >> idChild;
            std::string ans = task.Create(idChild);
            SendMsg(&task.parent, ans);
        }
        else if (command == "pid") {
            std::string ans = task.Pid();
            SendMsg(&task.parent, ans);
        }
        else if (command == "ping") {
            int idChild;
            request >> idChild;
            std::string ans = task.Ping(idChild);
            SendMsg(&task.parent, ans);
        }
        else if (command == "send") {
            int id;
            request >> id;
            std::string str;
            getline(request, str);
            str.erase(0, 1);
            std::string ans;
            ans = task.Send(str, id);
            SendMsg(&task.parent, ans);
        }
        else if (command == "exec") {
            int targetId;
            request >> targetId;

            std::string name;
            request >> name;

            if (request.peek() == ' ') { // Save value
                int value;
                request >> value;
                map[name] = value;
                std::string to_send = "Ok:" + std::to_string(targetId);
                SendMsg(&task.parent, to_send);
            }
            else { // load val
                auto it = map.find(name);
                if (map.count(name) > 0) {
                    std::string to_send = "Ok:" + std::to_string(targetId) + ": " + std::to_string(it->second);
                    SendMsg(&task.parent, to_send);
                }
                else {
                    std::string to_send = "Ok:" + std::to_string(targetId) + " not found";
                    SendMsg(&task.parent, to_send);
                }

            }
        }
        else if (command == "kill") {
            std::string ans = task.Kill();
            ans = std::to_string(task.id) + " " + ans;
            SendMsg(&task.parent, ans);
            Disconnect(&task.parent, task.parentPort);
            task.parent.close();
            break;
        }
    }

    return 0;
}