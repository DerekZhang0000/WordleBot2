#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <thread>
#include <mutex>

#ifndef WORDNETWORK_H
#define WORDETWORK_H

typedef std::vector<std::string> WordStateList;
typedef std::unordered_set<std::string> WordList;
typedef std::unordered_map<char, std::unordered_set<std::string>> LetterTree;
typedef std::unordered_map<char, std::unordered_map<unsigned char, std::unordered_set<std::string>>> LetterIndexTree;
typedef LetterIndexTree LetterRepetitionTree;
typedef std::unordered_map<unsigned char, unsigned short> StatMap;
typedef std::vector<std::pair<std::string, unsigned long long>> WordScoreList;

class WordNetwork
{
    public:

    WordNetwork(std::string const & filePath);
    ~WordNetwork();

    void runGameTerminal();
    void runSimTerminal(unsigned short trials);

    void printWordList();

    // private:

    void populateWordStates();
    void populateWordList(std::string const & filePath, WordList & wordList);
    void populateTrees(WordList & wordList);

    void filterList(std::string const & guessWord, std::string const & wordState, WordList & wordList);
    void filterLetterNode(char const & letter, WordList & wordList);
    void filterLetterIndexNode(char const & letter, unsigned char const & index, WordList & wordList);
    void filterInverseLetterNode(char const & letter, unsigned char const & index, WordList & wordList);
    void filterInverseLetterIndexNode(char const & letter, unsigned char const & index, WordList & wordList);
    void filterLetterRepetitionNode(char const & letter, unsigned char const & repetitions, WordList & wordList);

    unsigned short wordsRemoved(std::string const & word, std::string const & wordState);
    void addRemovedLetterNode(char const & letter, WordList & removedWordList);
    void addRemovedLetterIndexNode(char const & letter, unsigned char const & index, WordList & removedWordList);
    void addRemovedInverseLetterNode(char const & letter, unsigned char const & index, WordList & removedWordList);
    void addRemovedInverseLetterIndexNode(char const & letter, unsigned char const & index, WordList & removedWordList);
    void addRemovedLetterRepetitionNode(char const & letter, unsigned char const & repetitions, WordList & wordList);

    std::string bestNext(WordList & wordList);
    unsigned long long wordScore(std::string const & guessWord, WordList & wordList, unsigned long long & minWordScore);
    std::string wordState(std::string const & guessWord, std::string const & answerWord);

    std::string bestNextMultiThreaded(WordList & wordList, unsigned short & threads);
    void bestNextThread(WordList & wordList, WordScoreList & bestList);

    void updateStats(unsigned short steps);

    WordStateList wordStates;
    WordList wordList;
    WordList parentWordList;

    LetterTree letterTree; // Tree containing letter nodes (groups of words that contain a common letter)
    LetterTree inverseLetterTree; // Tree containing inverse letter nodes (group of words that EXCLUDE a common letter)
    LetterIndexTree letterIndexTree; // Tree containing letter nodes that contain index nodes (groups of words that contain the common letter at a common index)
    LetterIndexTree inverseLetterIndexTree; // Tree containing inverse letter nodes letter nodes that contain inverse index nodes (groups of words that EXCLUDE the common letter at a common index)
    LetterRepetitionTree letterRepetitionTree; // Tree containing letter nodes that contain repetition nodes (groups of words that have the same repetitions of a common letter)

    StatMap statMap;

    std::mutex mu;
};

#endif