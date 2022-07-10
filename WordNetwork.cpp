#include "WordNetwork.h"

WordNetwork::WordNetwork(std::string const & filePath)
{
    populateWordStates();
    populateWordList(filePath, wordList);
    populateWordList(filePath, parentWordList);
    populateTrees(wordList);
}

WordNetwork::~WordNetwork()
{

}

void WordNetwork::runGameTerminal()
{
    unsigned short steps = 0;
    std::string wordState;

    std::cout << "sprig\n";
    std::cin >> wordState;
    filterList("sprig", wordState, wordList);
    populateTrees(wordList);
    ++steps;

    bool completed = false;
    while (steps <= 6) {
        std::string bestWord = bestNext(wordList);
        std::cout << bestWord << "\n";
        std::cin >> wordState;
        ++steps;

        if (wordState == "22222") {
            completed = true;
            break;
        }

        filterList(bestWord, wordState, wordList);
        populateTrees(wordList);

        if (wordList.size() == 1) {
            std::cout << wordList.begin()->c_str() << "\n";
            completed = true;
            ++steps;
            break;
        }
    }

    if (!completed) {
        std::cout << "Incomplete\n";
    }

    std::cout << "Steps: " << steps << "\n";
}

void WordNetwork::runSimTerminal(unsigned short trials)
{
    for (unsigned short i = 0; i < trials; ++i)
    {
        populateWordList("AnswerWords.txt", wordList);
        populateTrees(wordList);
        auto wordListIt = wordList.begin();
        std::advance(wordListIt, rand() % wordList.size());

        unsigned short steps = 0;
        std::string answerWord = wordListIt->c_str();
        std::cout << answerWord << "\n";
        ++steps;

        std::string state = wordState("sprig", answerWord);
        filterList("sprig", state, wordList);
        populateTrees(wordList);


        while (steps <= 6) {
            std::string const bestWord = bestNext(wordList);
            std::string const state = wordState(bestWord, answerWord);
            filterList(bestWord, state, wordList);
            populateTrees(wordList);
            ++steps;

            if (state == "22222") { break; }

            if (wordList.size() == 1) {
                ++steps;
                break;
            }
        }

        updateStats(steps);
    }

}

void WordNetwork::updateStats(unsigned short steps)
{
    if (steps > 6) {
        statMap[7] += 1;
    }
    else {
        statMap[steps] += 1;
    }

    unsigned short step1 = statMap[1];
    unsigned short step2 = statMap[2];
    unsigned short step3 = statMap[3];
    unsigned short step4 = statMap[4];
    unsigned short step5 = statMap[5];
    unsigned short step6 = statMap[6];
    unsigned short incomplete = statMap[7];

    unsigned int sum = step1 + step2 + step3 + step4 + step5 + step6;
    double average = (double)(step1 + step2 * 2 + step3 * 3 + step4 * 4 + step5 * 5 + step6 * 6) / (sum != 0 ? sum : 1);
    double accuracy = (double)sum / ((sum != 0 ? sum : 1) + incomplete);

    std::cout << "Avg: " << average
    << ", Accuracy: " << accuracy
    << ", 1 Step: " << step1
    << ", 2 Steps: " << step2
    << ", 3 Steps: " << step3
    << ", 4 Steps: " << step4
    << ", 5 Steps: " << step5
    << ", 6 Steps: " << step6
    << ", Incomplete: " << incomplete
    << "\n";
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

void WordNetwork::populateWordList(std::string const & filePath, WordList & wordList)
{
    std::ifstream file(filePath);
    std::string word;

    while (std::getline(file, word)) {
        wordList.insert(word);
    }
}

void WordNetwork::populateTrees(WordList & wordList)
{
    letterTree.clear();
    letterIndexTree.clear();
    inverseLetterTree.clear();

    for (auto const & word : wordList) {
        std::unordered_map<char, unsigned char> letterCounts;

        for (unsigned char i = 0; i < 5; ++i) {
            char const letter = word.at(i);
            letterIndexTree[letter][i].insert(word);
            letterCounts[letter] += 1;
        }

        for (auto const & letter : "abcdefghijklmnopqrstuvwxyz") {
            if (letterCounts.find(letter) == letterCounts.end()) {
                inverseLetterTree[letter].insert(word);

                for (unsigned char j = 0; j < 5; ++j) {
                    inverseLetterIndexTree[letter][j].insert(word);
                }
            }
            else {
                letterTree[letter].insert(word);

                for (unsigned char j = 0; j < 5; ++j) {
                    if (word.at(j) == letter) { continue; }
                    inverseLetterIndexTree[letter][j].insert(word);
                }
            }
        }

        for (auto [letter, repetitions] : letterCounts) {
            letterRepetitionTree[letter][repetitions].insert(word);
        }
    }
}

// Filters word list based on input word and resulting word state
void WordNetwork::filterList(std::string const & word, std::string const & wordState, WordList & wordList)
{
    // Adds letter counts to map, bool represents if we know the count is exact
    std::unordered_map<char, std::pair<unsigned char, bool>> letterCounts;
    for (unsigned char i = 0; i < 5; ++i) {
        char const letter = word.at(i);

        auto letterCountsIt = letterCounts.find(letter);
        if (letterCountsIt == letterCounts.end()) {
            letterCounts.emplace(letter, std::make_pair<unsigned char, bool>(wordState.at(i) != '0' ? 1 : 0, false));
        }
        else {
            if (wordState.at(i) != '0') {
                letterCountsIt->second.first += 1;
            }
            else {  // We know the count is exact if we see a 0 (gray) letter state
                letterCountsIt->second.second = true;
            }
        }
    }

    for (auto const & letter : word) {
        auto const letterCountsPair = letterCounts.find(letter)->second;
        auto const letterCount = letterCountsPair.first;
        auto const exactCount = letterCountsPair.second;

        if (exactCount) {
            for (unsigned char i = 1; i <= 5; ++i) {
                if (i == letterCount) { continue; }
                filterLetterRepetitionNode(letter, i, wordList);
            }
        }
        else {
            for (unsigned char i = 1; i < letterCount; ++i) {
                filterLetterRepetitionNode(letter, i, wordList);
            }
        }
    }

    for (unsigned char i = 0; i < 5; ++i)
    {
        char const letter = word.at(i);
        char const letterState = wordState.at(i);
        auto & letterCount = letterCounts.find(letter)->second.first;

        if (letterState == '2')
        {
            filterInverseLetterIndexNode(letter, i, wordList);
            filterInverseLetterNode(letter, i, wordList);
            letterCount -= 1;
        }
    }

    for (unsigned char i = 0; i < 5; ++i)
    {
        char const letter = word.at(i);
        char const letterState = wordState.at(i);
        auto & letterCount = letterCounts.find(letter)->second.first;

        if (letterState == '0' &&
            letterCount == 0)
        {
            filterLetterNode(letter, wordList);
        }
        else if (letterState == '1')
        {
            filterLetterIndexNode(letter, i, wordList);
            filterInverseLetterNode(letter, i, wordList);
            letterCount -= 1;
        }
    }
}

// Removes all the words in the word list that have the specified letter in the word
void WordNetwork::filterLetterNode(char const & letter, WordList & wordList)
{
    // std::cout << "Removing letter node " << letter << "\n";
    auto const letterNodeIt = letterTree.find(letter);
    if (letterNodeIt == letterTree.end()) { return; }

    for (auto const & word : letterNodeIt->second) {
        wordList.erase(word);
        // std::cout << "Removing " << word << "\n";
    }
}

// Removes all the words in the word list that DO NOT have the specified letter in the word
void WordNetwork::filterInverseLetterNode(char const & letter, unsigned char const & index, WordList & wordList)
{
    // std::cout << "Inverse removing letter index node " << letter << " " << (int)index << "\n";
    auto const inverseLetterNodeIt = inverseLetterTree.find(letter);
    if (inverseLetterNodeIt == inverseLetterTree.end()) { return; }

    for (auto const & word : inverseLetterNodeIt->second) {
        wordList.erase(word);
        // std::cout << "Removing " << word << "\n";
    }
}

// Removes all the words in the word list that have the specified letter at the specified index
void WordNetwork::filterLetterIndexNode(char const & letter, unsigned char const & index, WordList & wordList)
{
    // std::cout << "Removing letter index node " << letter << " " << (int)index << "\n";
    auto const letterNodeIt = letterIndexTree.find(letter);
    if (letterNodeIt == letterIndexTree.end()) { return; }

    auto const letterIndexNodeIt = letterNodeIt->second.find(index);
    if (letterIndexNodeIt == letterNodeIt->second.end()) { return; }

    auto const letterIndexList = letterIndexNodeIt->second;

    for (auto const & word : letterIndexList) {
        wordList.erase(word);
        // std::cout << "Removing " << word << "\n";
    }
}

// Removes all the words in the word list that DO NOT have the specified letter at the specified index
void WordNetwork::filterInverseLetterIndexNode(char const & letter, unsigned char const & index, WordList & wordList)
{
    // std::cout << "Removing letter index node " << letter << " " << (int)index << "\n";
    auto const letterNodeIt = inverseLetterIndexTree.find(letter);
    if (letterNodeIt == letterIndexTree.end()) { return; }

    auto const letterIndexNodeIt = letterNodeIt->second.find(index);
    if (letterIndexNodeIt == letterNodeIt->second.end()) { return; }

    auto const letterIndexList = letterIndexNodeIt->second;

    for (auto const & word : letterIndexList) {
        wordList.erase(word);
        // std::cout << "Removing " << word << "\n";
    }
}

// Removes all the words in the word list that have the specified character a specified number of times
void WordNetwork::filterLetterRepetitionNode(char const & letter, unsigned char const & repetitions, WordList & wordList)
{
    // std::cout << "Removing letter repetition node " << letter << " " << (int)repetitions << "\n";
    auto const letterNodeIt = letterRepetitionTree.find(letter);
    if (letterNodeIt == letterIndexTree.end()) { return; }

    auto const letterRepetitionNodeIt = letterNodeIt->second.find(repetitions);
    if (letterRepetitionNodeIt == letterNodeIt->second.end()) { return; }

    auto const letterRepetitionList = letterRepetitionNodeIt->second;

    for (auto const & word : letterRepetitionList) {
        wordList.erase(word);
        // std::cout << "Removing " << word << "\n";
    }
}

// Returns int representing how many unique words are removed from the list of possible words
unsigned short WordNetwork::wordsRemoved(std::string const & word, std::string const & wordState)
{
    WordList removedWordList;

    std::unordered_map<char, std::pair<unsigned char, bool>> letterCounts;
    for (unsigned char i = 0; i < 5; ++i) {
        char const letter = word.at(i);

        auto letterCountsIt = letterCounts.find(letter);
        if (letterCountsIt == letterCounts.end()) {
            letterCounts.emplace(letter, std::make_pair<unsigned char, bool>(wordState.at(i) != '0' ? 1 : 0, false));
        }
        else {
            if (wordState.at(i) != '0') {
                letterCountsIt->second.first += 1;
            }
            else {
                letterCountsIt->second.second = true;
            }
        }
    }

    for (auto const & letter : word) {
        auto const letterCountsPair = letterCounts.find(letter)->second;
        auto const letterCount = letterCountsPair.first;
        auto const exactCount = letterCountsPair.second;

        if (exactCount) {
            for (unsigned char i = 1; i <= 5; ++i) {
                if (i == letterCount) { continue; }
                addRemovedLetterRepetitionNode(letter, i, removedWordList);
            }
        }
        else {
            for (unsigned char i = 1; i < letterCount; ++i) {
                addRemovedLetterRepetitionNode(letter, i, removedWordList);
            }
        }
    }

    for (unsigned char i = 0; i < 5; ++i)
    {
        char const letter = word.at(i);
        char const letterState = wordState.at(i);
        auto & letterCount = letterCounts.find(letter)->second.first;

        if (letterState == '2')
        {
            addRemovedInverseLetterIndexNode(letter, i, removedWordList);
            addRemovedInverseLetterNode(letter, i, removedWordList);
            letterCount -= 1;
        }
    }

    for (unsigned char i = 0; i < 5; ++i)
    {
        char const letter = word.at(i);
        char const letterState = wordState.at(i);
        auto & letterCount = letterCounts.find(letter)->second.first;

        if (letterState == '0' &&
            letterCount == 0)
        {
            addRemovedLetterNode(letter, removedWordList);
        }
        else if (letterState == '1')
        {
            addRemovedLetterIndexNode(letter, i, removedWordList);
            addRemovedInverseLetterNode(letter, i, removedWordList);
            letterCount -= 1;
        }
    }

    return removedWordList.size();
}

// Adds the removed words from the letter node to the removed word list
void WordNetwork::addRemovedLetterNode(char const & letter, WordList & removedWordList)
{
    auto const letterNodeIt = letterTree.find(letter);
    if (letterNodeIt == letterTree.end()) { return; }

    for (auto const & word : letterNodeIt->second) {
        removedWordList.insert(word);
    }
}

// Adds the removed words from the inverse letter node to the removed word list
void WordNetwork::addRemovedInverseLetterNode(char const & letter, unsigned char const & index, WordList & removedWordList)
{
    auto const inverseLetterNodeIt = inverseLetterTree.find(letter);
    if (inverseLetterNodeIt == inverseLetterTree.end()) { return; }

    for (auto const & word : inverseLetterNodeIt->second) {
        removedWordList.insert(word);
    }
}

// Adds the removed words from the letter index node to the removed word list
void WordNetwork::addRemovedLetterIndexNode(char const & letter, unsigned char const & index, WordList & removedWordList)
{
    auto const letterNodeIt = letterIndexTree.find(letter);
    if (letterNodeIt == letterIndexTree.end()) { return; }

    auto const letterIndexNodeIt = letterNodeIt->second.find(index);
    if (letterIndexNodeIt == letterNodeIt->second.end()) { return; }

    auto const letterIndexList = letterIndexNodeIt->second;

    for (auto const & word : letterIndexList) {
        removedWordList.insert(word);
    }
}

// Adds the removed words from the inverse letter index node to the removed word list
void WordNetwork::addRemovedInverseLetterIndexNode(char const & letter, unsigned char const & index, WordList & removedWordList)
{
    auto const letterNodeIt = letterRepetitionTree.find(letter);
    if (letterNodeIt == letterIndexTree.end()) { return; }

    auto const letterIndexNodeIt = letterNodeIt->second.find(index);
    if (letterIndexNodeIt == letterNodeIt->second.end()) { return; }

    auto const letterIndexList = letterIndexNodeIt->second;

    for (auto const & word : letterIndexList) {
        removedWordList.insert(word);
    }
}

// Adds the removed words from the letter repetition node to the removed word list
void WordNetwork::addRemovedLetterRepetitionNode(char const & letter, unsigned char const & repetitions, WordList & removedWordList)
{
    auto const letterNodeIt = inverseLetterIndexTree.find(letter);
    if (letterNodeIt == letterIndexTree.end()) { return; }

    auto const letterRepetitionNodeIt = letterNodeIt->second.find(repetitions);
    if (letterRepetitionNodeIt == letterNodeIt->second.end()) { return; }

    auto const letterRepetitionList = letterRepetitionNodeIt->second;

    for (auto const & word : letterRepetitionList) {
        removedWordList.insert(word);
    }
}

std::string WordNetwork::bestNext(WordList & wordList)
{
    unsigned long long minWordScore = 18446744073709551615ULL;  // Max unsigned long long value
    std::string minWord = wordList.begin()->c_str();

    for (auto const & word : wordList) {
        // std::cout << "Calculating score for " << word << "... ";
        unsigned long long score = wordScore(word, wordList, minWordScore);
        // std::cout << score;
        if (score < minWordScore) {
            minWordScore = score;
            minWord = word;
            // std::cout << " New best!";
        }
        // std::cout << "\n";
    }

    return minWord;
}

// Returns a score proportional to the expected list residue of a guess word (lower is better)
unsigned long long WordNetwork::wordScore(std::string const & guessWord, WordList & wordList, unsigned long long & minWordScore)
{
    unsigned long long removedSum = 0;
    for (auto const & state : wordStates) {
        unsigned short removed = wordsRemoved(guessWord, state);
        removedSum += removed * removed;

        if (removedSum * removedSum > minWordScore) {    // Exits if word score is worse than current best
            // std::cout << "Aborting " << guessWord << " ";
            return minWordScore;
        }
    }
    return removedSum * removedSum;
}

std::string WordNetwork::bestNextMultiThreaded(WordList & wordList, unsigned short & threads)
{
    std::vector<WordList> sublists;
    for (unsigned char i = 0; i < threads; ++i) {
        sublists.push_back(WordList());
    }

    auto sublistsIt = sublists.begin();
    for (auto const & word : wordList) {
        if (sublistsIt == sublists.end()) { break; }

        sublistsIt->insert(word);
        ++sublistsIt;
    }

    WordScoreList bestList;
    for (auto const & sublist : sublists) {
        std::thread thread(bestNextThread, std::ref(sublist), std::ref(bestList));
        thread.join();
    }

    return "A";
}

void WordNetwork::bestNextThread(WordList & wordList, WordScoreList & bestList)
{
    unsigned long long minWordScore = 18446744073709551615ULL;
    std::string minWord = wordList.begin()->c_str();

    for (auto const & word : wordList) {
        unsigned long long score = wordScore(word, parentWordList, minWordScore);
        if (score < minWordScore) {
            minWordScore = score;
            minWord = word;
        }
    }

    mu.lock();
    bestList.push_back(std::make_pair(minWord, minWordScore));
    mu.unlock();
}

// Returns a word state based on guess word and assumed answer word
std::string WordNetwork::wordState(std::string const & guessWord, std::string const & answerWord)
{
    std::unordered_map<char, unsigned char> letterCounts;
    for (auto const & letter : answerWord) {
        letterCounts[letter] += 1;
    }

    char charArray[6] = {'0', '0', '0', '0', '0', '\0'};
    for (unsigned char i = 0; i < 5; ++i) {
        auto const letter = guessWord.at(i);
        auto presentLettersIt = letterCounts.find(letter);
        if (presentLettersIt != letterCounts.end() &&
            presentLettersIt->second > 0 &&
            letter == answerWord.at(i))
        {
            charArray[i] = '2';
            presentLettersIt->second -= 1;
        }
    }

    for (unsigned char i = 0; i < 5; ++i) {
        auto const letter = guessWord.at(i);
        auto presentLettersIt = letterCounts.find(letter);
        if (presentLettersIt != letterCounts.end() &&
            presentLettersIt->second > 0)
        {
            charArray[i] = '1';
            presentLettersIt->second -= 1;
        }
    }

    return charArray;
}