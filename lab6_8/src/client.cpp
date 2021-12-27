#include <bits/stdc++.h>
#include "calc_node.h"
#include "zmq_funcs.h"
#include "topology.h"

int main() {
    std:: string command;
    CalculationNode node(-1, -1, -1);
    std:: string answer;
    std:: cout << "create id" << std:: endl;
    std:: cout << "exec id" << std:: endl;
    std:: cout << "pingall" << std:: endl;
    std:: cout << "kill id" << std:: endl;
    BalancedTree tree;
    while ((std:: cout << "Command: ") && (std:: cin >> command)) {
        if (command == "create") {
            int child;
            std:: cin >> child;
            if (tree.Exist(child)) {
                std:: cout << "Error: Already exists" << std:: endl;
            }
            else {
                while (true) {
                    int idParent = tree.FindID();
                    if (idParent == node.id) {
                        answer = node.create(child);
                        tree.AddInTree(child, idParent);
                        break;
                    }
                    else {
                        std:: string message = "create " + std:: to_string(child);
                        answer = node.sendstring(message, idParent);
                        if (answer == "Error: Parent not found") {
                            tree.AvailabilityCheck(idParent);
                        }
                        else {
                            tree.AddInTree(child, idParent);
                            break;
                        }
                    }
                }
                std:: cout << answer << std::endl;
            }
        }
        else if (command == "exec") {
            std:: string pattern, text;
            int child;
            std:: cin >> child;
            if (!tree.Exist(child)) {
                std:: cout << "Error: Parent is not existed" << std:: endl;
            }
            else {
            	std:: cout << "Enter pattern text: ";
            	std:: cin >> pattern;
            	std:: cout << "Enter text: ";
            	std:: cin >> text;
                std:: string message = "exec " + pattern + " " + text;
                answer = node.sendstring(message, child);
                std:: cout << answer << std:: endl;
            }
        }
        else if (command == "pingall") {
            std:: string str;
            std:: vector<int> not_available;
  
                for (int j : tree.ids) {
                    std:: string answer = node.ping(j);
                    if (answer != "Ok: 1") {
                        not_available.push_back(j);
                    }
                }
                if (not_available.empty()) {
                    std:: cout << "Ok: -1" << std:: endl;
                }
                else {
                    std:: cout << "Ok: ";
                    for (int z = 0; z < not_available.size(); ++z) {
                        std:: cout << not_available[z] << " ";
                    }
                    std:: cout << std:: endl;
                    not_available.clear();
                }
            
        }
        else if (command == "kill") {
            int child;
            std:: cin >> child;
            std:: string message = "kill";
            if (!tree.Exist(child)) {
                std:: cout << "Error: Parent is not existed" << std:: endl;
            }
            else {
                answer = node.sendstring(message, child);
                if (answer != "Error: Parent not found") {
                    tree.RemoveFromRoot(child);
                    if (child == node.left_id){
                        unbind(node.left, node.left_port);
                        node.left_id = -2;
                        answer = "Ok";
                    }
                    else if (child == node.right_id) {
                        node.right_id = -2;
                        unbind(node.right, node.right_port);
                        answer = "Ok";
                    }
                    else {
                        message = "clear " + std:: to_string(child);
                        answer = node.sendstring(message, std:: stoi(answer));
                    }
                    std:: cout << answer << std:: endl;
                }
            }
        }
        else {
            std:: cout << "Please enter correct command!" << std:: endl;
        }
    }
    node.kill();
    return 0;
}
