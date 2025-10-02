#include "app.hpp"
#include "api_client.hpp"
#include "ws_stream_buffer.hpp"
#include <iostream>
#include <deque>

void start_deribit_ws(const std::string& instrument); 

extern WSStreamBuffer stream_buffer;  //shared resource

app::app(){}

void app::app_init(){

    DeribitAPIClient client("api id", "api key"); // <<-- yoho here you need to share your secrets

        try {
    if (client.authenticate()) {
       
        std::cout << "[+] Successfully Authenticated" <<std::endl; 
    } else {
        std::cerr << "Authentication failed. \n Probably you forgot to add you api key and id if not than it is something else entirely!!\n And if yes than go to app.cpp and add your secrets \n";
        std::cout<< "PLEASE RETRY " << std::endl;
        throw std::exception();
    }
         }
    catch(std::exception &e){
        std::cout<<"Authentication Issue"<<std::endl;
    }   

    

    
    while(1){
        PrintScreen();
        int input;

        try{
        input = getuserinput();
        processinput(input);
        }
        catch(std::exception &e){
                std::cout<<" ERROR : INVALID INPUT"<<std::endl;
        }
    }

}

  void app::PrintScreen(){
    std::cout<<std::endl<<"====== APP ====== "<<std::endl;
    std::cout<<"1. Place Order"<<std::endl;
    std::cout<<"2. Cancel Order"<<std::endl;
    std::cout<<"3. Modify Order"<<std::endl;
    std::cout<<"4. Get OrderBook"<<std::endl;
    std::cout<<"5. View Current Positions"<<std::endl;
    std::cout<<"6. View Market " <<std::endl;
    std::cout<<"7. Subscribe " <<std::endl;
    //std::cout<<std::endl<<"======= APP FIN ======="<<std::endl;
    std::cout<< " ..Please select an option.. " <<std::endl;
}


  void app::processinput(int input){

        switch(input)
        {
            case 1:

                try{
                option1();   
                    break;
                }
                catch(std::exception &e){
                    throw e;
                }

            case 2:

                try{
                    option2();
                }
                catch(std::exception &e){
                        throw e;
                }
               
                break;

            case 3:

                try{
                    option3();
                }
                catch(std::exception &e){
                        throw e;
                }
              
                break;

            case 4:

                try{
                    option4();
                }
                catch(std::exception &e){
                        throw e;
                }

                break;
            
            case 5:

                try{
                    option5();
                }
                catch(std::exception &e){
                        throw e;
                }
                break;

            case 6:

                try{
                    option6();
                }
                catch(std::exception &e){
                    throw e;
                }
                break;
                
            case 7: 

                        try{
                    option7();
                }
                catch(std::exception &e){
                    throw e;
                }
                break;
            

            default : 
                std::cout<<"BAD INPUT"<<std::endl;
                break;

        }

}

 int app::getuserinput(){
    int input;
    std::string line;
    std::getline(std::cin,line);

    try{
        input = std::stoi(line);
        }
    catch(std::exception & e){
        throw e;
     }

    return input;
}

 std::string app::spaceremover(std::string & s){
    
                 int l = 0;
    int h = s.size() - 1;

    // Remove leading spaces
    while (l <= h && s[l] == ' ') {
        l++;
    }

    // Remove trailing spaces
    while (h >= l && s[h] == ' ') {
        h--;
    }

    // Edge case: string has only spaces
    if (l > h) return "";

    return s.substr(l, h - l + 1);
    
     
   
}

void app::option1(){

     std::cout<<"PLACING ORDER"<<std::endl;
                std::cout<<"1. BUY "<<std::endl;
                std::cout<<"2. SELL"<<std::endl;
                std::cout<<"CHOOSE : ";
                int i {0};
                i = getuserinput();
            
                if (i!=2 and i!=1){
                    throw std::exception() ; 
                }

                std::cout<<std::endl<<"Instrument Name : ";
                
                std::string instrument_name;
                std::getline(std::cin , instrument_name);
                instrument_name = app::spaceremover(instrument_name);
                //std::cout<<"BOOM"<<instrument_name;
                double amount{0} , price{0};

            try{
                std::cout<<std::endl<<"Amount : ";
                amount = getuserinput();
                std::cout<<std::endl<<"Price : ";
                price = getuserinput();
                    }
                catch(std::exception &e){
                    throw  e;
                }

                std::cout<<std::endl<<"Type : ";
                std::string type;
                std::getline(std::cin, type);
                type = app::spaceremover(type);

                DeribitAPIClient::place_order(instrument_name , amount , price , i-1 , type );

}

void app::option2(){

     std::cout<<"CANCELING ORDER"<<std::endl;
                std::cout<<std::endl<<"Order ID : ";
                std::string order_id;
                std::getline(std::cin , order_id);
                order_id = app::spaceremover(order_id); 

                DeribitAPIClient::cancel_order(order_id);
}

void app::option3(){
          std::cout<<"MODIFYING ORDER"<<std::endl;
                std::cout<<"Order ID : ";
                std::string order_id;
                std::getline(std::cin , order_id);
                order_id = app::spaceremover(order_id); 
                double amount{0} , price{0};
            try{
                
                std::cout<<std::endl<<"Amount : ";
                amount = getuserinput();
                std::cout<<std::endl<<"Price : ";
                price = getuserinput();
                }
                catch(std::exception &e ){
                    throw e;
                }

                DeribitAPIClient::modify_order(order_id , amount , price);

}

void app::option4(){


                std::cout<<std::endl<<"ORDER-BOOK"<<std::endl;
                std::cout<<"Instrument Name : ";
                std::string instrument_name;
                std::getline(std::cin , instrument_name);
                instrument_name = app::spaceremover(instrument_name);
                std::cout<<std::endl<<"Depth : ";
                int depth;
                try{
                depth = getuserinput();
                }
                catch(std::exception  &e){
                    throw e;
                }

                DeribitAPIClient::get_orderbook(instrument_name , depth);

}

void app::option5(){

        std::cout<<std::endl<<"VIEWING CURRENT POSITIONS"<<std::endl;
                std::cout<<"Currency : ";
                std::string currency ; 
                std::getline(std::cin , currency);
                currency = spaceremover(currency);
                std::cout<<std::endl<<"Kind : ";
                std::string kind ; 
                std::getline(std::cin , kind);
                kind = spaceremover(kind);

                DeribitAPIClient::get_positions(currency , kind);
                
}

void app::option6(){

    std::cout << "\n=== Real-Time Order Book ===\n";
    auto book_data = stream_buffer.snapshot();
    try {
        int i=1;
        for(auto & update : book_data ){
        std::cout <<"["<<i<<"] INDEX NAME : " << update.index_name << " | PRICE : " << update.price<< " | TIMESTAMP : " << update.timestamp<<std::endl;
        ++i;
        }
    } 
    catch(std::exception &e){
        
        std::cout << "No live data yet.\n";
    }

}

void app::option7(){

    std::string instrument;
    std::cout<<"Help instrument name example \"deribit_price_index.btc_usd\""<<std::endl;
    std::cout<<"Instrument : ";
    std::getline(std::cin , instrument);
    instrument = spaceremover(instrument);
    start_deribit_ws(instrument);

}
