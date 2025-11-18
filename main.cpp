#include <iostream>
#include <cmath>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdlib>
using namespace std;
namespace fs = std::filesystem;

void wait(int ms){
    this_thread::sleep_for(chrono::milliseconds(ms));
}

void throw_error(string name){
    cout << "ERROR: " << name << '\n';
}

void lowercase(string &word1){
    for(char &c: word1) c = tolower(c);
}

bool file_exists(string name){
    fstream file(name);
    return (bool)(file);
}

void writeto(string name, string content){
    ofstream file(name);
    file << content;
    file.close();
}

string readfrom(string name){
    ifstream file(name);
    if(!file) writeto(name,""); //creates empty file

    string line, result = "";
    while(getline(file,line))
        result += line + '\n';

    return result;
}

void open_file(string name){
    if(!file_exists(name)) {
        throw_error("FILE NOT FOUND");
        return;
    }
#ifdef _WIN32
    std::string command = "start \"\" \"" + name + "\"";
    system(command.c_str());
#elif __APPLE__
    std::string command = "open \"" + name + "\"";
    system(command.c_str());
#else
    std::string command = "xdg-open \"" + name + "\"";
    system(command.c_str());
#endif
}

void take_input(){
    cout << "> ";
    string modified,equals,op; //name of modified file, equal sign (sometimes different)
    cin >> modified >> equals;

    //exceptions
    if(modified == "open"){
        open_file(equals);
        return;
    }
    else if(modified == "new" || modified == "clear"){
        writeto(equals,"");
        return;
    }
    else if(modified == "delete" || modified == "kill"){
        if(!fs::remove(equals)) throw_error("FILE NOT FOUND");
    }
    else{
        cin >> op; //operation
        lowercase(op);
        if(op == "new" || op == "clear")
            writeto(modified,"");
        else if(op == "delete" || op == "kill"){
            if(!fs::remove(modified)) throw_error("FILE NOT FOUND");
        }
        else if(op == "open") open_file(modified);
        else if(op == "merge"){
            string name1, name2;
            cin >> name1 >> name2;
            writeto(modified, readfrom(name1) + readfrom(name2));
        }
        else if(op == "copy"){
            string name;
            cin >> name;
            writeto(modified, readfrom(name));
        }
        else throw_error("UNKNOWN COMMAND");
    }
    cin.ignore();
}

int main(){
    while(true) take_input();
}
