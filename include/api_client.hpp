#pragma once
#include <string>

class DeribitAPIClient {

    public:
        
        DeribitAPIClient(const std::string & client_id , const std::string & client_secret);
        bool authenticate();
        static bool get_orderbook(const std::string & instrument_name, int depth = 1);
        static bool get_positions(const std::string & currency , const std::string & kind = "option");
        static bool cancel_order(const std::string & order_id);
        static bool place_order(const std::string & instrument_name , double amount , double price , int i = 0 , const std::string & type = "limit" );
        static bool modify_order(const std::string & order_id , double amount , double price);
         
    private:
        std::string client_id ;
        std::string client_secret ;
        static std::string access_token ;
        std::string get_access_token() ;
        

};