#include "api_client.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>

std::string DeribitAPIClient::access_token = ""; 

using json = nlohmann::json;

static size_t WriteCallback(void * contents , size_t size , size_t nmemb ,std::string * output){
    size_t totalSize = size * nmemb ;
    output -> append((char*)contents,totalSize);
    return totalSize;

}

// constructor
DeribitAPIClient::DeribitAPIClient(const std::string & client_id , const std::string & client_secret )
    : client_id(client_id),client_secret(client_secret) {}   // name shadowing problem can arise we list inisialisation was not done here if i dont want to do the list inisialisation i can use  
                                                            /*  using this * we can mitigate the name shadowing problem or we can use different var names in our constructor always!!
                                                            this -> client_id = client_id;     
                                                            this -> client_secret = client_secret;
                                                            */
                                                                

bool DeribitAPIClient::authenticate(){
    CURLcode ret;
    CURL * curl ; 
    curl = curl_easy_init();
    if(!curl) return false;

    std::string url = "https://test.deribit.com/api/v2/public/auth";
    std::string response;
    std::string PostFields = "{ \"method\": \"public/auth\" , \"params\": { \"grant_type\": \"client_credentials\",\"client_id\": \""+ client_id + "\", \"client_secret\": \"" + client_secret + "\" } }";
    //json jData = json::parse(PostFields);
    //std::cout<<std::setw(4)<<jData<<std::endl;
    
    // we can also parse the json 
    /*
            json j = {  {"method","public/auth"},
                    {"params": {
                        {"client_id" , clien_id},
                        {"client_secret" , client_secret},
                        {"grant_type" , "client_credentials"} 
                           }
                    } 
                };
            std::cout<<std::setw(4)<<j<<std::endl;
           // std::string PostFields = j.dump();
    */
    
    // curl 
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); //url
    

    // data is not post on redirection if not opted so if there could be redirection we should 
    // curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); //redirection
    // curl_easy_setopt(curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL); // CURL will shift to GET and our data will not be post
    
    
    curl_easy_setopt(curl, CURLOPT_POST, 1L); //POST method
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, PostFields.c_str()); //data
//    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //verbose

    // header
    struct curl_slist * headers = NULL;
    headers = curl_slist_append(headers , "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl ,CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
        std::cerr << "Curl Error: " << curl_easy_strerror(ret) << "\n";
        //memory cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        curl = NULL;
        headers = NULL;
        return false;
    }

    //memory cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    headers = NULL;
    curl = NULL;
    //std::cout<<response<<std::endl;
    try {
        json j = json::parse(response);
       //std::cout<<std::setw(4)<<j<<std::endl;
        access_token = j["result"]["access_token"];
        return true;
    } catch (const std::exception& e) {
        std::cerr << "JSON Parse Error: " << e.what() << "\n";
        return false;
    }
}

std::string DeribitAPIClient::get_access_token()  {

        if(access_token.empty()){
            throw std::exception();
        }

    return access_token;
}

bool DeribitAPIClient::place_order(const std::string & instrument_name , double amount , double price  , int  i , const std::string & type)
    {
        
    
    
        if(access_token.empty()) {
            std::cerr << "Access Token is missing \n";
            return false;
        }

    CURLcode ret;        
    CURL * curl = NULL;    
    curl = curl_easy_init();
    if(!curl) return false;

    std::string response;
    std::string url;
    json jData;

  //---------------//  
    if(i==1){
     url = "https://test.deribit.com/api/v2/private/sell";
     jData = {
        {"method","private/sell"},
        {"params", {
        
            {"instrument_name", instrument_name} , 
            {"amount", amount} ,
            {"price", price} ,
            {"type", type} 
            
                   }
        } 
                 };
    }
    // default buy
    else{
         url = "https://test.deribit.com/api/v2/private/buy";
         jData = {
        {"method","private/buy"},
        {"params", {
        
            {"instrument_name", instrument_name} , 
            {"amount", amount} ,
            {"price", price} ,
            {"type", type} 
            
                   }
        } 
                 };

    }
//--------------//

    std::string bearer = "Authorization: Bearer " + access_token;   

    std::cout<< std::endl << "--ORDER_DETAILS-- " << std::endl << jData["params"] << std::endl << "--ORDER_DETAILS_FIN--"<<std::endl;
    std::string PostFields = jData.dump();

    //Headers
    struct curl_slist * headers = NULL;
    headers = curl_slist_append(headers , bearer.c_str());
    headers = curl_slist_append(headers , "Content-Type: application/json");

    curl_easy_setopt(curl , CURLOPT_URL , url.c_str());
    //curl_easy_setopt(curl , CURLOPT_VERBOSE , 1L);
    curl_easy_setopt(curl , CURLOPT_HTTPHEADER , headers);
    curl_easy_setopt(curl , CURLOPT_POSTFIELDS , PostFields.c_str());
    curl_easy_setopt(curl , CURLOPT_WRITEFUNCTION , WriteCallback);
    curl_easy_setopt(curl , CURLOPT_WRITEDATA , &response);

    ret = curl_easy_perform(curl);
    if(ret != CURLE_OK){
        std::cerr << "Curl Error: " << curl_easy_strerror(ret) << std::endl;
        //memory cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        curl = NULL;
        headers = NULL;

        return false;
    }
        //memory cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        curl = NULL;
        headers = NULL;

       // std::cout << "Order Response : " << response << std::endl;

        try{    
                json j = json::parse(response);
                if(j.contains("result")){
                    std::cout<< std::endl <<"[+] Order placed successfully. Order ID:"<< j["result"]["order"]["order_id"] << std::endl;
                    return true;
                }
                
                std::cout<<std::endl <<"--ERROR-- \n" << std::setw(4) << j["error"] << std::endl;
                std::cout<<"--ERROR_FIN--" << std::endl; 
                return false;
        }
        catch(std::exception & e){
                std::cerr << "JSON Error:" << e.what() << std::endl;
                return false;
        }

}


bool DeribitAPIClient::cancel_order(const std::string & order_id){
    
       

    if(access_token.empty()) {
            std::cerr << "Access Token is missing \n";
            return false;
        }

    CURLcode ret;
    CURL * curl = NULL ; 
    curl = curl_easy_init();
    if(!curl) return false;

    std::string response;
    std::string url = "https://test.deribit.com/api/v2/private/cancel";
    std::string bearer = "Authorization: Bearer " + access_token;

    json jData = {
        {"method","private/cancel"} ,
        {"params",{
                {"order_id",order_id}
                  }
        }
                };
        
    std::cout<< std::endl << "--CANCEL_ORDER_DETAILS--" << std::endl << jData["params"] << std::endl << "--CANCEL_ORDER_DETAILS_FIN--"<<std::endl; 
    std::string PostFields = jData.dump();

    //header
    struct curl_slist * headers = NULL;
    headers = curl_slist_append(headers , bearer.c_str());
    headers = curl_slist_append(headers , "Content-Type: application/json");

    curl_easy_setopt(curl , CURLOPT_URL , url.c_str());
    //curl_easy_setopt(curl , CURLOPT_VERBOSE , 1L);
    curl_easy_setopt(curl , CURLOPT_HTTPHEADER , headers);
    curl_easy_setopt(curl , CURLOPT_POSTFIELDS , PostFields.c_str());
    curl_easy_setopt(curl , CURLOPT_WRITEFUNCTION , WriteCallback);
    curl_easy_setopt(curl , CURLOPT_WRITEDATA , &response);

    ret = curl_easy_perform(curl);
    if(ret != CURLE_OK){
        std::cerr << "Curl Error: " << curl_easy_strerror(ret) << std::endl;
        //memory cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        curl = NULL;
        headers = NULL;

        return false;
    }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        curl = NULL;
        headers = NULL;

        try{    
                json j = json::parse(response);
                if(j.contains("result")){
                    std::cout<< std::endl <<"[+] Order Canceled successfully. Order ID:"<< j["result"]["order_id"] << std::endl;
                    return true;
                }
                
                std::cout<<std::endl <<"--ERROR-- \n" << std::setw(4) << j["error"] << std::endl;
                std::cout<<"--ERROR_FIN--" << std::endl; 
                return false;
        }
        catch(std::exception & e){
                std::cerr << "JSON Error:" << e.what() << std::endl;
                return false;
        }


}

bool DeribitAPIClient::modify_order(const std::string & order_id , double amount , double price ){

        

      if(access_token.empty()) {
            std::cerr << "Access Token is missing \n";
            return false;
        }

        CURLcode ret;
        CURL * curl = NULL;
        curl = curl_easy_init();
        if(!curl) return false;

        std::string response;
        std::string url = "https://test.deribit.com/api/v2/private/edit";
        std::string bearer = "Authorization: Bearer " + access_token;

        json jData = {
            {"method","private/edit"} , 
            {"params" ,{
                    {"order_id",order_id},
                    {"amount", amount},
                    {"price", price}

                       }
            }
                    };

            std::cout<<std::endl << "--EDIT_ORDER_DETAIL--" << std::endl << jData["params"] <<std::endl <<"--EDIT_ORDER_DETAILS_FIN--"<<std::endl;
            std::string PostFields = jData.dump();

            //header
            struct curl_slist * headers = NULL;
            headers = curl_slist_append(headers , bearer.c_str());
            headers = curl_slist_append(headers , "Content-Type: application/json");

            curl_easy_setopt(curl , CURLOPT_URL , url.c_str());
            curl_easy_setopt(curl , CURLOPT_HTTPHEADER , headers);
            curl_easy_setopt(curl , CURLOPT_POSTFIELDS , PostFields.c_str());
            curl_easy_setopt(curl , CURLOPT_WRITEFUNCTION , WriteCallback);
            curl_easy_setopt(curl , CURLOPT_WRITEDATA , &response);

            ret = curl_easy_perform(curl);
            if(ret != CURLE_OK){
                std::cerr << "Curl Error: " << curl_easy_strerror(ret) << std::endl;
                //memory cleanup
                curl_easy_cleanup(curl);
                curl_slist_free_all(headers);
                curl = NULL;
                headers = NULL;

                return false;
            }

            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            curl = NULL;
            headers = NULL;

            try{    
                json j = json::parse(response);
                if(j.contains("result")){
                    std::cout<< std::endl <<"[+] Order modified successfully. Order ID:"<< j["result"]["order"]["order_id"] << std::endl;
                    return true;
                }
                
                std::cout<<std::endl <<"--ERROR-- \n" << std::setw(4) << j["error"] << std::endl;
                std::cout<<"--ERROR_FIN--" << std::endl; 
                return false;
        }
            catch(std::exception & e){
                std::cerr << "JSON Error:" << e.what() << std::endl;
                return false;
        }

}

bool DeribitAPIClient::get_orderbook(const std::string & instrument_name , int depth){
     // simple GET request
        

        if(access_token.empty()) {
            std::cerr << "Access Token is missing \n";
            return false;
        }

    CURLcode ret;        
    CURL * curl = NULL;    
    curl = curl_easy_init();
    if(!curl) return false;

    std::string response;
    std::string url = "https://test.deribit.com/api/v2/public/get_order_book?depth="+std::to_string(depth)+"&instrument_name="+instrument_name;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    ret = curl_easy_perform(curl);
    if(ret != CURLE_OK){
         std::cerr << "Curl Error: " << curl_easy_strerror(ret) << std::endl;
                //memory cleanup
                curl_easy_cleanup(curl);
                curl = NULL;
                
                return false;
    }

        curl_easy_cleanup(curl);
            curl = NULL;
                

       try{    
                json j = json::parse(response);
                if(j.contains("result")){

                    std::cout<< std::endl <<"=== ORDER BOOK : " << instrument_name << " (Depth " << depth << ") ==="<< std::endl;
                    std::cout<< "\n=== BIDS:\n";
                    for(const auto& bid : j["result"]["bids"]){
                        std::cout<< "Price: " << bid[0] <<" | Amount:" << bid[1] << std::endl;
                    }

                    std::cout << "\n=== ASKS:\n";
                    for (const auto& ask : j["result"]["asks"]) {
                    std::cout << "Price: " << ask[0] << " | Amount: " << ask[1] << "\n";
                    }

                    std::cout<<std::endl <<"=== ORDER BOOK FIN ==="<<std::endl;
                    return true;
                }
                
                std::cout<<std::endl <<"--ERROR-- \n" << std::setw(4) << j["error"] << std::endl;
                std::cout<<"--ERROR_FIN--" << std::endl; 
                return false;
        }
            catch(std::exception & e){
                std::cerr << "JSON Error:" << e.what() << std::endl;
                return false;
        }


}


bool DeribitAPIClient::get_positions(const std::string & currency , const std::string & kind){
           
       

     if(access_token.empty()) {
            std::cerr << "Access Token is missing \n";
            return false;
        }

    CURLcode ret;        
    CURL * curl = NULL;    
    curl = curl_easy_init();
    if(!curl) return false;

    std::string response;
    std::string url = "https://test.deribit.com/api/v2/private/get_positions";
    std::string bearer = "Authorization: Bearer " + access_token;

    json jData = {
            {"method","private/get_positions"},
            {"params",{
                    {"currency",currency},
                    {"kind",kind}
                    }
            }
    };


    std::string PostFields = jData.dump();
    struct curl_slist * headers = NULL;
    headers = curl_slist_append(headers , bearer.c_str());
    headers = curl_slist_append(headers , "Content-Type: application/json");


    curl_easy_setopt(curl , CURLOPT_URL , url.c_str());
    curl_easy_setopt(curl , CURLOPT_HTTPHEADER ,headers);
    curl_easy_setopt(curl , CURLOPT_POSTFIELDS , PostFields.c_str());
    curl_easy_setopt(curl , CURLOPT_WRITEFUNCTION , WriteCallback);
    curl_easy_setopt(curl , CURLOPT_WRITEDATA , &response);

    
    ret = curl_easy_perform(curl);
    if(ret != CURLE_OK){
         std::cerr << "Curl Error: " << curl_easy_strerror(ret) << std::endl;
                //memory cleanup
                curl_easy_cleanup(curl);
                curl = NULL;
                curl_slist_free_all(headers);
                headers = NULL;
                return false;
    }

        curl_easy_cleanup(curl);
            curl = NULL;
        curl_slist_free_all(headers);
        headers = NULL;
   
    try{    
            json j = json::parse(response);
            if(j.contains("result")){

                     std::cout << "=== Open Positions (" << currency <<" : " << kind << ") ===\n";

        for (const auto& position : j["result"]) {
            std::cout << "Instrument: " << position["instrument_name"] << "\n";
            std::cout << "  Size: " << position["size"]
                      << " | Avg Price: " << position["average_price"]
                      << " | Delta: " << position["delta"] << "\n";
                }

            return true;
            }
                
                std::cout<<std::endl <<"--ERROR-- \n" << std::setw(4) << j["error"] << std::endl;
                std::cout<<"--ERROR_FIN--" << std::endl; 
                return false;
        }
            catch(std::exception & e){
                std::cerr << "JSON Error:" << e.what() << std::endl;
                return false;
        }


}
