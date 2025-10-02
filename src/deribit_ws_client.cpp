#include "ws_stream_buffer.hpp"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include <boost/asio/ssl/context.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> WSClient;
typedef websocketpp::lib::shared_ptr <boost::asio::ssl::context> context_ptr ;
using json = nlohmann::json;

// TLS intialisation handler
context_ptr on_tls_init(websocketpp::connection_hdl hdl){

    std::cout<< "[TLS] Initialising TLS handler" << std::endl; 
    context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tls_client);

    try {
        ctx-> set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 |
                         boost::asio::ssl::context::single_dh_use);

        ctx -> set_verify_mode(boost::asio::ssl::verify_peer);

        ctx-> set_default_verify_paths();

        std::cout<<"[TLS] Context configured successfully" << std::endl;
    }
    catch(std::exception & e){
        std::cerr << "[TLS ERROR] Failed to configure TLS context : " << e.what() <<std::endl;
        // we can throw nullptr or throw error to indicate failure
        return nullptr; 
    }

    return ctx;

}


WSStreamBuffer stream_buffer;

void on_message(websocketpp::connection_hdl , WSClient::message_ptr msg) {

    try{
        json j = json::parse(msg->get_payload());
       // std::cout<<j<<std::endl;
        
        //check if the msg is a subscription confirmation or actual data
        if(j.contains("params") and j["params"].contains("channel") and j["params"].contains("data")) {

            const auto & data = j["params"]["data"];  //json

        if(data.contains("index_name") and data.contains("price") and data.contains("timestamp")) {

            OrderBookUpdate update;  //struct we defined

            //std::cout<<"boo";
            update.index_name = data["index_name"].get<std::string>();
            update.price = data["price"].get<double>();
            update.timestamp = data["timestamp"].get<long long>();
            stream_buffer.push(update);
               // std::cout<<"pushed";
            } 
        }
        else if (j.contains("result")){
            //subscription confirmation or other rpc results
            std::cout<< "[WS INFO] Received result message : " << j.dump(2) << std::endl;
            }
        else{
            std::cout<<std::endl<<"===ERROR==="<<std::endl<<j<<std::endl;
        }
    }
    catch(std::exception & e){
        std::cerr << "ERROR : " << e.what() <<std::endl; 
    }

}


void start_deribit_ws(const std::string & instrument) {

    std::thread([instrument](){
        WSClient wsclient;

        try{
                wsclient.init_asio(); //asio integration
                //tls initialisation handler
                wsclient.set_tls_init_handler(&on_tls_init);

                //register other handlers
                wsclient.set_message_handler(&on_message);

                wsclient.set_open_handler([&wsclient , instrument](websocketpp::connection_hdl hdl) {
                                            // lambda expression 
                    std::cout<< "[WS Info] Connection opened. Subscribing to " <<instrument << "..." <<std::endl;
                    json sub = {
                        {"method","public/subscribe"} ,
                        {"params" , {
                           // {"channels",{"book."+instrument+".raw"}}
                             {"channels", {instrument} }
                                    }
                        }
                                };                    

                    websocketpp::lib::error_code send_ec;
                    wsclient.send(hdl , sub.dump() , websocketpp::frame::opcode::text , send_ec);
                    if(send_ec){
                        std::cerr<< "[WS Error] Failed to send subscription : " << send_ec.message() << std::endl;
                    }
                    else {
                            std::cout<< "[WS Info] Subscription message sent for "<< instrument << std::endl;
                    }

                });

                wsclient.set_fail_handler([](websocketpp::connection_hdl hdl) {
                 // You can get more detailed error info here using client.get_con_from_hdl(hdl)
                std::cerr << "[WS Error] Connection attempt failed." << std::endl;
            });

            wsclient.set_close_handler([](websocketpp::connection_hdl hdl) {
                std::cerr << "[WS Info] Connection closed." << std::endl;
            });

            websocketpp::lib::error_code ec;
            WSClient::connection_ptr con = wsclient.get_connection("wss://test.deribit.com/ws/api/v2" , ec);

            if(ec){
                std::cerr << "[WS ERROR] Could not create connection : " << ec.message() << std::endl;
                return ; 
            }

            // connection attempt
             wsclient.connect(con);

             // start the client's processing loop in separate thread
             std::thread io_thread([&wsclient](){
                try{

                        std::cout<<"[WS INFO] Starting client event loop ..." << std::endl;

                        wsclient.run(); // this blocks until the wsclient stops or an error occurs
                        
                        std::cout<<"[WS INFO] Client event loop stopped. " <<std::endl;
                }  
                catch(std::exception &e){
                        std::cerr << "[WS ERROR] Exception in client run loop : " << e.what() <<std::endl;
                }
                catch(...){
                    std::cerr << "[WS ERROR] Unknown exception in client run loop " << std::endl;
                }
             });


             io_thread.join();


        }
        catch (const websocketpp::exception & e) {
            std::cerr << "[WS Error] WebSocket++ exception: " << e.what() << std::endl;
        } catch (const std::exception & e) {
            std::cerr << "[WS Error] Standard exception in start_deribit_ws: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "[WS Error] Unknown exception in start_deribit_ws." << std::endl;
        }
    }).detach(); //Detach the setup 

}