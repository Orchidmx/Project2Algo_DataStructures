// Code for finding top common phrases
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <deque>
#include <vector>
#include <algorithm>

// Helper function to clean and tokenize line to single word output
std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> words;
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        word.erase(std::remove_if(word.begin(), word.end(), [](char c) { return ispunct(c) && c != '\'' && c != '-'; }), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty()) {
            words.push_back(word);
        }
    }
    return words;
}

// Function to find common phrases of a given length
void findMostCommonPhrases(const std::string& filename1, const std::string& filename2, int phraseLength, std::ostream& output) {
    std::unordered_map<std::string, int> freqTomSawyer, freqHuckFinn;

    // Process each book
    for (const std::string& filename : {filename1, filename2}) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        std::unordered_map<std::string, int>& freqMap = (filename == filename1) ? freqTomSawyer : freqHuckFinn;
        std::deque<std::string> wordQueue;
        std::string line;

        while (std::getline(file, line)) {
            std::vector<std::string> words = tokenize(line);
            for (const std::string& word : words) {
                wordQueue.push_back(word);
                if (wordQueue.size() == phraseLength) {
                    std::string phrase;
                    for (const std::string& w : wordQueue) {
                        phrase += (phrase.empty() ? "" : " ") + w;
                    }
                    freqMap[phrase]++;
                    wordQueue.pop_front();
                }
            }
        }
        file.close();
    }

    // Collect only common phrases
    std::vector<std::pair<int, std::string>> commonPhrases;
    for (const auto& [phrase, count1] : freqTomSawyer) {
        if (freqHuckFinn.find(phrase) != freqHuckFinn.end()) {
            int totalCount = count1 + freqHuckFinn[phrase];
            commonPhrases.emplace_back(totalCount, phrase);
        }
    }

    // Sort by total frequency in descending order
    std::sort(commonPhrases.rbegin(), commonPhrases.rend());

    // Output results
    output << "phrases with " << phraseLength << " words\n";
    for (size_t i = 0; i < std::min(commonPhrases.size(), size_t(10)); ++i) {
        const auto& [totalFreq, phrase] = commonPhrases[i];
        output << phrase << "**" << totalFreq << "\n";
    }
    output << "%%%%%%%%%%%%%%%%%%%%\n";
}

int main() {
    std::ofstream outFile("topfoundPhrases_output.txt");
    if (!outFile) {
        std::cerr << "Error creating output file\n";
        return 1;
    }

    const std::string filename1 = "tomSawyer.txt";
    const std::string filename2 = "huckleberryFinn.txt";

    for (int phraseLength = 1; phraseLength <= 10; ++phraseLength) {
        findMostCommonPhrases(filename1, filename2, phraseLength, outFile);
    }

    outFile.close();
    std::cout << "Top phrases written to topfoundPhrases_output.txt\n";
    return 0;
}
