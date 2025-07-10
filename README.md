#C++ Intelli-Search: Advanced Source Code Search Tool
A blazingly fast, interactive command-line tool for searching within C++ source code, featuring intelligent auto-completion, fuzzy search, and result ranking.

**

✨ Key Features
This tool goes beyond simple text matching to provide a true developer-assistant experience.

🧠 Intelligent Auto-Completion
Fuzzy Search & Typo Tolerance: Finds what you mean, not just what you type. Misspell a function name (e.g., isrpime), and it will still suggest isPrime.

Smart Ranking: Suggestions are ranked by relevance. Functions and variables you use most frequently appear first.

Interactive Suggestions: Get real-time suggestions as you type.

Multi-Keyword Support: Press TAB to complete a keyword, and the tool automatically adds a ,  so you can immediately start typing the next one.

⚡️ High-Performance Engine
One-Time Indexing: The entire codebase is scanned only once at startup to build a high-speed search index, making subsequent searches instantaneous.

Efficient Trie Structure: Utilizes a Trie (prefix tree) data structure for instant suggestions, even across hundreds of files.

Regex-Powered Parsing: Employs regular expressions to accurately identify and index all valid identifiers (functions, variables, classes, etc.).

🖥️ Interactive & User-Friendly CLI
Real-time Keystroke Response: The UI updates instantly with every keypress, no need to wait for Enter.

Result Highlighting: Found keywords are highlighted directly in the console output for easy spotting.

Direct File Opening: Open any result file in your default editor directly from the search results.

Safe Console Handling: Uses direct Windows API calls for screen clearing, avoiding slower and less secure system() calls.

🚀 Getting Started
Prerequisites
A C++17 compliant compiler (e.g., MinGW g++, MSVC)

Windows Operating System

The <conio.h> header (typically included with Windows compilers)

Compilation
Clone the repository and compile the source file using g++:

g++ -std=c++17 -o CppIntelliSearch.exe main.cpp

Usage
Run the executable from your terminal.

./CppIntelliSearch.exe

Enter the path to the source code directory you want to search.

Start typing your query. Use TAB to auto-complete, and Enter to search.

🤝 Contributing
Contributions, issues, and feature requests are welcome! Feel free to check the issues page.

📄 License
This project is licensed under the MIT License - see the LICENSE.md file for details.
