#include "WordNetwork.h"

// Tasks:
// 1. Populate word list DONE
// 2. Create data structure to contain words DONE
// 3. Filter possible words with word state DONE
// 4. Compare weighted average list reduction DONE
// 4.5 Multithreading? Direct web interaction, Monte Carlo Sims
// 5. Make a bangin README with pictures

int main()
{
    WordNetwork wordNetwork = WordNetwork("AllowedrWords.txt");
    // wordNetwork.printWordList();
    // std::cout << wordNetwork.wordList.size() << "\n";
    std::cout << wordNetwork.bestNext(wordNetwork.wordList) << "\n";
    // wordNetwork.printWordList();
    // std::cout << wordNetwork.wordList.size() << "\n";
    std::string str;
    std::cin >> str;
    return 0;
}

// std::string filter(std::string & )