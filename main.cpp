#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;


const string RACES[5]={"Human", "Elf", "Dwarf", "Orc", "Halflingl"};
const int START_COINS = 10;
const char SEX [2] = {'M', 'F'};

void write_character_to_json(const string& name, const string& race, char sex, int coins) {
    json character = {
        {"name", name},
        {"race", race},
        {"sex", string(1, sex)},  // Convertire char a string per JSON
        {"coins", coins}
    };

    // Carica i personaggi esistenti se il file esiste, altrimenti crea un nuovo JSON
    json characters;
    ifstream input_file("characters.json");
    if (input_file.is_open()) {
        input_file >> characters;
        input_file.close();
    } else {
        characters["characters"] = json::array();
    }

    // Aggiungi il nuovo personaggio all'array dei caratteri
    characters["characters"].push_back(character);

    // Riscrivi il JSON aggiornato nel file
    ofstream output_file("characters.json");
    if (output_file.is_open()) {
        output_file << characters.dump(4);  // Formattazione leggibile, con 4 spazi (tab)
        output_file.close();
        cout << "Character added and saved to characters.json!" << endl;
    }
}

void start_game()
{
    string scelta;
    do{
        cout << "Do you want to start from scratch? (YES or NO)" << endl;
        cin >> scelta;
        if(scelta=="NO")
        {
            json characters;
            ifstream char_file("characters.json");
            if(char_file.is_open())
            {
                char_file >> characters;
                char_file.close();
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
