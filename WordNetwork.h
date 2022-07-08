#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#ifndef WORDNETWORK_H
#define WORDETWORK_H

typedef std::vector<std::string> WordStates;
typedef std::unordered_set<std::string> WordList;
typedef std::unordered_map<char, std::unordered_set<std::string>> LetterTree;
typedef std::unordered_map<char, std::unordered_map<unsigned char, std::unordered_set<std::string>>> LetterIndexTree;

class WordNetwork
{
    public:

    WordNetwork(std::string const & filePath);
    ~WordNetwork();

    std::string wordState(std::string const & guessWord, std::string const & answerWord);
    std::string bestNext(WordList & wordList);

    void printWordList();
    void printTree(LetterIndexTree & letterIndexTree);

    // private:

    void populateWordStates();
    void populateWordList(std::string const & filePath);
    void populateTrees(WordList & wordList);

    void filterList(std::string const & word, std::string const & wordState, WordList & wordList);
    void filterLetterNode(char const & letter, WordList & wordList);
    void filterLetterIndexNode(char const & letter, unsigned char const & index, WordList & wordList);
    void filterInverseLetterNode(char const & letter, unsigned char const & index, WordList & wordList);

    unsigned short int wordsRemoved(std::string const & word, std::string const & wordState);
    void addRemovedLetterNode(char const & letter, WordList & removedWordList);
    void addRemovedLetterIndexNode(char const & letter, unsigned char const & index, WordList & removedWordList);
    void addRemovedInverseLetterNode(char const & letter, unsigned char const & index, WordList & removedWordList);

    unsigned int wordScore(std::string const & word, WordList & wordList);

    WordStates wordStates;
    WordList wordList;

    LetterTree letterTree;
    LetterTree inverseLetterTree;
    LetterIndexTree inverseLetterIndexTree;
};

#endif