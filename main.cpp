//code for basic part of programming project 2
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <deque>
#include <vector>
#include <algorithm>
#include <iomanip>

// Helper function to clean and tokenize line to single word output
std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> words;
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        // Helps remove punctuation and convert to lowercase **DEBUGGED**
        word.erase(std::remove_if(word.begin(), word.end(), [](char c) { return ispunct(c) && c != '\'' && c != '-'; }), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty()) {
            words.push_back(word);
        }
    }
    return words;
}

// Function that finds common phrases of N lenght
void findMostCommonPhrases(const std::string& filename1, const std::string& filename2, int phraseLength, std::ostream& output) {
    std::unordered_map<std::string, int> freqTomSawyer, freqHuckFinn; // applied use of hash maps that store phrase frequency

// Process each book
    for (const std::string& filename : {filename1, filename2}) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR" << filename << std::endl; // When File is not found
            return;
        }
//Chooses frequency map
        std::unordered_map<std::string, int>& freqMap = (filename == filename1) ? freqTomSawyer : freqHuckFinn;
        std::deque<std::string> wordQueue;  //Deque tracks word squence
        std::string line;

        // Read each line and process
        while (std::getline(file, line)) {
            std::vector<std::string> words = tokenize(line);
            for (const std::string& word : words) {
                wordQueue.push_back(word);
                if (wordQueue.size() == phraseLength) {
                    std::string phrase;
                    for (const std::string& w : wordQueue) {
                        phrase += (phrase.empty() ? "" : " ") + w;  //phrase builder
                    }
                    freqMap[phrase]++;   // increment phrase frequency
                    wordQueue.pop_front();  
                }
            }
        }
        file.close();
    }

    // Find the top phrases for given lenght
    std::vector<std::tuple<int, std::string, int>> topPhrases; // To store (total frequency, phrase, freq in Huck Finn)

    // Combination of phrases that appear in both files
    for (const auto& [phrase, count1] : freqTomSawyer) {
        int count2 = freqHuckFinn[phrase];
        int totalCount = count1 + count2;
        if (totalCount > 0) {
            topPhrases.emplace_back(totalCount, phrase, count2);
        }
    }

    // *descending order* frequency
    std::sort(topPhrases.rbegin(), topPhrases.rend());

    // Displays the top 10 phrases
    output << "Top 10 most frequent phrases of length " << phraseLength << "\n";
    output << std::setw(50) << "Phrase" 
           << std::setw(30) << "Frequency in Tom Sawyer" 
           << std::setw(30) << "Frequency in Huckleberry Finn" 
           << "\n";

    for (size_t i = 0; i < std::min(topPhrases.size(), size_t(10)); ++i) {
        const auto& [totalFreq, phrase, countHuckFinn] = topPhrases[i];
        output << std::setw(50) << phrase 
               << std::setw(30) << freqTomSawyer[phrase] 
               << std::setw(30) << countHuckFinn 
               << "\n";
    }
    output << "%%%%%%%%%%%%%%%%%%%%\n"; // I chose this as a seperator based on the example output on blackboard
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
