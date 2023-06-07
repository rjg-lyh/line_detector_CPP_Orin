#include "tools.hpp"
#include "iomanip"

void ppColor(){
    printf("This is a character control test!\n" );
    printf("[%2u]" CLEAR "CLEAR\n" NONE, __LINE__);
    printf("[%2u]" BLACK "BLACK " L_BLACK "L_BLACK\n" NONE, __LINE__);
    printf("[%2u]" RED "RED " L_RED "L_RED\n" NONE, __LINE__);
    printf("[%2u]" GREEN "GREEN " L_GREEN "L_GREEN\n" NONE, __LINE__);
    printf("[%2u]" BROWN "BROWN " YELLOW "YELLOW\n" NONE, __LINE__);
    printf("[%2u]" BLUE "BLUE " L_BLUE "L_BLUE\n" NONE, __LINE__);
    printf("[%2u]" PURPLE "PURPLE " L_PURPLE "L_PURPLE\n" NONE, __LINE__);
    printf("[%2u]" CYAN "CYAN " L_CYAN "L_CYAN\n" NONE, __LINE__);
    printf("[%2u]" GRAY "GRAY " WHITE "WHITE\n" NONE, __LINE__);
    printf("[%2u]" BOLD "BOLD\n" NONE, __LINE__);
    printf("[%2u]" UNDERLINE "UNDERLINE\n" NONE, __LINE__);
    printf("[%2u]" BLINK "BLINK\n" NONE, __LINE__);
    printf("[%2u]" REVERSE "REVERSE\n" NONE, __LINE__);
    printf("[%2u]" HIDE "HIDE\n" NONE, __LINE__);
}

void printInfo(double time, int count, const char* seq, int state){
    if(state == 0)
        cout << PURPLE << left << setw(70 + count - 4)<< "|---------" + string(seq) + ": " + to_string(time) + "ms" << "|" << NONE << endl << endl;
    else
        cout << GREEN << left << setw(70 + count - 4)<< "|---------" + string(seq) + ": " + to_string(time) + "ms" << "|" << NONE << endl;    
}

void TimerClock::update(){
    _start = high_resolution_clock::now();
}
// 获取秒
double TimerClock::getTimeSecond(){
    return getTimeMicroSec()*0.000001;
}
// 获取毫秒
double TimerClock::getTimeMilliSec(){
    return getTimeMicroSec()*0.001;
}
// 获取微秒
long long TimerClock::getTimeMicroSec(){
    return duration_cast<microseconds>(high_resolution_clock::now() - _start).count();
}