#include "WordNetwork.h"

std::unordered_map<std::string, unsigned short int> wordStateList;

WordNetwork::WordNetwork(std::string const & filePath)
{
    populateWordStates();
    populateWordList(filePath);
    populateTrees(wordList);
}

WordNetwork::~WordNetwork()
{

}

// Returns a word state based on guess word and assumed answer word
std::string WordNetwork::wordState(std::string const & guessWord, std::string const & answerWord)
{
    std::unordered_map<char, unsigned char> presentLetters;
    for (auto const & letter : answerWord) {
        presentLetters[letter] += 1;
    }

    char charArray[6] = {'0', '0', '0', '0', '0', '\0'};
    for (unsigned char i = 0; i < 5; ++i) {
        auto const letter = guessWord.at(i);
        auto presentLettersIt = presentLetters.find(letter);
        if (presentLettersIt != presentLetters.end() &&
            presentLettersIt->second > 0 &&
            letter == answerWord.at(i))
        {
            charArray[i] = '2';
            presentLettersIt->second -= 1;
        }
    }
    for (unsigned char i = 0; i < 5; ++i) {
        auto const letter = guessWord.at(i);
        auto presentLettersIt = presentLetters.find(letter);
        if (presentLettersIt != presentLetters.end() &&
            presentLettersIt->second > 0)
        {
            charArray[i] = '1';
            presentLettersIt->second -= 1;
        }
    }

    return charArray;
}

std::string WordNetwork::bestNext(WordList & wordList)
{
    unsigned int minWordScore = 4294967295; // Max unsigned int
    std::string minWord = wordList.begin()->c_str();
    std::ofstream file("WordScores.txt");
    for (auto const & word : wordList) {
        std::cout << "Calculating score for " << word << "...\n";
        unsigned int score = wordScore(word, wordList);
        if (score < minWordScore) {
            minWordScore = score;
            minWord = word;
            std::cout << "New best: " << word << ", " << "Residue Proportion: " << score << "\n";
        }
        // file << word << score << "\n";
    }
    return minWord;
}

void WordNetwork::printWordList()
{
    std::cout << "Printing word list:\n";
    for (auto const & word : wordList)
    {
        std::cout << word << "\n";
    }
    std::cout << "End of word list\n";
}

void WordNetwork::populateWordStates()
{
    for (unsigned char i = 0; i < 3; ++i) {
        for (unsigned char j = 0; j < 3; ++j) {
            for (unsigned char k = 0; k < 3; ++k) {
                for (unsigned char l = 0; l < 3; ++l) {
                    for (unsigned char m = 0; m < 3; ++m) {
                        char state[6] = {(char)(i + '0'), (char)(j + '0'), (char)(k + '0'), (char)(l + '0'), (char)(m + '0'), '\0'};
                        wordStates.push_back(state);
                    }
                }
            }
        }
    }
}

void WordNetwork::populateWordList(std::string const & filePath)
{
    std::ifstream file(filePath);
    std::string word;

    while (std::getline(file, word)) {
        wordList.insert(word);
    }
}

void WordNetwork::populateTrees(WordList & wordList)
{
    for (auto const & word : wordList) {
        std::unordered_set<char> presentLetters;

        for (unsigned char i = 0; i < 5; ++i) {
            char const letter = word.at(i);
            inverseLetterIndexTree[letter][i].insert(word);
            presentLetters.insert(letter);
        }

        for (auto const & letter : "abcdefghijklmnopqrstuvwxyz") {
            if (presentLetters.find(letter) == presentLetters.end()) {
                inverseLetterTree[letter].insert(word);
            }
            else {
                letterTree[letter].insert(word);
            }
        }
    }
}

// Filters word list based on input word and resulting word state
void WordNetwork::filterList(std::string const & word, std::string const & wordState, WordList & wordList)
{
    std::unordered_map<char, unsigned char> presentLetters;
    for (unsigned char i = 0; i < 5; ++i) {
        char const letter = word.at(i);
        char const letterState = wordState.at(i);

        if (letterState != '0') {
            presentLetters[letter] += 1;
        }
        else {
            presentLetters[letter] = 0;
        }
    }

    // Needs to filter on all correct words first
    for (unsigned char i = 0; i < 5; ++i)
    {
        char const letter = word.at(i);
        char const letterState = wordState.at(i);
        auto presentLettersIt = presentLetters.find(letter);

        if (letterState == '2' &&
            presentLettersIt->second > 0)
        {
            filterInverseLetterNode(letter, i, wordList);
            presentLettersIt->second -= 1;
        }
    }

    for (unsigned char i = 0; i < 5; ++i)
    {
        char const letter = word.at(i);
        char const letterState = wordState.at(i);
        auto presentLettersIt = presentLetters.find(letter);

        if (letterState == '0' &&
            presentLettersIt->second == 0)
        {
            filterLetterNode(letter, wordList);
        }
        else if (letterState == '1' &&
                 presentLettersIt->second > 0)
        {
            filterLetterIndexNode(letter, i, wordList);
            filterInverseLetterNode(letter, i, wordList);
            presentLettersIt->second -= 1;
        }
    }
}

// Removes all the words in the word list that have the specified letter in the word
void WordNetwork::filterLetterNode(char const & letter, WordList & wordList)
{
    // std::cout << "Removing letter node " << letter << "\n";
    auto const & letterNodeIt = letterTree.find(letter);
    if (letterNodeIt == letterTree.end()) { return; }

    for (auto const & word : letterNodeIt->second) {
        wordList.erase(word);
        // std::cout << "Removing " << word << "\n";
    }
}

// Removes all the words in the word list that have the specified letter at the specified index
void WordNetwork::filterLetterIndexNode(char const & letter, unsigned char const & index, WordList & wordList)
{
    // std::cout << "Removing letter index node " << letter << " " << (int)index << "\n";
    auto const & letterNodeIt = inverseLetterIndexTree.find(letter);
    if (letterNodeIt == inverseLetterIndexTree.end()) { return; }

    auto const & letterIndexNodeIt = letterNodeIt->second.find(index);
    if (letterIndexNodeIt == letterNodeIt->second.end()) { return; }

    auto const & letterIndexList = letterIndexNodeIt->second;

    for (auto const & word : letterIndexList) {
        wordList.erase(word);
        // std::cout << "Removing " << word << "\n";
    }
}

// Removes all the words in the word list that DO NOT have the specified letter in the word
void WordNetwork::filterInverseLetterNode(char const & letter, unsigned char const & index, WordList & wordList)
{
    // std::cout << "Inverse removing letter index node " << letter << " " << (int)index << "\n";
    auto const & inverseLetterNodeIt = inverseLetterTree.find(letter);
    if (inverseLetterNodeIt == inverseLetterTree.end()) { return; }

    for (auto const & word : inverseLetterNodeIt->second) {
        wordList.erase(word);
        // std::cout << "Removing " << word << "\n";
    }
}

// Returns int representing how many unique words are removed from the list of possible words
unsigned short int WordNetwork::wordsRemoved(std::string const & guessWord, std::string const & wordState)
{
    std::unordered_map<char, unsigned char> presentLetters;
    for (unsigned char i = 0; i < 5; ++i) {
        char const letter = guessWord.at(i);
        char const letterState = wordState.at(1);

        if (letterState != '0') {
            presentLetters[letter] += 1;
        }
        else {
            presentLetters[letter] = 0;
        }
    }

    WordList removedWordList;
    for (unsigned char i = 0; i < 5; ++i)
    {
        char const letter = guessWord.at(i);
        char const letterState = wordState.at(1);
        auto presentLettersIt = presentLetters.find(letter);

        if (letterState == '0' &&
            presentLettersIt->second == 0)
        {
            addRemovedLetterNode(letter, removedWordList);
        }
        else if (letterState == '1' &&
                 presentLettersIt->second > 0)
        {
            addRemovedLetterIndexNode(letter, i, removedWordList);
            presentLettersIt->second -= 1;
        }
        else if (letterState == '2' &&
                 presentLettersIt->second > 0)
        {
            addRemovedInverseLetterNode(letter, i, removedWordList);
            presentLettersIt->second -= 1;
        }
    }

    return removedWordList.size();
}

// Adds removed words from letter node to removed word list
void WordNetwork::addRemovedLetterNode(char const & letter, WordList & removedWordList)
{
    auto const & letterNodeIt = letterTree.find(letter);
    if (letterNodeIt == letterTree.end()) { return; }

    for (auto const & word : letterNodeIt->second) {
        removedWordList.insert(word);
    }
}

// Adds removed words from letter index node to removed word list
void WordNetwork::addRemovedLetterIndexNode(char const & letter, unsigned char const & index, WordList & removedWordList)
{
    auto const & letterNodeIt = inverseLetterIndexTree.find(letter);
    if (letterNodeIt == inverseLetterIndexTree.end()) { return; }

    auto const & letterIndexNodeIt = letterNodeIt->second.find(index);
    if (letterIndexNodeIt == letterNodeIt->second.end()) { return; }

    auto const & letterIndexList = letterIndexNodeIt->second;

    for (auto const & word : letterIndexList) {
        removedWordList.insert(word);
    }
}

// Adds removed words from inverse letter node to removed word list
void WordNetwork::addRemovedInverseLetterNode(char const & letter, unsigned char const & index, WordList & removedWordList)
{
    auto const & inverseLetterNodeIt = inverseLetterTree.find(letter);
    if (inverseLetterNodeIt == inverseLetterTree.end()) { return; }

    for (auto const & word : inverseLetterNodeIt->second) {
        wordList.insert(word);
    }
}

// Returns a score proportional to the expected list residue of a guess word (lower is better)
unsigned int WordNetwork::wordScore(std::string const & guessWord, WordList & wordList)
{
    unsigned int removedSum = 0;
    for (auto const & state : wordStates) {
        unsigned int removed = wordsRemoved(guessWord, state);
        removedSum += removed * removed;
    }
    return removedSum * removedSum;
}