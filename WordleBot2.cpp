#include "WordNetwork.h"

// Tasks:
// 1. Populate word list DONE
// 2. Create data structure to contain words DONE
// 3. Filter possible words with word state
// 4. Compare weighted average list reduction
// 5. Make a bangin README with pictures

int main()
{
    WordNetwork wordNetwork = WordNetwork("testWords.txt");
    wordNetwork.printTree();
    // wordNetwork.filterList("space", "22222", wordNetwork.wordList);
    // for (auto const & word : wordNetwork.wordList)
    // {
    //     std::cout << word << "\n";
    // }
    // std::cout << wordNetwork.bestNext(wordNetwork.wordList);
    std::string str;
    std::cin >> str;
    return 0;
}

// std::string filter(std::string & )