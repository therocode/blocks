/*
FreeBSD License

Copyright (c) 2010, Stefan Gatu

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "console.hpp"

//Setting the default color
//if platform is Windows
#ifdef _WIN32
    //set default values
    int Console::BGColor = 0;
    int Console::FGColor = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
    HANDLE Console::hConsole =  GetStdHandle(STD_OUTPUT_HANDLE);
    WORD wOldColorAttrs;
//if the plaform is other than Windows
#else
    string Console::BGColor = "30";
    string Console::FGColor = "47";
#endif

void Console::Initialise()
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
    GetConsoleScreenBufferInfo(hConsole, &csbiInfo);
    wOldColorAttrs = csbiInfo.wAttributes;
#endif
}

//Clears the console to the background color
void Console::Clear(){
    #ifdef _WIN32
        system("cls");
    #else
        cout << "\033[2J";
    #endif
}
//Sets the color(background and foreground)
void Console::SetColor(){
    #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, FGColor | BGColor);
    #else
        string clr = "\033[";
        clr += BGColor;
        clr += ";";
        clr += FGColor;
        clr += "m";
        cout << clr;
    #endif
}
//Print a string
void Console::Write(string str){
	#ifdef _MSC_VER
		cout << str.c_str();
	#else
		cout << str;
	#endif
}
//Print a int
void Console::Write(int v) {
    cout << v;
}
//Print a bool
void Console::Write(bool v){
    cout << (v?"TRUE":"FALSE");
}
//Print a char array
void Console::Write(char str[]){
    cout << str;
}
//Print a char
void Console::Write(char c){
    cout << c;
}
void Console::Write(double d){
    cout << d;
}
void Console::Write(float f){
    cout << f;
}

//Print a string and moves the cursor to the next line
void Console::WriteLine(string str){
	#ifdef _MSC_VER
		cout << str.c_str() << endl;
	#else
		cout << str << endl;
	#endif
}
//Moves the cursor to the next line
void Console::WriteLine(){
    cout << endl;
}
//Print a char array and moves the cursor to the next line
void Console::WriteLine(char str[]){
    cout << str <<endl;
}
//Read the first key user presses
int Console::ReadKey(){
    int val = 0;
    #ifdef _WIN32
		#ifdef _MSC_VER
			val = (int)_getch();
		#else
			val = (int)getch();
		#endif
    #else
        struct termios oldt,newt;
        tcgetattr( STDIN_FILENO, &oldt );
        newt = oldt;
        newt.c_lflag &= ~( ICANON | ECHO );
        tcsetattr( STDIN_FILENO, TCSANOW, &newt );
        val = getchar();
        tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    #endif
    if(val == 224)
        val += Console::ReadKey();
    return val;
}
bool Console::KeyDown(){
    #ifdef _WIN32
        #ifdef _MSC_VER
            return _kbhit()
        #else
            return kbhit();
        #endif
    #else
//        struct timeval tv;
//        fd_set rdfs;
//
//        tv.tv_sec = 0;
//        tv.tv_usec = 0;
//
//        FD_ZERO(&rdfs);
//        FD_SET (STDIN_FILENO, &rdfs);
//
//        select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
//        return FD_ISSET(STDIN_FILENO, &rdfs);

    #endif
    return false;
}
//Wait for user input and saves the input in a string
string Console::ReadLine(){
    string str;
    char c = getchar();
    while(c != '\n'){
        if(c == '\r')
            continue;
        else
            str+=c;
        c = getchar();
    }while(c != '\n');
    return str;
}
//Set the background color
void Console::SetBGColor(const short color){
    #ifdef _WIN32
    short val;
    switch(color){
        default:
        case (int)ConsoleColour::BLACK:
            val = 0;
        break;
        case (int)ConsoleColour::BLUE:
            val = BACKGROUND_BLUE;
        break;
        case (int)ConsoleColour::CYAN:
            val =  BACKGROUND_BLUE|BACKGROUND_GREEN;
        break;
        case (int)ConsoleColour::GREEN:
            val = BACKGROUND_GREEN;
        break;
        case (int)ConsoleColour::MAGENTA:
            val = BACKGROUND_BLUE|BACKGROUND_RED;
        break;
        case (int)ConsoleColour::RED:
            val = BACKGROUND_RED;
        break;
        case (int)ConsoleColour::WHITE:
            val = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;
        break;
        case (int)ConsoleColour::YELLOW:
            val = BACKGROUND_GREEN|BACKGROUND_RED;
        break;
    }
    BGColor = val;
    #else
    string val;
    switch(color){
        default:
        case (int)ConsoleColour::BLACK:
            val = "40";
        break;
        case (int)ConsoleColour::BLUE:
            val = "44";
        break;
        case (int)ConsoleColour::CYAN:
            val = "46";
        break;
        case (int)ConsoleColour::GREEN:
            val = "42";
        break;
        case (int)ConsoleColour::MAGENTA:
            val = "45";
        break;
        case (int)ConsoleColour::RED:
            val = "41";
        break;
        case (int)ConsoleColour::WHITE:
            val = "47";
        break;
        case (int)ConsoleColour::YELLOW:
            val = "43";
        break;
    }
    BGColor = val;
    #endif
    SetColor();
}
//Set the foreground color(or text color)
void Console::SetFGColor(const short color){

    #ifdef _WIN32
    int val;
    switch(color){
        default:
        case (int)ConsoleColour::BLACK:
            val = 0;
        break;
        case (int)ConsoleColour::BLUE:
            val = FOREGROUND_BLUE;
        break;
        case (int)ConsoleColour::CYAN:
            val =  FOREGROUND_BLUE|FOREGROUND_GREEN;
        break;
        case (int)ConsoleColour::GREEN:
            val = FOREGROUND_GREEN;
        break;
        case (int)ConsoleColour::MAGENTA:
            val = FOREGROUND_BLUE|FOREGROUND_RED;
        break;
        case (int)ConsoleColour::RED:
            val = FOREGROUND_RED;
        break;
        case (int)ConsoleColour::WHITE:
            val = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
        break;
        case (int)ConsoleColour::YELLOW:
            val = FOREGROUND_GREEN|FOREGROUND_RED;
        break;
    }
    FGColor = val;
    #else
    string val;
    switch(color){
        case (int)ConsoleColour::BLACK:
            val = "30";
        break;
        case (int)ConsoleColour::BLUE:
            val = "34";
        break;
        case (int)ConsoleColour::CYAN:
            val = "36";
        break;
        case (int)ConsoleColour::GREEN:
            val = "32";
        break;
        case (int)ConsoleColour::MAGENTA:
            val = "35";
        break;
        case (int)ConsoleColour::RED:
            val = "31";
        break;
        default:
        case (int)ConsoleColour::WHITE:
            val = "37";
        break;
        case (int)ConsoleColour::YELLOW:
            val = "33";
        break;
    }
    FGColor = val;
    #endif
    SetColor();
}

void Console::ResetColor()
{
    #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, wOldColorAttrs);
        //SetFGColor(ConsoleColour::WHITE);
    #else
        cout << "\033[0m";
    #endif
}

//Sets the console title
void Console::SetTitle(const string str){
    #ifdef _WIN32
        SetConsoleTitle(str.c_str());
    #else
        cout << "\033]0;\0" << str << "\x07\0";
    #endif
}
void Console::SetTitle(const char str[]){
    #ifdef _WIN32
        SetConsoleTitle(str);
    #else
        cout << "\033]0;\0" << str << "\x07\0";
    #endif
}
void Console::SetCursorPosition(int x,int y){
    #ifdef _WIN32
        COORD pos  = {(short)x,(short)y};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
    #else
		stringstream str;
		str << "\033[" << x << ";" << y << "H";
		cout << str.str();
	#endif
}
