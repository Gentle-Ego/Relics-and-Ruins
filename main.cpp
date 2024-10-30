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

const vector<string> RACES={"Human", "Elf", "Dwarf", "Orc", "Halfling", "Tiefling", "Gnome", "Goblin", "Kobold", "Hobbit"};
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

void slowCout(const string& text, int delay_ms = 40) {
    for (char c : text) {
        cout << c << flush;  // Print each character and flush to display immediately
        this_thread::sleep_for(chrono::milliseconds(delay_ms));  // Delay between characters
    }
    cout;  // Optional: print a newline after the whole string
}

string stringToLower(const string& str)
{
    string result = "";

    for (char ch : str) {
        // Convert each character to lowercase using tolower
        result += tolower(ch);
    }

    return result;
}

string findPosition(int x){
    switch(x){
        case -3:
            return "Monster Association";
        case -2:
            return "Shops Plaza";
        case -1:
            return "Tutorial";
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
    int current_dungeon; //da 1 a (num dungeon) o 0 se capitale, -1 se si è all'inizio, -2 se Shop, -3 se MHA
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
    vector<json> equipped;

    Character(string n, string r, string s) 
        : name(n), race(r), sex(s), 
          coins(START_COINS), level(1), experience(0),
          current_turn(0), current_dungeon(-1), pos_x(0), pos_y(0),
          health(100), max_health(100),
          current_food(100), max_food(100),
          mana(50), max_mana(50), mana_regeneration(1),
          strength(10), defense(10), dexterity(10), critical(0.1){}

    // Aggiungi un singolo oggetto
    void addItem(const json& item, Character& character) {
        if (!item.contains("type") || !item.contains("name")) {
            throw invalid_argument("Item must contain 'type' and 'name' fields");
        }
        inventory.push_back(item);
        write_character_to_json(character);
    }

    // Aggiungi molteplici oggetti
    void addItems(const vector<json>& items, Character& character) {
        for (const auto& item : items) {
            addItem(item, character);
        }
        write_character_to_json(character);
    }
    
    // Elimina un oggetto
    bool removeItem(const string& itemName, Character& character) {
        auto it = find_if(inventory.begin(), inventory.end(),
            [&itemName](const json& item) {
                return item["name"] == itemName;
            });

        if (it != inventory.end()) {
            inventory.erase(it);
            write_character_to_json(character);
            return true;
        }
        return false;
    }

    // Controlla se l'oggetto esiste
    bool hasItem(const string& itemName) const {
        return any_of(inventory.begin(), inventory.end(),
            [&itemName](const json& item) {
                return item["name"] == itemName;
            });
    }

    // Trova numero di stesso oggetto
    int getItemTypeCount(const string& itemType) const {
        return count_if(inventory.begin(), inventory.end(),
            [&itemType](const json& item) {
                return item["type"] == itemType;
            });
    }

	vector<json> findItemsType(const string& itemType){
		vector<json> items;
        string selection = itemType.substr(0, itemType.length() - 6);	// Togliere il .json e la s di multiple armors.json -> armor
        for (const auto& item : inventory) {
            // Controlla se l'oggetto ha un level_required minore o uguale al livello del giocatore
            if (item.contains("type") && item["type"] == "selection") {
                items.push_back(item);
            }
        }
    	return items;
	}

	vector<json> findItemsName(const string& itemName){
		vector<json> items;
        for (const auto& item : inventory) {
            // Controlla se l'oggetto ha un level_required minore o uguale al livello del giocatore
            if (item.contains("name") && item["name"] == "itemName") {
                items.push_back(item);
            }
        }
    	return items;
	}

    void write_character_to_json(Character& charac) {
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

};

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
    c.equipped = ch["equipped"].get<vector<json>>();

    return c;
}


// Funzione per leggere il file JSON del negozio e visualizzare gli oggetti con livello appropriato
vector<json> loadShopItems(const string& filename, int lvl) {
    vector<json> items;
    ifstream file(filename);
    if (file.is_open()) {
        json data;
        file >> data;
        string selection = filename.substr(0, filename.length() - 5);
        for (const auto& item : data[selection]) {
            // Controlla se l'oggetto ha un level_required minore o uguale al livello del giocatore
            if (!item.contains("level_required") || item["level_required"] <= lvl) {
                items.push_back(item);
            }
        }
        file.close();
    } else {
        cerr << "Error loading shop file: " << filename << endl;
    }
    return items;
}

// Funzione per il negozio
void shop(Character& character) {
    character.current_dungeon = -2;
    string filename, option;
    shop:
    clearScreen();
    slowCout("Welcome to the shops area! Choose a shop to visit:\n");
    slowCout("1. DragonForge Armory\n2. The Weapons of Valoria\n3. The Alchemist's Kiss\n4. Feast & Famine\n5. Relics & Rarities\n6. The Rusty Nail\n");
    cout << "\nSelect a number (or 'exit' to leave the shop): ";
    int choice;
    cin >> choice;
	clearScreen();

    switch (choice) {
        case 1: filename = "armors.json"; slowCout("Welcome to Dragon Forge\n"); break;
        case 2: filename = "weapons.json"; slowCout("Welcome to The Weapons of Valoria\n"); break;
        case 3: filename = "foods.json"; slowCout("Welcome to The Alchemist's Kiss\n"); break;
        case 4: filename = "potions.json"; slowCout("Welcome to Feast & Famine\n"); break;
        case 5: filename = "usables.json"; slowCout("Welcome to Relics & Rarities\n"); break;
        case 6: filename = "utilities.json"; slowCout("Welcome to The Rusty Nail\n"); break;
        default: cout << "Exiting shop.\n"; this_thread::sleep_for(chrono::seconds(4));; character.current_dungeon=0; main_menu(character); return;
    }

	do{
		slowCout("Would you like to Buy or Sell?\n");
		cin >> option;
	} while(option != "Buy" && option != "Sell");
    shopx:

	if(option == "Buy"){
		vector<json> items = loadShopItems(filename, character.level);

    	// Visualizza gli oggetti disponibili da comprare
	    cout << "Available items at your current level:\n";
 	    for (size_t i = 0; i < items.size(); ++i) {
    	    cout << i + 1 << ". " << items[i]["name"] << " - " << items[i]["value"] << " coins\n";
    	}
	} else {
		vector<json> items = character.findItemsType(filename);

		// Visualizza gli oggetti disponibili da vendere
	    cout << "Available items in your inventory, that can be sell in this shop:\n";
 	    for (size_t i = 0; i < items.size(); ++i) {
    	    cout << i + 1 << ". " << items[i]["name"] << " - " << items[i]["value"] << " coins, selling price: " << item[i]["value"]*0.75 << "\n";	// Oggetto vendibile al .75 del valore
    	}
	}
    // Selezione e acquisto
    int itemChoice;
    cout << "Enter the number of the item to "<< option <<" (or 0 to exit): ";
    cin >> itemChoice;

    if (itemChoice > 0 && itemChoice <= items.size()) {
        json selectedItem = items[itemChoice - 1];
		if(option == "Buy"){
			if (character.coins >= selectedItem["value"]) {
				character.coins -= int(selectedItem["value"]);
        		character.addItem(selectedItem, character);
				clearScreen();
 	            cout << "Purchased " << selectedItem["name"] << " for " << selectedItem["value"] << " coins!\n";
   	            goto shopx;
			} else {
            clearScreen();
            cout << "Not enough coins to buy " << selectedItem["name"] << ".\n";
            goto shopx;
        	}
		} else {
			character.coins += int(selectedItem["value"])*0.75;
       		character.removeItem(selectedItem["name"], character);
			clearScreen();
            cout << "Sold " << selectedItem["name"] << " for " << selectedItem["value"]*0.75 << " coins!\n";
			goto shopx;
		}
    } else {
        cout << "Exiting shop.\n";
        goto shop;
    }
}

void main_menu(Character character){
	character["current_dungeon"] = 0;
	clearScreen();
	

}

void start_game(Character character)
{
    string tutChoice;
    //Aspetta 5 secondi prima di iniziare il gioco
    this_thread::sleep_for(chrono::seconds(4));
    clearScreen();

    //mettere il giocatore nel posto ultimo salvato, o iniziare con introduzione se current_dungeon == -1
    if(character.current_dungeon == -1){
        slowCout(" __          ________ _      _____ ____  __  __ ______   _______ ____                 \n \\ \\        / /  ____| |    / ____/ __ \\|  \\/  |  ____| |__   __/ __ \\                \n  \\ \\  /\\  / /| |__  | |   | |   | |  | | \\  / | |__       | | | |  | |               \n   \\ \\/  \\/ / |  __| | |   | |   | |  | | |\\/| |  __|      | | | |  | |               \n    \\  /\\  /  | |____| |___| |___| |__| | |  | | |____     | | | |__| |               \n  ___\\/  \\/___|______|______\\_____\\____/|_|  |_|______|___ |_|  \\____/__ _   _  _____ \n |  __ \\|  ____| |    |_   _/ ____|/ ____|   ___    |  __ \\| |  | |_   _| \\ | |/ ____|\n | |__) | |__  | |      | || |    | (___    ( _ )   | |__) | |  | | | | |  \\| | (___  \n |  _  /|  __| | |      | || |     \\___ \\   / _ \\/\\ |  _  /| |  | | | | | . ` |\\___ \\ \n | | \\ \\| |____| |____ _| || |____ ____) | | (_>  < | | \\ \\| |__| |_| |_| |\\  |____) |\n |_|  \\_\\______|______|_____\\_____|_____/   \\___/\\/ |_|  \\_\\\\____/|_____|_| \\_|_____/ \n                                                                                      \n                                                                                      ", 1);
		slowCout("Would you like to skip the tutorial?");
		cin >> tutChoice;
		if(stringToLower(tutChoice) == "yes"){
			main_menu(character);
		}

        slowCout("\nIt's a brisk morning, and the first rays of sunlight begin to warm the chilly air as you make your way to the association. The path is familiar, but today, every step feels heavier, charged with anticipation. After years of waiting, you're finally here, standing at the threshold, 18 and ready to join.\nThe building stands tall and welcoming, with the association's emblem proudly displayed by the entrance. You take a deep breath and step inside, feeling a strange mix of nerves and excitement. The reception area is bustling, with people chatting and moving about, each seemingly caught up in their own purpose. You feel an odd sense of belonging, this is where you've always wanted to be, and today, it's happening.\nApproaching the front desk there is a red haired cute girl waiting, you hand over your ID with a subtle grin, savoring the moment. The receptionist smiles knowingly, having seen this scene many times before, and says,\n\n \"Happy birthday! Excited to finally join? My name's Rosie, and I'll be your guide through the new chapter of your life!\nFollow me, we have to finish some formal paper works, then I'll be honored to let you know in depth your job and how to do it well!\"\n\nA rush of pride washes over you as you nod, and she gestures toward a set of double doors at the end of the hall.\nYou walk through, and the room beyond has an almost ceremonial feel. You see walls lined with framed photos of previous members, a legacy of sorts, and you feel a connection to the history, as though your name, too, will someday join those ranks, becoming a DUNGEONS CLEARER!\nThe official enrollment process is straightforward but significant: signing your name in the registry, filling out some final paperwork, and confirming your dedication to the association's values. When you finish, Rosie hands you a membership badge with your name engraved on it, still warm from the print. It feels real, solid—an achievement.");
        if(!character.hasItem("Association Badge"){
            character.addItem({{"type", "badge"}, {"name", "Association Badge"}}, character);}
        cout << "\n\nPress ENTER to continue...\n";
        cin.ignore();
        clearScreen();

        slowCout("Rosie:\n\"Well, welcome to the Association "+character.name+"!\nNow let me introduce you to the association, as you may know, your objective is to clear the dungeons that are in the basement, do not worry I'll show you them as soon as we can. To clear those dungeons you'll need to defeat the Dungeon's keeper, you may see it like a boss of the dungeon, they are very strong creatures able to destroy one's career in a matter of seconds.\nDo not worry, the other monsters of the dungeons are very easy to beat, as long as they not come in horde, because they are like insects, weak alone, strong in group.\n\nNow, you caneasily understand that killing those monsters is not an easy task without the proper equipment. For this reason we of the association will give you 100 Coins to buy your first armor, weapon and maybe utility.\nSince we are in the capital Eràn you'll be able to find many shops and stores of any kind, you may chose your equipment according to yourfighting style.\n\nOk now, please take a rest in our motel, The Golden Pidgeon, tomorrow I'll help you find the shops, and after the dungeons.\"");
        cout << "\n\nPress ENTER to continue...\n";
        cin.ignore();
        clearScreen();
        slowCout("Sleeping, at the Golden Pidgeon", 100);
        slowCout(". . . . . . . .", 1000);
        cout << "\n\nPress ENTER to wake up...";
        cin.ignore();
        clearScreen();

        slowCout("The soft morning light seeps through the small window of your room at The Golden Pigeon, casting a gentle glow over the simple but cozy space. Stretching, you feel the excitement and nerves of the previous day slowly start to return. Today's the day you'll begin your first steps as a Dungeon Clearer.\n\nAfter freshening up, you make your way downstairs, greeted by the sounds and smells of a bustling morning. You spot Rosie by the entrance, waiting with a welcoming smile.\n\n");
        slowCout("Rosie:\n\"Good morning! Did you sleep well? It's a big day today,\" she says with a grin. \"Now, let me take you on a short tour around Eràn, and then we'll head down to the association basement where the dungeons are located.\"\n\n");
        cout << "\n\nPress ENTER to continue...\n";
        cin.ignore();
        clearScreen();

        slowCout("You follow Rosie out of The Golden Pigeon and into the heart of Eràn, the capital city. The streets are bustling with people of all kinds: merchants calling out their goods, adventurers gathered in small groups, and children running around, eyes wide with wonder. Rosie points out several important spots as you go, including the various guilds, a few inns, and, most importantly, the local armory and supply shops.\n\n");
        slowCout("Rosie:\n\"As a Monster Hunter, you'll want to invest wisely in your equipment. This is where you'll find everything from weapons to potions and magical items. Just remember, your first purchase should match your style, whether it's something more defensive or offensive.\"\n\n");
        cout << "\n\nPress ENTER to continue...\n";
        cin.ignore();
        clearScreen();

        slowCout("After the short tour, Rosie leads you back to the association building. Passing through the main hall, you reach a set of thick, reinforced doors marked with an emblem. Rosie pushes them open, revealing a dark staircase descending into the basement.\n\n");
        slowCout("Rosie:\n\"Here's where your journey begins,\" she says with a note of pride. \"The dungeons lie below, and each dungeon has its own unique challenges. The first one isn't as tough as the others, but the Dungeon Keeper still poses a significant threat.\"\n\n");
        slowCout("You feel the weight of your responsibility settling on your shoulders. The basement air is cool and smells faintly of stone and earth, as if centuries of battles and challenges have left their mark on the atmosphere. You take a deep breath, preparing yourself.\n\n");

        cout << "\n\nPress ENTER to continue...\n";
        cin.ignore();
        clearScreen();
        slowCout("Rosie:\n\"But first, you'll need the right equipment. The association has provided you with 100 Coins, so head over to the shops and make sure you're well prepared. Once you're ready, come back here, and I'll show you the way to the dungeon entrance.\"\n\n");

        cout << "\n\nPress ENTER to head to the shops...\n";
        cin.ignore();
        clearScreen();

        shop(character);

        main_menu(character);

    } else if(character.current_dungeon == 0){
        main_menu(character);
    } else if(character.current_dungeon == -2){
        shop(character);
    } else if(character.current_dungeon == -3){
        mha_menu(character);
    } else {

    }
}

void select_char()
{
    string scelta="";
    do{
        cout << "Do you want to start from scratch? (YES or NO)" << endl;
        cin >> scelta;
        if(stringToLower(scelta)=="no")
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
                cout << "___________________\nSELECT YOUR CHARACTER!\n___________________" << endl;
                for (const auto& character : characters["characters"]) {
                    cout << "Name: " << character["name"] << endl;
                    cout << "Race: " << character["race"] << endl;
                    cout << "Sex: " << character["sex"] << endl;
                    cout << "Coins: " << character["coins"] << endl;
                    cout << "Level: " << character["level"] << endl;
                    cout << "Position: " << findPosition(character["current_dungeon"]) << endl;
                    cout << "___________________" << endl;
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
        } else if(stringToLower(scelta)=="yes") 
        {
            string n, r, s;

            cout << "Create your character:\nName: ";
            cin >> n;

            do{
                cout << "Race (select from Human, Elf, Dwarf, Orc, Halfling, Tiefling, Gnome, Goblin, Kobold, Hobbit): ";
                cin >> r;
            }while(find(first, last, r)==RACES.end());

            do{
                cout << "Sex (insert M or F): ";
                cin >> s;
            }while(s!="M" && s!="F");

            Character chosen_char(n, r, s);

            clearScreen();
            cout << "You created: " << chosen_char.name << " (Level " << chosen_char.level << ")\n";
            chosen_char.write_character_to_json(chosen_char);
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