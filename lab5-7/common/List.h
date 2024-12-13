#pragma once

#include <iostream>
#include <sstream>
#include <optional>
#include <memory>
#include <unordered_map>
#include <string>
#include <windows.h>
#include <list>
#include <algorithm>

#include "ZmqManager.h"

class Node {
public:
    Node(int _id, int _parentPort = -1)
        : 
        id(_id), parent(context, ZMQ_REP), parentPort(_parentPort) {
        if (_id != -1) {
            Connect(&parent, _parentPort);
        }
    }

    std::string Ping(int _id) {
        std::string ans = "Ok: 0";
        if (_id == id) {
            ans = "Ok: 1";
            return ans;
        }
        else if (auto it = std::find_if(children.begin(), children.end(), [&](auto a) {return a.first == _id; }); it != children.end()) {
            std::string msg = "ping " + std::to_string(_id);
            SendMsg(it->second.get(), msg);
            auto iChild = std::find_if(children.begin(), children.end(), [&](auto a) {return a.first == _id; });
            if (auto msg = ReceiveMessage(iChild->second.get()); msg.has_value(), msg == "Ok: 1") {
                ans = *msg;
            }
            return ans;
        }
        return ans;
    }

    std::string Create(int idChild) {
        children.push_back(std::make_pair(idChild,std::make_shared<zmq::socket_t>(context, ZMQ_REQ)));

        auto iChild = std::find_if(children.begin(), children.end(), [&](auto a) {return a.first == idChild; });
        int newPort = Bind(iChild->second.get(), idChild);
        childrenPort.push_back(std::make_pair(idChild, newPort));

        std::string commandLine = "node.exe " + std::to_string(idChild) + " " + std::to_string(newPort);
        STARTUPINFO si{};
        PROCESS_INFORMATION pi{};
        si.cb = sizeof(si);

        if (!CreateProcess(
            nullptr,
            const_cast<char*>(commandLine.c_str()),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi)) {
            return "Error: Could not create process for node " + std::to_string(idChild);
        }

   
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        std::string pidChild = "Error: couldn't connect to child";

        iChild->second.get()->set(zmq::sockopt::sndtimeo, 3000);
        SendMsg(iChild->second.get(), "pid");
        if (auto msg = ReceiveMessage(iChild->second.get()); msg.has_value()) {
            pidChild = *msg;
        }
        return "Ok: " + pidChild;
    }

    std::string Pid() {
        return std::to_string(GetCurrentProcessId());
    }

    std::string Send(const std::string& str, int _id) {
        if (children.size() == 0) {
            return "Error: Not found";
        }
        else if (auto it = std::find_if(children.begin(), children.end(), [&](auto a) {return a.first == _id; }); it != children.end()) {
            if (SendMsg(it->second.get(), str)) {
                std::string ans = "Error: Not found";
                if (auto msg = ReceiveMessage(it->second.get()); msg.has_value()) {
                    ans = *msg;
                }
                return ans;
            }
        }
        else {
            std::string ans = "Error: Not found";
            for (auto& child : children) {
                std::string msg = "send " + std::to_string(id) + " " + str;
                if (SendMsg(child.second.get(), msg)) {
                    if (auto msg = ReceiveMessage(child.second.get()); msg.has_value()) {
                        ans = *msg;
                    }
                }
            }
            return ans;
        }
        return 0;
    }

    std::string Kill() {
        std::string ans;
        for (auto& child : children) {
            std::string msg = "kill";
            if (SendMsg(child.second.get(), msg)) {
                if (auto tmp = ReceiveMessage(child.second.get()); tmp.has_value()) {
                    msg = *tmp;
                }
                if (ans.size() > 0) {
                    ans = ans + " " + msg;
                }
                else {
                    ans = msg;
                }
            }
            auto iPort = std::find_if(childrenPort.begin(), childrenPort.end(), [&](auto a) {return a.second == child.first; });
            Unbind(child.second.get(), iPort->second);
            child.second->close();
        }
        children.clear();
        childrenPort.clear();
        return ans;
    }
private:
    zmq::context_t context;
public:
    std::list<std::pair<int, std::shared_ptr<zmq::socket_t>>> children;
    std::list<std::pair<int, int>> childrenPort;
    int id;
    zmq::socket_t parent;
    int parentPort;
};