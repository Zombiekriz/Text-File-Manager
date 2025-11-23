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

const int TXT = 1, EXE = 2, NOTYPE=0;
int file_type(string &s){
    if(s.size()<4) return NOTYPE;
    string type = s.substr(s.size()-4,4);
    if(type == ".txt") return TXT;
    if(type == ".exe") return EXE;
    return NOTYPE;
}

const int ALPHAMIN = 32, ALPHAMAX = 127, ALPHADIF = ALPHAMAX-ALPHAMIN+1;
void crypt(string dname, string kname, bool en){
    string content = readfrom(dname), key = readfrom(kname);
    vector<int> pref(key.size()), suf(key.size());
    pref[0] = key[0];
    suf[key.size()-1] = key.back();
    for(int i=1;i<key.size();i++){
        pref[i] = (pref[i-1]+key[i])%(ALPHADIF);
        suf[key.size()-i-1] = (suf[key.size()-i]+key[key.size()-i-1])%(ALPHADIF);
     }
    for(int i=0;i<content.size();i++){
        if(content[i]=='\n') continue;
        int ki = i%key.size(), c = content[i];
        if(en){ //bullshit ass encryption i dont even know how reversing it works exactly
            c ^= (pref[ki]+suf[ki])%ALPHADIF;
            c = (c - ALPHAMIN + ALPHADIF)%ALPHADIF;
            c += (suf[ki]*pref[ki] + (unsigned int)(i*i))%ALPHADIF;
            c %= ALPHADIF;
            c += ALPHAMIN;
        }
        else{
            c -= ALPHAMIN;
            c = (c - (pref[ki]*suf[ki] + (unsigned int)(i*i))%ALPHADIF + ALPHADIF) % ALPHADIF;
            c ^= (pref[ki]+suf[ki])%ALPHADIF;
            c += ALPHAMIN;
        }
        content[i] = c;
    }
    writeto(dname, content);
}

void take_input(vector<string> &v){
    cout << "> ";
    string line,word="";
    getline(cin,line);
    for(char c: line){
        if(isspace(c)) {v.push_back(word); word = "";}
        else word += tolower(c);
    }
    if(!isspace(line.back())) v.push_back(word);
}

string fromv(vector<string> &v, int index){
    if(index >= v.size()) {throw_error("INCOMPLETE LOGIC"); return "";}
    else return v[index];
}

void process_input(){
    vector<string> v;
    take_input(v);
    if(fromv(v,0).empty()) return;

    if(file_type(v[0]) == EXE) throw_error("INVALID LOGIC");
    else if(file_type(v[0]) == NOTYPE){ //shortcuts
        if(v[0] == "exit") exit(0);

        if(fromv(v,1).empty()) return;
        if(v[0] == "new" || v[0] == "create" || v[0] == "clear"){
            if(file_type(v[1]) != TXT) {throw_error("WRONG FILE TYPE"); return;}
            writeto(v[1],"");
        }
        else if(v[0] == "kill" || v[0] == "delete"){
            if(file_type(v[1]) != TXT) {throw_error("WRONG FILE TYPE"); return;}
            if(!fs::remove(v[1])) throw_error("FILE NOT FOUND");
        }
        else if(v[0] == "open"){
            if(file_type(v[1]) != TXT) {throw_error("WRONG FILE TYPE"); return;}
            open_file(v[1]);
        }
        else throw_error("UNKNOWN COMMAND");
    }
    else if(file_type(v[0]) == TXT){
        if(fromv(v,1).empty()) return;
        if(v[1] != "=") {throw_error("INVALID LOGIC"); return;}

        if(fromv(v,2).empty()) return;
        if(file_type(v[2]) == TXT) writeto(v[0],readfrom(v[2]));
        else{
            if(v[2] == "new" || v[2] == "create" || v[3] == "clear")
                writeto(v[0],"");
            else if(v[2] == "kill" || v[2] == "delete"){
                if(!fs::remove(v[0])) throw_error("FILE NOT FOUND");
            }
            else if(v[2] == "open")
                open_file(v[0]);
            else if(v[2] == "merge" || v[2] == "+"){
                if(fromv(v,4).empty()) return;
                if(file_type(v[3]) != TXT || file_type(v[4]) != TXT) {throw_error("WRONG FILE TYPE"); return;}
                writeto(v[0], readfrom(v[3])+readfrom(v[4]));
            }
            else if(v[2] == "encrypt" || v[2] == "decrypt"){
                if(fromv(v,3).empty()) return;
                if(file_type(v[3]) != TXT) {throw_error("WRONG FILE TYPE"); return;}
                crypt(v[0],v[3],v[2]=="encrypt");
            }
            else throw_error("UNKNOWN COMMAND");
        }
    }
}

int main(){
    while(true) process_input();
}
