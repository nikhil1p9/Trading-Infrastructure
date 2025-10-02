#pragma once
#include <string>

class app {

public:
    app();
    void app_init();
    static std::string spaceremover(std::string & s);
    
private:

    static void PrintScreen();
    static int getuserinput();
    static void processinput(int input);
    static void option1();
    static void option2();
    static void option3();
    static void option4();
    static void option5();
    static void option6();
    static void option7();

};