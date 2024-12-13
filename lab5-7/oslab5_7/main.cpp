#include <set>

#include "List.h"
#include "ZmqManager.h"

int main() {
    std::set<int> Nodes;
    Node task(-1);
    Nodes.insert(-1);
    std::string command;

    while (std::cin >> command) {
        if (command == "create") {
            int idChild, idParent;
            std::cin >> idChild >> idParent;
            if (Nodes.find(idChild) != Nodes.end()) {
                std::cout << "Error: Already exists" << std::endl;
            }
            else if (Nodes.find(idParent) == Nodes.end()) {
                std::cout << "Error: Parent not found" << std::endl;
            }
            else if (idParent == task.id) { // from -1
                std::string ans = task.Create(idChild);
                std::cout << ans << std::endl;
                Nodes.insert(idChild);
            }
            else { // from other node
                std::string str = "create " + std::to_string(idChild);
                std::string ans = task.Send(str, idParent);
                std::cout << ans << std::endl;
                Nodes.insert(idChild);
            }
        }
        else if (command == "ping") {
            int idChild;
            std::cin >> idChild;
            if (Nodes.find(idChild) == Nodes.end()) {
                std::cout << "Error: Not found" << std::endl;
            }
            else if (std::find_if(task.children.begin(), task.children.end(), [&](auto a) {return a.first == idChild; }) != task.children.end()) {
                std::string ans = task.Ping(idChild);
                std::cout << ans << std::endl;
            }
            else {
                std::string str = "ping " + std::to_string(idChild);
                std::string ans = task.Send(str, idChild);
                if (ans == "Error: Not found") {
                    ans = "Ok: 0";
                }
                std::cout << ans << std::endl;
            }
        }
        else if (command == "exec") {
            int id;
            std::cin >> id;
            if (Nodes.find(id) == Nodes.end()) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            }

            std::string name;
            std::cin >> name;

            if (std::cin.peek() == ' ') { // Save value
                int value;
                std::cin >> value;
                std::string msg = "exec " + std::to_string(id) + " " + name + " " + std::to_string(value);
                std::string ans = task.Send(msg, id);
                std::cout << ans << std::endl;
            }
            else { // Load value
                std::string msg = "exec " + std::to_string(id) + " " + name;
                std::string ans = task.Send(msg, id);
                std::cout << ans << std::endl;
            }
        }
        else if (command == "kill") {
            int id;
            std::cin >> id;
            std::string msg = "kill";
            if (Nodes.find(id) == Nodes.end()) {
                std::cout << "Error: Not found" << std::endl;
            }
            else {
                std::string ans = task.Send(msg, id);
                if (ans != "Error: Not found") {
                    std::istringstream ids(ans);
                    int tmp;
                    while (ids >> tmp) {
                        Nodes.erase(tmp);
                    }
                    ans = "Ok";
                    auto found = std::find_if(task.children.begin(), task.children.end(), [&](auto a) {return a.first == id; });
                    auto found2 = std::find_if(task.childrenPort.begin(), task.childrenPort.end(), [&](auto a) {return a.first == id; });
                    if (found != task.children.end()) {
                        Unbind(found->second.get(), found2->second);
                        found->second->close();
                        task.children.erase(found);
                        task.childrenPort.erase(found2);
                    }
                }
                std::cout << ans << std::endl;
            }
        }
        else if (command == "exit") {
            task.Kill();
            return 0;
        }
    }
}