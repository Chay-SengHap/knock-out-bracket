#include <iostream>
#include <vector>
#include "solution.hpp"


int main() {
    vector<string> players = {"Anna", "Ben", "Chou", "Dara", "Ean", "Faye", "Gita", "Hout"};
    TournamentTree bracket;
    bracket.built(players);
    
    cout << "Teams before Round 1: ";
    for (const string& p : players) {
        cout << p << " ";
    }
    cout << endl << endl;
    
    cout << "Initial Bracket:" << endl;
    bracket.printBracket();
    
    // 1. Simulate Round 1: Call recordResult for each first-round match (IDs 1-4)
    cout << "\n--- Simulating Round 1 (random scores) ---" << endl;
    for (int i = 1; i <= 4; ++i) {
        bracket.recordResult(i);
    }
    cout << "\nAfter Round 1:" << endl;
    bracket.printBracket();
    
    // 2. Round 2 (semis, IDs 5-6)
    cout << "\n--- Simulating Round 2 ---" << endl;
    for (int i = 5; i <= 6; ++i) {
        bracket.recordResult(i);
    }
    cout << "\nAfter Round 2:" << endl;
    bracket.printBracket();
    
    // 3. Final (ID 7)
    cout << "\n--- Simulating Final ---" << endl;
    bracket.recordResult(7);
    cout << "\nAfter Final (Champion decided):" << endl;
    bracket.printBracket();
    
    // 4. Two wouldMeet queries
    cout << "\n--- Would-Meet Queries ---" << endl;
    auto meet1 = bracket.wouldMeet("Chou", "Hout");
    cout << "Chou vs Hout: Match " << meet1.first << ", Round " << meet1.second << endl;
    auto meet2 = bracket.wouldMeet("Anna", "Dara");
    cout << "Anna vs Dara: Match " << meet2.first << ", Round " << meet2.second << endl;
    
    // 5. Two pathToFinal queries (theoretical full path)
    cout << "\n--- Path-to-Final Queries (Theoretical) ---" << endl;
    vector<int> pathAnna = bracket.pathToFinal("Anna");
    cout << "Anna's path: ";
    for (int id : pathAnna) cout << id << " ";
    cout << endl;
    vector<int> pathFaye = bracket.pathToFinal("Faye");
    cout << "Faye's path: ";
    for (int id : pathFaye) cout << id << " ";
    cout << endl;
    
    return 0;
}