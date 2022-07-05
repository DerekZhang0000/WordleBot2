#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

#ifndef WORDNETWORK_H
#define WORDETWORK_H

typedef std::unordered_set<std::string> WordList;
typedef std::unordered_map<char, std::unordered_map<unsigned char, std::unordered_set<std::string>>> LetterIndexTree;

class WordNetwork
{
    public:

    WordNetwork(std::string const & filePath);
    ~WordNetwork();

    std::string wordState(std::string const & guessWord, std::string const & answerWord);
    std::string bestNext(WordList & wordList);

    void printWordList();
    void printTree();

    // private:

    void populateList(std::string const & filePath);
    void populateTree(WordList & wordList);

    void filterList(std::string const & word, std::string const & wordState, WordList & wordList);
    void removeLetterNode(char const & letter, WordList & wordList);
    void removeLetterIndexNode(char const & letter, unsigned char const & index, WordList & wordList);
    void inverseRemoveLetterIndexNode(char const & letter, unsigned char const & index, WordList & wordList);

    unsigned short int filterInt(std::string const & word, std::string const & wordState);
    void addRemovedLetterNode(char const & letter, WordList & removedWordList);
    void addRemovedLetterIndexNode(char const & letter, unsigned char const & index, WordList & removedWordList);
    void addInverseRemovedLetterIndexNode(char const & letter, unsigned char const & index, WordList & removedWordList);

    double wordScore(std::string const & word, WordList & wordList);

    WordList wordList;
    LetterIndexTree letterIndexTree;
};

#endif