#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;


const string RACES[5]={"Human", "Elf", "Dwarf", "Orc", "Halflingl"};
const int START_COINS = 10;
const char SEX [2] = {'M', 'F'};

void start_game()
{
    string scelta;
    do{
        cout << "Do you want to start from scratch? (YES or NO)" << endl;
        cin >> scelta;
        if(scelta=="NO")
        {
            if(/*c'è qualcosa in characters*/)
            {
                //seleziona quale si vuole
            }
        } else if(scelta=="YES") 
        {
            cout<<"Create your character:\n> ";
        }
    }while(scelta!="YES" || scelta!="NO")
}

class Character
{
    string name, race;
    char sex;
    int coins;
    Character () {}
    Character (string n, string r, char s) 
    {
        name=n;
        race=r;
        sex=s;
        coins=START_COINS;
    }
};

int main()
{
    start_game();

    return 0;
}
