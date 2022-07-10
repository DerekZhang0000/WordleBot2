#include "WordNetwork.h"

// Tasks:
// 1. Populate word list DONE
// 2. Create data structure to contain words DONE
// 3. Filter possible words with word state DONE
// 4. Compare weighted average list reduction DONE
// 4.1 Use the actual information equation
// 4.5 Multithreading, Skipping searches that take too long DONE,
//     Direct web interaction?, Monte Carlo Sims
// 5. Make a bangin README with pictures, equations, diagrams

// sprig is the best word for the answer list
// cedis is the best word for the full list
int main()
{
    WordNetwork wordNetwork = WordNetwork("AnswerWords.txt");
    wordNetwork.runGameTerminal();
    // wordNetwork.runSimTerminal(100);
    // wordNetwork.filterList("sprig", "00000", wordNetwork.wordList);
    // wordNetwork.filterList("leery", "01210", wordNetwork.wordList);
    // wordNetwork.printWordList();
    // std::cout << wordNetwork.bestNext(wordNetwork.wordList);
    // wordNetwork.filterList("savvy", "21000", wordNetwork.wordList);
    // wordNetwork.filterList("steam", "22220", wordNetwork.wordList);

    std::string str;
    std::cin >> str;
    return 0;
}

// std::string filter(std::string & )