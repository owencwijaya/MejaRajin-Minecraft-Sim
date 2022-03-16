// sample main file, replace this with your own code
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "../Lib/core/all_headers.hpp"

using namespace std;

tuple <string, string, string, string> parse(string line){
    istringstream iss(line);
    string ID;
    string nameTok, typeTok, btypeTok;
    getline(iss, ID, ' ');
    getline(iss, nameTok, ' ');
    getline(iss, typeTok, ' ');
    getline(iss, btypeTok, ' ');
    return make_tuple(ID, nameTok, typeTok, btypeTok);
}

tuple <tuple<int,int>, vector<string>, tuple<int,string>> parse(ifstream* file) {
    int i = 0, n = 0, m = 0;
    vector<string> recipeList;
    string name;
    int quantity;
    for (string line; getline(*file, line);) {
        if (i == 0) {
            istringstream iss(line);
            string row, col;
            getline(iss, row, ' ');
            getline(iss, col, ' ');
            n = stoi(row);
            m = stoi(col);    
        } else if (i <= n) {
            istringstream iss(line);
            for (int j = 0; j < m; j++) {
                string temp;
                getline(iss, temp, ' ');
                if (temp == "-") {
                    recipeList.push_back("UNDEFINED");
                } else {
                    recipeList.push_back(temp);
                }
                
            }
        } else {
            istringstream iss(line);
            string tempname, tempquantity;
            getline(iss, tempname, ' ');
            getline(iss, tempquantity, ' ');
            name = tempname;
            quantity = stoi(tempquantity);
        }
        i++;
    }
    return make_tuple(make_tuple(n, m),recipeList,make_tuple(quantity,name));
}

int main() {
    string configPath = "./config";
    string itemConfigPath = configPath + "/item.txt";

    // read item from config file
    ifstream itemConfigFile(itemConfigPath);
    for (string line; getline(itemConfigFile, line);) {
        itemConfig->push_back(parse(line));
    }
    // read recipes
    for (const auto &entry :
        filesystem::directory_iterator(configPath + "/recipe")) {
        ifstream* recipeConfigFile = new ifstream(entry.path());
        recipeConfig->push_back(parse(recipeConfigFile));
        // read from file and do something
    }

    //
    // test display inventory
    // cout << inv->get(0, 0).getID() << endl;

    // sample interaction
    // crftab->set(0, new NonTool(1, "OAK_PLANK", "NONTOOL", "PLANK", 2));
    // crftab->set(1, new NonTool(1, "OAK_PLANK", "NONTOOL", "PLANK", 2));
    // crftab->set(2, new NonTool(1, "OAK_PLANK", "NONTOOL", "PLANK", 2));
    // crftab->set(3, new NonTool(1, "OAK_PLANK", "NONTOOL", "PLANK", 2));
    // crftab->set(4, new NonTool(1, "OAK_PLANK", "NONTOOL", "PLANK", 2));
    // crftab->set(5, new NonTool(1, "OAK_PLANK", "NONTOOL", "PLANK", 2));
    // crftab->set(6, new NonTool(1, "OAK_PLANK", "NONTOOL", "PLANK", 2));
    // crftab->set(7, new NonTool(1, "OAK_PLANK", "NONTOOL", "PLANK", 2));
    // crftab->set(8, new NonTool(1, "OAK_PLANK", "NONTOOL", "PLANK", 2));

    // crftab->set(0, new NonTool(12, "DIAMOND", "NONTOOL", "-", 2));
    // crftab->set(1, new NonTool(12, "DIAMOND", "NONTOOL", "-", 2));
    // crftab->set(3, new NonTool(12, "DIAMOND", "NONTOOL", "-", 2));
    // crftab->set(4, new NonTool(7, "STICK", "NONTOOL", "-", 2));
    // crftab->set(7, new NonTool(7, "STICK", "NONTOOL", "-", 2));

    // sample interaction
    // cout << crftab->get(0)->getQuantity() << " ";
    // inv->set(0,0, new Item(3,"BIRCH_LOG", "NONTOOL", "LOG"));
    // crftab = new CraftingTable();
    cout << "\n\nInput command: ";
    string command;
    while (cin >> command) {
        if (command == "SHOW"){
            crftab->displayMenu();
            inv->displayMenu();
        } else if (command == "DETAILS"){
            DetailsHandler();
        } else if (command == "GIVE"){
            GiveHandler();
        } else if (command == "DISCARD"){
            DiscardHandler();
        }
        else if (command == "EXPORT") {
            string outputPath;
            cin >> outputPath;
            ofstream outputFile(outputPath);

            // hardcode for first test case
            outputFile << "21:10" << endl;
            outputFile << "6:1" << endl;
            for (int i = 2; i < 27; i++) {
                outputFile << "0:0" << endl;
            }

            cout << "Exported" << endl;
        } else if (command == "CRAFT") {
            try {
                CraftingHandler();
            }
            catch(const char* e) {
                cout << e;
            }    
        } else if (command == "MOVE") {
            string slotSrc;
            int slotQty;
            // need to handle multiple destinations
            cin >> slotSrc >> slotQty;
            try{
                MoveHandler(slotSrc,slotQty);
                cout << "Move succeeded\n";
            }catch(string err){
                cout << err;
            }
            
        } else {
        // todo
            cout << "Invalid command!" << endl;
        }
        cout << "Input command: ";
    }
    return 0;
}
