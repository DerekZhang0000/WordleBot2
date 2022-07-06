#include "WordNetwork.h"

std::unordered_map<std::string, unsigned short int> wordStateList;

WordNetwork::WordNetwork(std::string const & filePath)
{
    populateList(filePath);
    populateTree(wordList);
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
        if (presentLettersIt == presentLetters.end() ||
            presentLettersIt->second == 0)
        {
            charArray[i] = '0';
        }
        else if (letter == answerWord.at(i)) {
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
    double maxWordScore = 0;
    std::string maxWord = wordList.begin()->c_str();
    for (auto const & word : wordList) {
        std::cout << "Calculating score for " << word << "...\n";
        double score = wordScore(word, wordList);
        if (score > maxWordScore) {
            maxWordScore = score;
            maxWord = word;
        }
    }
    return maxWord;
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

void WordNetwork::printTree()
{
    std::cout << "Printing tree:\n";
    for (auto const & letterNode : letterIndexTree)
    {
        std::cout << "Words that contain letter " << letterNode.first << ":\n";
        for (auto const letterIndexNode : letterNode.second)
        {
            std::cout << "    Index " << (int)letterIndexNode.first << ":\n";
            for (auto const word : letterIndexNode.second)
            {
                std::cout << "        " << word << "\n";
            }
        }
    }
    std::cout << "End of tree\n";
}

void WordNetwork::populateList(std::string const & filePath)
{
    std::ifstream file(filePath);
    std::string word;

    while (std::getline(file, word)) {
        wordList.insert(word);
    }
}

void WordNetwork::populateTree(WordList & wordList)
{
    for (auto & word : wordList) {

        for (unsigned char j = 0; j < 5; ++j) {
            letterIndexTree[word.at(j)][j].insert(word);
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
    for (unsigned char i = 0; i < 5; ++i)
    {
        char const letter = word.at(i);
        char const letterState = wordState.at(i);
        auto presentLettersIt = presentLetters.find(letter);

        if (letterState == '0' &&
            presentLettersIt->second == 0)
        {
            removeLetterNode(letter, wordList);
        }
        else if (letterState == '1' &&
                 presentLettersIt->second > 0)
        {
            removeLetterIndexNode(letter, i, wordList);
            presentLettersIt->second -= 1;
        }
        else if (letterState == '2' &&
                 presentLettersIt->second > 0)
        {
            std::cout << "Inverse removing letter node " << letter << " " << (int)i << "\n";
            for (auto word : wordList) {
                try {
                    if (word.size() != 0 && word.at(i) != letter) {
                        std::cout << "Erasing " << word << "\n";
                        wordList.erase(word.c_str());
                    }
                }
                catch (...) {}
            }
            presentLettersIt->second -= 1;
        }
    }
}

// Removes all the words in the word list that contain the specified letter
void WordNetwork::removeLetterNode(char const & letter, WordList & wordList)
{
    std::cout << "Removing letter node " << letter << "\n";
    auto const & letterNodeIt = letterIndexTree.find(letter);
    if (letterNodeIt == letterIndexTree.end()) { return; }

    for (unsigned char i = 0; i < 5; ++i) {
        auto const & letterIndexNodeIt = letterNodeIt->second.find(i);

        if (letterIndexNodeIt == letterNodeIt->second.end()) { continue; }
        auto const & letterIndexList = letterIndexNodeIt->second;

        for (auto const & word : letterIndexList) {
            wordList.erase(word);
            std::cout << "Removing " << word << "\n";
        }
    }
}

// Removes all the words in the word list that have the specified letter at the specified index
void WordNetwork::removeLetterIndexNode(char const & letter, unsigned char const & index, WordList & wordList)
{
    std::cout << "Removing letter index node " << letter << " " << (int)index << "\n";
    auto const & letterNodeIt = letterIndexTree.find(letter);
    if (letterNodeIt == letterIndexTree.end()) { return; }

    auto const & letterIndexNodeIt = letterNodeIt->second.find(index);

    if (letterIndexNodeIt == letterNodeIt->second.end()) { return; }
    auto const & letterIndexList = letterIndexNodeIt->second;

    for (auto const & word : letterIndexList) {
        wordList.erase(word);
        std::cout << "Removing " << word << "\n";
    }
}

// Removes all the words in the word list that do not have the specified letter at the specified index
void WordNetwork::inverseRemoveLetterIndexNode(char const & letter, unsigned char const & index, WordList & wordList)
{
    std::cout << "Inverse removing letter index node " << letter << " " << (int)index << "\n";
    auto const & letterNodeIt = letterIndexTree.find(letter);
    if (letterNodeIt == letterIndexTree.end()) { return; }

    for (unsigned char i = 0; i < 5; ++i) {
        if (i == index) { continue; }
        auto const & letterIndexNodeIt = letterNodeIt->second.find(i);

        if (letterIndexNodeIt == letterNodeIt->second.end()) { continue; }
        auto const & letterIndexList = letterIndexNodeIt->second;

        for (auto const & word : letterIndexList) {
            wordList.erase(word);
            std::cout << "Removing " << word << "\n";
        }
    }
}

// Returns int representing how many unique words are removed from the list of possible words
unsigned short int WordNetwork::filterInt(std::string const & word, std::string const & wordState)
{
    std::unordered_map<char, unsigned char> presentLetters;
    for (unsigned char i = 0; i < 5; ++i) {
        char const letter = word.at(i);
        char const letterState = wordState.at(1);

        if (letterState != '0') {
            presentLetters[letter] += 1;
        }
    }

    WordList removedWordList;
    for (unsigned char i = 0; i < 5; ++i)
    {
        char const letter = word.at(i);
        char const letterState = wordState.at(1);
        auto presentLettersIt = presentLetters.find(letter);

        if (letterState == '0' &&
            (presentLettersIt == presentLetters.end() ||
             presentLettersIt->second == 0))
        {
            addRemovedLetterNode(letter, removedWordList);
        }
        else if (letterState == '1')
        {
            addRemovedLetterIndexNode(letter, i, removedWordList);
            presentLettersIt->second -= 1;
        }
        else if (letterState == '2')
        {
            addInverseRemovedLetterIndexNode(letter, i, removedWordList);
            presentLettersIt->second -= 1;
        }
    }

    return removedWordList.size();
}

// Adds removed words from letter node to removed word list
void WordNetwork::addRemovedLetterNode(char const & letter, WordList & removedWordList)
{
    auto const & letterNodeIt = letterIndexTree.find(letter);

    if (letterNodeIt == letterIndexTree.end()) { return; }

    for (unsigned char i = 0; i < 5; ++i) {
        auto const & letterIndexNodeIt = letterNodeIt->second.find(i);

        if (letterIndexNodeIt == letterNodeIt->second.end()) { continue; }
        auto const & letterIndexList = letterIndexNodeIt->second;

        for (auto & word : letterIndexList) {
            removedWordList.insert(word);
        }
    }
}

// Adds removed words from letter index node to removed word list
void WordNetwork::addRemovedLetterIndexNode(char const & letter, unsigned char const & index, WordList & removedWordList)
{
    auto const & letterNodeIt = letterIndexTree.find(letter);
    if (letterNodeIt == letterIndexTree.end()) { return; }

    auto const & letterIndexNodeIt = letterNodeIt->second.find(index);

    if (letterIndexNodeIt == letterNodeIt->second.end()) { return; }
    auto const & letterIndexList = letterIndexNodeIt->second;

    for (auto & word : letterIndexList) {
        removedWordList.insert(word);
    }
}

// Adds removed words from other letter index nodes to removed word list
void WordNetwork::addInverseRemovedLetterIndexNode(char const & letter, unsigned char const & index, WordList & removedWordList)
{
    auto const & letterNodeIt = letterIndexTree.find(letter);

    if (letterNodeIt == letterIndexTree.end()) { return; }

    for (unsigned char i = 0; i < 5; ++i) {
        if (i == index) { continue; }
        auto const & letterIndexNodeIt = letterNodeIt->second.find(i);

        if (letterIndexNodeIt == letterNodeIt->second.end()) { continue; }
        auto const & letterIndexList = letterIndexNodeIt->second;

        for (auto & word : letterIndexList) {
            removedWordList.insert(word);
        }
    }
}

// Returns the word score of a word given the possible word list
double WordNetwork::wordScore(std::string const & guessWord, WordList & wordList)
{
    unsigned int removedSum = 0;
    for (std::string const & answerWord : wordList) {
        removedSum += filterInt(guessWord, wordState(guessWord, answerWord));
    }
    return 1 - (removedSum / wordList.size());
}