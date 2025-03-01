#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <pthread.h>

using namespace std;

// Creating a structure to store key-value pairs which contains the input word and its count
struct wordCountPair {
    string key;
    int count;
};

// Creating a structure to store the final results which contains the key and its total count
struct reducedResult {
    string key;
    int val;
};

// Creating a structure for map data passed to threads which contains the processing vector, chunk, and mutex lock
struct mapInput {
    vector<wordCountPair>& processing;
    string chunk;
    mutex& syncMutex;
};

// Creating a structure for reduce data passed to threads which contains the processing vector, final results vector, key, and mutex lock
struct ReduceData {
    vector<wordCountPair>& processing;
    vector<reducedResult>& finalResults;
    string key;
    mutex& syncMutex;
};

// Creating a function to find a key in the vector of key-value pairs
wordCountPair* findKeyinVector(vector<wordCountPair>& vec, const string& key) {
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i].key == key) {
            return &vec[i];
        }
    }
    return nullptr;
}

// Creating a function to split a string into words based on spaces and return a vector of words
vector<string> splitIntoWords(const string& str) {
    vector<string> words;
    size_t start = 0, end = 0;

    while (end < str.length()) {
        while (end < str.length() && str[end] == ' ') ++end;
        start = end;
        while (end < str.length() && str[end] != ' ') ++end;

        if (start < end) {
            words.push_back(str.substr(start, end - start));
        }
    }

    return words;
}

// Creating a function to create chunks of input based on the chunk size and return a vector of chunks
vector<string> divideIntoChunks(const string& input, size_t chunkSize) {
    vector<string> chunks;
    vector<string> words = splitIntoWords(input);
    size_t numChunks = (words.size() + chunkSize - 1) / chunkSize;

    for (size_t i = 0; i < numChunks; ++i) {
        string chunk = "";
        for (size_t j = i * chunkSize; j < (i + 1) * chunkSize && j < words.size(); ++j) {
            chunk += words[j] + " ";
        }
        chunks.push_back(chunk);
    }

    return chunks;
}

// Creating a map function executed by threads to process the input chunks 
void* executeMapPhase(void* arg) {
    mapInput* data = static_cast<mapInput*>(arg);
    vector<string> words = splitIntoWords(data->chunk);

    lock_guard<mutex> lock(data->syncMutex);

    for (const string& word : words) {
        wordCountPair* keyValue = findKeyinVector(data->processing, word);

        if (keyValue) {
            keyValue->count++;
        } else {
            data->processing.push_back({word, 1});
        }
    }

    return nullptr;
}

// Creating a reduce function executed by threads to process the key-value pairs and calculate the total count for each key
void* executeReducePhase(void* arg) {
    ReduceData* data = static_cast<ReduceData*>(arg);
    int sum = 0;

    lock_guard<mutex> lock(data->syncMutex);

    for (const wordCountPair& kv : data->processing) {
        if (kv.key == data->key) {
            sum += kv.count;
            break;
        }
    }

    data->finalResults.push_back({data->key, sum});
    return nullptr;
}

// Creating a function to process the input string by creating chunks, mapping the words, and reducing the key-value pairs
void executeMapReduce(const string& input, size_t chunkInputSize) {
    vector<string> chunkInput = divideIntoChunks(input, chunkInputSize);
    vector<wordCountPair> processing;
    mutex syncMutex;
    vector<pthread_t> mapperThreads(chunkInput.size());
    vector<mapInput> mapData(chunkInput.size(), {processing, "", syncMutex});

    for (size_t i = 0; i < chunkInput.size(); ++i) {
        mapData[i].chunk = chunkInput[i];
        pthread_create(&mapperThreads[i], nullptr, executeMapPhase, &mapData[i]);
    }

    for (size_t i = 0; i < chunkInput.size(); ++i) {
        pthread_join(mapperThreads[i], nullptr);
    }

    vector<reducedResult> finalReducedResults;
    vector<pthread_t> reducerThreads(processing.size());
    vector<ReduceData> reduceData(processing.size(), {processing, finalReducedResults, "", syncMutex});

    for (size_t i = 0; i < processing.size(); ++i) {
        reduceData[i].key = processing[i].key;
        pthread_create(&reducerThreads[i], nullptr, executeReducePhase, &reduceData[i]);
    }

    for (size_t i = 0; i < processing.size(); ++i) {
        pthread_join(reducerThreads[i], nullptr);
    }

    cout << "Results: ";
    for (const auto& result : finalReducedResults) {
        cout << "(" << result.key << ", " << result.val << "), ";
    }
    cout << "\n";
}

int main() {

    // Creating test cases for easy, medium, and hard cases
    vector<string> easyTestCases = {
        "apple banana grape apple apple grape banana banana grape",
        "rose lily tulip rose rose tulip lily lily tulip",
        "Mercury Venus Earth Mars Venus Earth Mars Jupiter Saturn"
    };

    vector<string> mediumTestCases = {
        "apple banana grape cherry lemon orange apple banana grape cherry lemon orange cherry lemon orange banana grape cherry lemon",
        "rose lily tulip orchid dandelion tulip orchid dandelion rose lily tulip rose lily tulip dandelion tulip orchid dandelion",
        "Mercury Venus Earth Mars Jupiter Saturn Mars Jupiter Saturn Venus Earth Mars Jupiter Saturn Mars Jupiter Saturn Venus Earth Mars Jupiter Saturn"
    };

    vector<string> hardTestCases = {
        "apple banana grape cherry lemon orange peach plum nectarine apricot blueberry raspberry blackberry apple banana grape cherry lemon apple banana grape cherry lemon orange peach plum nectarine apricot blueberry raspberry blackberry orange peach apple banana grape cherry lemon apple banana grape cherry lemon orange peach plum apple banana grape cherry lemon orange peach plum nectarine apricot blueberry raspberry blackberry nectarine apricot blueberry raspberry blackberry orange peach plum nectarine apricot blueberry raspberry blackberry plum nectarine apricot blueberry raspberry blackberry",
        "rose rose lily tulip orchid dandelion chrysanthemum daffodil iris sunflower marigold lily tulip orchid rose lily tulip orchid dandelion rose lily tulip orchid dandelion chrysanthemum daffodil iris sunflower marigold chrysanthemum daffodil rose lily tulip orchid dandelion chrysanthemum daffodil iris sunflower marigold iris sunflower marigold dandelion rose lily tulip orchid dandelion rose lily tulip orchid dandelion chrysanthemum daffodil iris sunflower marigold chrysanthemum daffodil iris sunflower marigold chrysanthemum daffodil iris sunflower marigold rose lily tulip orchid dandelion chrysanthemum daffodil iris sunflower marigold",
        "Mercury Venus Earth Mars Jupiter Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune Pluto Mercury Venus Earth Mars Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune Pluto Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune Pluto Jupiter Saturn Uranus Neptune Pluto Saturn Uranus Neptune Pluto Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune Pluto Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune Pluto Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune Pluto Mercury Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune Pluto Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune Pluto Venus Earth Mars Jupiter Saturn Uranus Neptune Pluto Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune Pluto"
    };

    // Creating a menu for the user to select an option to execute the program
    while (true) {
        int userChoice;
        cout << "\nChoose an option:\n";
        cout << "1. Easy test cases\n";
        cout << "2. Medium test cases\n";
        cout << "3. Hard test cases\n";
        cout << "4. Enter your own string\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> userChoice;
        cin.ignore(); // Clearing the input buffer 


        // Exiting the program based on user choice
        if (userChoice == 5) {
            cout << "Exiting the program. Goodbye!\n";
            break;
        }

        switch (userChoice) {
            case 1: {
                cout << "\nEasy Test Cases:\n";
                for (const string& input : easyTestCases) {
                    cout << "Input: " << input << "\n";
                    executeMapReduce(input, 2); // Small chunks for easy cases
                }
                break;
            }
            case 2: {
                cout << "\nMedium Test Cases:\n";
                for (const string& input : mediumTestCases) {
                    cout << "Input: " << input << "\n";
                    executeMapReduce(input, 3); // Medium chunks for medium cases
                }
                break;
            }
            case 3: {
                cout << "\nHard Test Cases:\n";
                for (const string& input : hardTestCases) {
                    cout << "Input: " << input << "\n";
                    executeMapReduce(input, 5); // Larger chunks for hard cases
                }
                break;
            }
            case 4: {
                cout << "\nEnter your custom string: ";
                string inputText;
                getline(cin, inputText);
                cout << "Processing your input:\n";
                executeMapReduce(inputText, 4); // Default chunk size for custom input
                break;
            }
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    }

    return 0;
}

