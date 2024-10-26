#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;


const vector<string> RACES={"Human", "Elf", "Dwarf", "Orc", "Halfling"};
auto first = RACES.begin();
auto last = RACES.end();
const int START_COINS = 100;
const string SEX [2] = {"M", "F"};

class Character
{
public:
    string name, race, sex;
    int coins, level;

    // Costruttore di default
    Character() : coins(START_COINS), level(1) {}

    // Costruttore parametrico
    Character(string n, string r, string s, int c = START_COINS, int l = 1)
        : name(n), race(r), sex(s), coins(c), level(l) {}
};

void write_character_to_json(Character charac) {
    json character = {
        {"name", charac.name},
        {"race", charac.race},
        {"sex", charac.sex},
        {"coins", charac.coins},
        {"level", charac.level}
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

// Funzione per convertire da JSON a oggetto Character
Character fromJSONtoCharacter(json ch)
{
    Character c(ch["name"], ch["race"], ch["sex"], ch["coins"], ch["level"]);
    return c;
}

void start_game(Character character)
{
    
}

void select_char(string scelta="")
{
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
                string char_name;

                // Stampo i vari personaggi chiedendo fra quali scegliere
                cout << "\n\n-------------------\nSELECT YOUR CHARACTER!\n-------------------" << endl;
                for (const auto& character : characters["characters"]) {
                    cout << "Name: " << character["name"] << endl;
                    cout << "Race: " << character["race"] << endl;
                    cout << "Sex: " << character["sex"] << endl;
                    cout << "Coins: " << character["coins"] << endl;
                    cout << "Level: " << character["level"] << endl;
                    cout << "-------------------" << endl;
                }


                cout << "\nWhat's the name of the choosen character?\n> ";
                cin >> char_name;
                for (const auto& character : characters["characters"]){
                    if(character["name"]==char_name){
                        Character chosen_char = fromJSONtoCharacter(character);
                        cout << "You selected: " << chosen_char.name << " (Level " << chosen_char.level << ")\n";
                        start_game(chosen_char);
                    }
                }

            } else
            {
                cout << "No characters found. Please start a new game." << endl;
                string sc="YES";
                select_char(sc);
            }
        } else if(scelta=="YES") 
        {
            Character chosen_char;
            string r, s;

            cout << "Create your character:\nName: ";
            cin >> chosen_char.name;

            do{
                cout << "Race (select from Human, Elf, Dwarf, Orc, Halfling): ";
                cin >> r;
            }while(find(first, last, r)==RACES.end());
            chosen_char.race = r;

            do{
                cout << "Sex (insert M or F): ";
                cin >> s;
            }while(s!="M" && s!="F");
            chosen_char.sex = s;

            cout << "You created: " << chosen_char.name << " (Level " << chosen_char.level << ")\n";
            write_character_to_json(chosen_char);
            start_game(chosen_char);
        }
    }while(scelta!="YES" && scelta!="NO");
}

int main()
{
    select_char();
    return 0;
}
