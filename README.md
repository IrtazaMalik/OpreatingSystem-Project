# OpreatingSystem-Project
Developed a concurrent MapReduce framework for efficient text processing using multithreading. The system splits input into chunks, maps word counts in parallel, and reduces results with thread-safe aggregation using mutex locks. It ensures scalability, flexibility, and data integrity.

## Approach:
The project focuses on implementing a concurrent MapReduce framework to efficiently process
textual data in parallel. This framework splits input into smaller chunks, processes each chunk in
parallel threads during the Map Phase, and aggregates results in the Reduce Phase. Mutex
locks ensure thread-safe operations, maintaining data integrity throughout the workflow.

## Components:
The solution is based on the MapReduce paradigm, which involves:
1. Splitting Input: Breaking down the input string into manageable chunks.
2. Mapping: Processing each chunk to generate intermediate results (word counts).
3. Reducing: Aggregating the intermediate results to compute the final counts for each
word.
Each step is designed to utilize multithreading for parallel processing, improving efficiency and
scalability.
Custom Data Structures
 wordCountPair: Represents intermediate key-value pairs, containing a key (word) and its
count.
 reducedResult: Represents the final aggregated results, containing a key (word) and its
total count.
Phases of MapReduce Framework:

## 1. Map Phase
 Function: executeMapPhase
 Processes a chunk of text to identify and count unique words.
 For each word:
o If the word exists in the processing vector, increment its count.
o Otherwise, create a new wordCountPair for it.
 Concurrency:
o Multiple threads independently process different chunks.
o A mutex ensures thread safety while modifying the shared processing vector.
## 2. Shuffle Phase
 This is implicitly handled by the processing vector during the Map Phase, which groups
all key-value pairs by key.
## 3. Reduce Phase
 Function: executeReducePhase
 Aggregates counts for each unique word across all key-value pairs.
 For each unique key:
o Find all occurrences of the key in the processing vector.
o Sum the associated counts and store the result in the finalResults vector.
 Concurrency:
o Multiple threads process different keys in parallel.
o A mutex ensures thread safety while modifying the shared finalResults vector.
Main Function Workflow:

1. Input Splitting:
o Divides the input string into chunks using the divideIntoChunks function.
o Chunk size is determined dynamically based on the input size and phase
requirements.
2. Map Phase Execution:
o Creates threads for each chunk.
o Processes chunks in parallel using executeMapPhase.
3. Reduce Phase Execution:
o Creates threads for each unique key.
o Aggregates results in parallel using executeReducePhase.
4. Displaying Results:
o Outputs the aggregated word counts in the form (word, count).

## Test Cases and User Interaction:
 The program offers predefined test cases (easy, medium, hard) and the option for
custom input.
 Chunk sizes are adjusted to match the complexity of the test case.

## Thread-Safe Execution:
 mutex locks ensure synchronized access to shared resources (processing and
finalResults vectors), preventing race conditions.

## Key Advantages
 Scalability: The multithreaded approach handles large inputs efficiently.
 Flexibility: Users can select predefined cases or provide custom inputs.
 Thread Safety: Ensures data integrity during parallel processing.
Flow Diagram:

