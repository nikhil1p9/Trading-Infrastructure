#pragma once
#include <mutex>
#include <deque>
#include <string>
#include <optional>
#include <iostream>

struct OrderBookUpdate {
    
    std::string index_name;
    double price;
    long long timestamp;

};

class WSStreamBuffer {

    private:
        std::mutex mtx;
        std::deque<OrderBookUpdate> buffer;
        const size_t max_size = 50;

    public:
        
        void push(const OrderBookUpdate & update) {
                //std::cout<<"add \t";
                std::lock_guard <std::mutex> lock(mtx);
                buffer.push_back(update);
                if(buffer.size() > max_size){
                  //  std::cout<<"pop \t";
                    buffer.pop_front();
                }
        }

        std::optional <OrderBookUpdate> latest() {
            std::lock_guard<std::mutex> lock(mtx);
            if (buffer.empty()) return std::nullopt;
            return buffer.back();
        }

        std::deque<OrderBookUpdate> snapshot(){
            std::lock_guard<std::mutex> lock(mtx);
            return buffer;
        }


};




