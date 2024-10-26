#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "json.hpp"
#include <cstdlib>  // For system()

using json = nlohmann::json;
using namespace std;

// Link Utili:
/*
https://tomeko.net/online_tools/cpp_text_escape.php?lang=en     Da Mega Text a stringa
https://patorjk.com/software/taag/#p=display&f=Big&t=WELCOME%20TO%20%0A%0ARELICS%20%26%20RUINS      Per creare Mega Text
https://github.com/Gentle-Ego/Relics-and-Ruins      GitHub per Update
*/


const vector<string> RACES={"Human", "Elf", "Dwarf", "Orc", "Halfling"};
auto first = RACES.begin();
auto last = RACES.end();
const int START_COINS = 100;
const string SEX [2] = {"M", "F"};

// Funzione per pulire la schermata
void clearScreen() {
    #ifdef _WIN32
        system("cls");   // Windows
    #else
        system("clear"); // Linux/macOS
    #endif
}

void slowCout(const std::string& text, int delay_ms = 50) {
    for (char c : text) {
        std::cout << c << std::flush;  // Print each character and flush to display immediately
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));  // Delay between characters
    }
    std::cout << std::endl;  // Optional: print a newline after the whole string
}

string findPosition(int x){
    switch(x){
        case -1:
            return "Just Started";
        case 0:
            return "Capital";
        case 1:
            return "First Dungeon";
        case 2:
            return "Second Dungeon";
        case 3:
            return "Third Dungeon";
        case 4:
            return "Fourth Dungeon";
        case 5:
            return "Fifth Dungeon";
        case 6:
            return "Sixth Dungeon";
        case 7:
            return "Seventh Dungeon";
        case 8:
            return "Eighth Dungeon";
        case 9:
            return "Ninth Dungeon";
        case 10:
            return "Tenth Dungeon";
    }
    return "";
}

static int calculateRequiredExp(int level) {
    // Formula esponenziale base
    // Esempio: 100 * (level^1.5)
    return static_cast<int>(100 * pow(level, 1.5));
}

static int calculateLevel(int experience) {
    int level = 1;
    while (calculateRequiredExp(level) <= experience) {
        level++;
    }
    return level - 1;
}

class Character {
public:
    // Stats base
    string name, race, sex;
    int coins, level;
    int experience;
    int current_turn; //solo dentro dungeon         
    //in caso di combattimenti si aggiornano solo a fine combattimento per sicurezza di salvataggio
    int current_dungeon; //da 1 a (num dungeon) o 0 se capitale, -1 se si è all'inizio
    int pos_x;
    int pos_y;

    // Risorse
    int health, max_health;
    int current_food, max_food;
    int mana, max_mana;
    int mana_regeneration;

    // Stats combattimento
    int strength;
    int defense;
    int dexterity;
    double critical;

    // Inventario e equipaggiamento
    vector<json> inventory;
    json equipped;

    Character(string n, string r, string s) 
        : name(n), race(r), sex(s), 
          coins(START_COINS), level(1), experience(0),
          current_turn(0), current_dungeon(-1), pos_x(0), pos_y(0),
          health(100), max_health(100),
          current_food(100), max_food(100),
          mana(50), max_mana(50), mana_regeneration(1),
          strength(10), defense(10), dexterity(10), critical(0.1){}
};

void write_character_to_json(Character charac) {
    // Crea il JSON del personaggio con tutti i nuovi attributi
    json character = {
        // Stats base
        {"name", charac.name},
        {"race", charac.race},
        {"sex", charac.sex},
        {"coins", charac.coins},
        {"level", charac.level},
        {"experience", charac.experience},

        // Posizione
        {"current_turn", charac.current_turn},
        {"current_dungeon", charac.current_dungeon},
        {"pos_x", charac.pos_x},
        {"pos_y", charac.pos_y},

        // Risorse
        {"health", charac.health},
        {"max_health", charac.max_health},
        {"current_food", charac.current_food},
        {"max_food", charac.max_food},
        {"mana", charac.mana},
        {"max_mana", charac.max_mana},
        {"mana_regeneration", charac.mana_regeneration},

        // Stats combattimento
        {"strength", charac.strength},
        {"defense", charac.defense},
        {"dexterity", charac.dexterity},
        {"critical", charac.critical},

        // Inventario e equipaggiamento
        {"inventory", charac.inventory},
        {"equipped", charac.equipped},
    };

    // Carica i personaggi esistenti se il file esiste
    json characters;
    ifstream input_file("characters.json");
    if (input_file.is_open()) {
        input_file >> characters;
        input_file.close();
    } else {
        characters["characters"] = json::array();
    }

    // Aggiungi o aggiorna il personaggio
    bool character_exists = false;
    for (auto& existing_char : characters["characters"]) {
        if (existing_char["name"] == charac.name) {
            existing_char = character;
            character_exists = true;
            break;
        }
    }

    if (!character_exists) {
        characters["characters"].push_back(character);
    }

    // Salva il file JSON aggiornato
    ofstream output_file("characters.json");
    if (output_file.is_open()) {
        output_file << characters.dump(4);
        output_file.close();
        cout << "Character saved to characters.json!" << endl;
    } else {
        cerr << "Error: Could not open file for writing!" << endl;
    }
}

// Funzione per convertire da JSON a oggetto Character
Character fromJSONtoCharacter(json ch) {
    // Crea un nuovo personaggio con i dati base
    Character c(ch["name"], ch["race"], ch["sex"]);
    
    // Aggiorna tutti gli altri attributi
    // Stats base
    c.coins = ch["coins"];
    c.level = ch["level"];
    c.experience = ch["experience"];

    // Posizione
    c.current_turn = ch["current_turn"];
    c.current_dungeon = ch["current_dungeon"];
    c.pos_x = ch["pos_x"];
    c.pos_y = ch["pos_y"];

    // Risorse
    c.health = ch["health"];
    c.max_health = ch["max_health"];
    c.current_food = ch["current_food"];
    c.max_food = ch["max_food"];
    c.mana = ch["mana"];
    c.max_mana = ch["max_mana"];
    c.mana_regeneration = ch["mana_regeneration"];

    // Stats combattimento
    c.strength = ch["strength"];
    c.defense = ch["defense"];
    c.dexterity = ch["dexterity"];
    c.critical = ch["critical"];

    // Inventario e equipaggiamento
    c.inventory = ch["inventory"].get<vector<json>>();
    c.equipped = ch["equipped"];

    return c;
}

void start_game(Character character)
{
    string placeholder="";
    //Aspetta 5 secondi prima di iniziare il gioco
    this_thread::sleep_for(chrono::seconds(5));
    clearScreen();

    //mettere il giocatore nel posto ultimo salvato, o iniziare con introduzione se current_dungeon == -1
    if(character.current_dungeon == -1){
        slowCout(" __          ________ _      _____ ____  __  __ ______   _______ ____                 \n \\ \\        / /  ____| |    / ____/ __ \\|  \\/  |  ____| |__   __/ __ \\                \n  \\ \\  /\\  / /| |__  | |   | |   | |  | | \\  / | |__       | | | |  | |               \n   \\ \\/  \\/ / |  __| | |   | |   | |  | | |\\/| |  __|      | | | |  | |               \n    \\  /\\  /  | |____| |___| |___| |__| | |  | | |____     | | | |__| |               \n  ___\\/  \\/___|______|______\\_____\\____/|_|  |_|______|___ |_|  \\____/__ _   _  _____ \n |  __ \\|  ____| |    |_   _/ ____|/ ____|   ___    |  __ \\| |  | |_   _| \\ | |/ ____|\n | |__) | |__  | |      | || |    | (___    ( _ )   | |__) | |  | | | | |  \\| | (___  \n |  _  /|  __| | |      | || |     \\___ \\   / _ \\/\\ |  _  /| |  | | | | | . ` |\\___ \\ \n | | \\ \\| |____| |____ _| || |____ ____) | | (_>  < | | \\ \\| |__| |_| |_| |\\  |____) |\n |_|  \\_\\______|______|_____\\_____|_____/   \\___/\\/ |_|  \\_\\\\____/|_____|_| \\_|_____/ \n                                                                                      \n                                                                                      ", 2);

        slowCout("It's a brisk morning, and the first rays of sunlight begin to warm the chilly air as you make your way to the association. The path is familiar, but today, every step feels heavier, charged with anticipation. After years of waiting, you're finally here, standing at the threshold, 18 and ready to join.\nThe building stands tall and welcoming, with the association's emblem proudly displayed by the entrance. You take a deep breath and step inside, feeling a strange mix of nerves and excitement. The reception area is bustling, with people chatting and moving about, each seemingly caught up in their own purpose. You feel an odd sense of belonging, this is where you've always wanted to be, and today, it's happening.\nApproaching the front desk there is a white haired old man waiting, you hand over your ID with a subtle grin, savoring the moment. The receptionist smiles knowingly, having seen this scene many times before, and says,\n\n \"Happy birthday! Excited to finally join?\"\n\nA rush of pride washes over you as you nod, and he gestures toward a set of double doors at the end of the hall.\nYou walk through, and the room beyond has an almost ceremonial feel. You see walls lined with framed photos of previous members, a legacy of sorts, and you feel a connection to the history, as though your name, too, will someday join those ranks, becoming a DUNGEONS CLEARER!\nThe official enrollment process is straightforward but significant: signing your name in the registry, filling out some final paperwork, and confirming your dedication to the association's values. When you finish, they hand you a membership badge with your name engraved on it, still warm from the print. It feels real, solid—an achievement.");
        system("pause"); 
        cout<<"\n\n";
        character.inventory.push_back("Association Badge");

        slowCout("");
    } else if(character.current_dungeon == 0){

    } else {

    }
}

void select_char()
{
    string scelta="";
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

                clearScreen();
                // Stampo i vari personaggi chiedendo fra quali scegliere
                cout << "-------------------\nSELECT YOUR CHARACTER!\n-------------------" << endl;
                for (const auto& character : characters["characters"]) {
                    cout << "Name: " << character["name"] << endl;
                    cout << "Race: " << character["race"] << endl;
                    cout << "Sex: " << character["sex"] << endl;
                    cout << "Coins: " << character["coins"] << endl;
                    cout << "Level: " << character["level"] << endl;
                    cout << "Position: " << findPosition(character["current_dungeon"]) << endl;
                    cout << "-------------------" << endl;
                }

                bool x = true;
                do{
                cout << "\nWhat's the name of the choosen character?\n> ";
                cin >> char_name;
                    for (const auto& character : characters["characters"]){
                        if(character["name"]==char_name){
                            Character chosen_char = fromJSONtoCharacter(character);
                            clearScreen();
                            cout << "You selected: " << chosen_char.name << " (Level " << chosen_char.level << ")\n";
                            x = false;
                            start_game(chosen_char);
                        }
                    }
                }while(x);

            } else
            {
                clearScreen();
                cout << "No characters found. Please start a new game." << endl;
                select_char();
            }
        } else if(scelta=="YES") 
        {
            string n, r, s;

            cout << "Create your character:\nName: ";
            cin >> n;

            do{
                cout << "Race (select from Human, Elf, Dwarf, Orc, Halfling): ";
                cin >> r;
            }while(find(first, last, r)==RACES.end());

            do{
                cout << "Sex (insert M or F): ";
                cin >> s;
            }while(s!="M" && s!="F");

            Character chosen_char(n, r, s);

            clearScreen();
            cout << "You created: " << chosen_char.name << " (Level " << chosen_char.level << ")\n";
            write_character_to_json(chosen_char);
            start_game(chosen_char);
        }
    }while(scelta!="YES" && scelta!="NO");
}

int main()
{
    clearScreen();
    select_char();
    return 0;
}
