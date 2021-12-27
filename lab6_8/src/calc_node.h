#include <bits/stdc++.h>
#include <vector>
#include "zmq_funcs.h"
#include "unistd.h"

std:: vector<unsigned int> find_substrings(std:: string pattern, std:: string text) {
	int i, j;
	std:: vector<unsigned int> vect;
	int tmp = pattern.length() - text.length() + 1;
	for (i = 0; i < tmp; ++i) {
		j = 0;
		while ((j < text.length()) && (text[j] == pattern[i + j])) {
			j += 1;
		}
		if (j == text.length()) {
			vect.push_back(i);
		}
	}
	return vect;
}

class CalculationNode {
    private:
        zmq:: context_t context;
    public:
        zmq:: socket_t left, right, parent;
        int id, left_id = -2, right_id = -2, parent_id;
        int left_port, right_port, parent_port;
        CalculationNode(int id, int parent_port, int parent_id): 
            id(id), 
            parent_port(parent_port), 
            parent_id(parent_id), 
            left(context, ZMQ_REQ),
            right(context, ZMQ_REQ), 
            parent(context, ZMQ_REP)
            {
            if (id != -1) {
                connect(parent, parent_port);
            }
        }
        std:: string create (int child_id) {
            int port;
            bool isleft = false;
            if (left_id == -2) {
                left_port = bind(left, child_id);
                left_id = child_id;
                port = left_port;
                isleft = true;
            }
            else if (right_id == -2) {
                right_port = bind(right, child_id);
                right_id = child_id;
                port = right_port;
            }
            else {
                std:: string fail = "Error: can not create the calculation node";
                return fail;
            }
            int fork_id = fork();
            if (fork_id == 0) {
                if (execl("./server", "server", std:: to_string(child_id).c_str(), std:: to_string(port).c_str(), std:: to_string(id).c_str(), (char*)NULL) == -1) {
                    std:: cout << "Error: can not run the execl-command" << std:: endl;
                    exit(EXIT_FAILURE);
                }
            }
            else {
                std:: string child_pid;
                try {
                    if (isleft) {
                        left.setsockopt(ZMQ_SNDTIMEO, 3000);
                        send_message(left, "pid");
                        child_pid = receive_message(left);
                    }
                    else {
                        right.setsockopt(ZMQ_SNDTIMEO, 3000);
                        send_message(right, "pid");
                        child_pid = receive_message(right);
                    }
                    return "Ok: " + child_pid;
                }
                catch (int) {
                    std:: string fail = "Error: can not connect to the child";
                    return fail;
                }
            }
        }
        std:: string ping (int id) {
            std:: string answer = "Ok: 0";
            if (this->id == id) {
                answer = "Ok: 1";
                return answer;
            }
            else if (left_id == id) {
                std:: string message = "ping " + std:: to_string(id);
                send_message(left, message);
                try {
                    message = receive_message(left);
                    if (message == "Ok: 1") {
                        answer = message;
                    }
                }
                catch(int){}
            }
            else if (right_id == id) {
                std:: string message = "ping " + std:: to_string(id);
                send_message(right, message);
                try {
                    message = receive_message(right);
                    if (message == "Ok: 1") {
                        answer = message;
                    }
                }
                catch(int){}
            }
            return answer;
        }
        std:: string sendstring (std:: string string, int id) {
            std:: string answer = "Error: Parent not found";
            if (left_id == -2 && right_id == -2) {
                return answer;
            }
            else if (left_id == id) {
                if (ping(left_id) == "Ok: 1") {
                    send_message(left, string);
                    try{
                        answer = receive_message(left);
                    }
                    catch(int){}
                }
            }
            else if (right_id == id) {
                if (ping(right_id) == "Ok: 1") {
                    send_message(right, string);
                    try {
                        answer = receive_message(right);
                    }
                    catch(int){}
                }
            }
            else {
                if (ping(left_id) == "Ok: 1") {
                    std:: string message = "send " + std:: to_string(id) + " " + string;
                    send_message(left, message);
                    try {
                        message = receive_message(left);
                    }
                    catch(int) {
                        message = "Error: Parent not found";
                    }
                    if (message != "Error: Parent not found") {
                        answer = message;
                    }
                }
                if (ping(right_id) == "Ok: 1") {
                    std:: string message = "send " + std:: to_string(id) + " " + string;
                    send_message(right, message);
                    try {
                        message = receive_message(right);
                    }
                    catch(int) {
                        message = "Error: Parent not found";
                    }
                    if (message != "Error: Parent not found") {
                        answer = message;
                    }
                }
            }
            return answer;
        }

        std:: string exec (std:: string string) {
            std:: istringstream string_thread(string);
            std:: string pattern, text, result;
            string_thread >> pattern;
            string_thread >> text;            
            std:: string answer = "Ok: " + std:: to_string(id) + ": ";
			std:: vector<unsigned int> res = find_substrings(pattern, text);
			if (res.empty()) {
				return answer;
			}
			else {
				for (int i = 0; i < res.size() - 1; ++i) {
					answer += std:: to_string(res[i]);
					answer += ";";
				}
				answer += std:: to_string(res.back());
			}
			
            return answer;
        }
        std:: string treeclear (int child) {
            if (left_id == child) {
                left_id = -2;
                unbind(left, left_port);
            }
            else {
                right_id = -2;
                unbind(right, right_port);
            }
            return "Ok";
        }
        std:: string kill () { 
            if (left_id != -2){
                if (ping(left_id) == "Ok: 1") {
                    std:: string message = "kill";
                    send_message(left, message);
                    try {
                        message = receive_message(left);
                    }
                    catch(int){}
                    unbind(left, left_port);
                    left.close();
                }
            }
            if (right_id != -2) {
                if (ping(right_id) == "Ok: 1") {
                    std:: string message = "kill";
                    send_message(right, message);
                    try {
                        message = receive_message(right);
                    }
                    catch (int){}
                    unbind(right, right_port);
                    right.close();
                }
            }
            return std:: to_string(parent_id);
        }
        ~CalculationNode() {}
};
