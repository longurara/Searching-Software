// HƯỚNG DẪN BIÊN DỊCH QUAN TRỌNG
// ---------------------------------
// Chương trình này yêu cầu chuẩn C++17 hoặc mới hơn.
// Vui lòng sử dụng cờ -std=c++17 khi biên dịch với g++.
//
// Ví dụ: g++ -std=c++17 -o TimKiem.exe your_source_file.cpp
// ---------------------------------

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <limits>
#include <windows.h> // Sử dụng trực tiếp Windows API
#include <conio.h>   // Để đọc ký tự ngay lập tức (cho auto-complete)
#include <regex>     // Để phân tích mã nguồn và tìm tên hàm
#include <map>       // Dùng trong cấu trúc Trie
#include <set>       // Để lưu trữ các định danh duy nhất

// --- Cài đặt màu sắc cho Console ---
const std::string COLOR_GREEN = "\x1B[32m";
const std::string COLOR_YELLOW = "\x1B[33m";
const std::string COLOR_RED_BG = "\x1B[41m";
const std::string COLOR_CYAN = "\x1B[36m";
const std::string COLOR_NORMAL = "\x1B[0m";

// --- Các cấu trúc dữ liệu ---

struct SearchConfig {
    bool case_insensitive = false;
    bool whole_word = false;
};

struct SearchResult {
    std::wstring file_path;
    int line_number;
    std::string line_content;
    std::string found_keyword;
};

// --- Cấu trúc Trie và Auto-complete NÂNG CAO ---

// Cấu trúc để lưu một gợi ý và các thông số xếp hạng
struct Suggestion {
    std::string word;
    int frequency; // Tần suất xuất hiện
    int distance;  // Khoảng cách lỗi (Levenshtein)

    // Để so sánh và sắp xếp các gợi ý
    bool operator<(const Suggestion& other) const {
        if (distance != other.distance) {
            return distance < other.distance; // Ưu tiên khoảng cách lỗi nhỏ hơn
        }
        return frequency > other.frequency; // Sau đó ưu tiên tần suất cao hơn
    }
};


struct TrieNode {
    std::map<char, TrieNode*> children;
    bool isEndOfWord = false;
    int frequency = 0; // Đếm tần suất

    ~TrieNode() {
        for (auto& pair : children) {
            delete pair.second;
        }
    }
};

class Trie {
private:
    TrieNode* root;

    // Hàm đệ quy cho tìm kiếm mờ (Fuzzy Search)
    void find_suggestions_recursive(
        const std::string& word,
        TrieNode* node,
        std::string current_prefix,
        std::vector<int>& prev_lev_row,
        std::map<std::string, Suggestion>& collected_suggestions,
        int max_distance
    ) const {
        // Cập nhật dòng Levenshtein cho ký tự mới trong prefix
        std::vector<int> current_lev_row(word.length() + 1);
        current_lev_row[0] = current_prefix.length();

        for (size_t i = 1; i <= word.length(); ++i) {
            int insert_cost = current_lev_row[i - 1] + 1;
            int delete_cost = prev_lev_row[i] + 1;
            int replace_cost = prev_lev_row[i - 1] + (word[i - 1] == current_prefix.back() ? 0 : 1);
            current_lev_row[i] = std::min({insert_cost, delete_cost, replace_cost});
        }

        // Nếu từ này là một gợi ý hợp lệ (trong khoảng cách cho phép)
        if (node->isEndOfWord && current_lev_row.back() <= max_distance) {
            if (collected_suggestions.find(current_prefix) == collected_suggestions.end() || collected_suggestions[current_prefix].distance > current_lev_row.back()) {
                 collected_suggestions[current_prefix] = {current_prefix, node->frequency, current_lev_row.back()};
            }
        }

        // Tiếp tục tìm kiếm đệ quy nếu có khả năng tìm thấy từ tốt hơn
        if (*std::min_element(current_lev_row.begin(), current_lev_row.end()) <= max_distance) {
            for (const auto& pair : node->children) {
                find_suggestions_recursive(word, pair.second, current_prefix + pair.first, current_lev_row, collected_suggestions, max_distance);
            }
        }
    }


public:
    Trie() { root = new TrieNode(); }
    ~Trie() { delete root; }

    // Chèn một từ và cập nhật tần suất
    void insert(const std::string& word) {
        TrieNode* current = root;
        for (char ch : word) {
            if (current->children.find(ch) == current->children.end()) {
                current->children[ch] = new TrieNode();
            }
            current = current->children[ch];
        }
        current->isEndOfWord = true;
        current->frequency++;
    }

    // Hàm tìm kiếm gợi ý NÂNG CAO
    std::vector<std::string> find_suggestions(const std::string& prefix, int max_distance = 1) const {
        std::map<std::string, Suggestion> collected_suggestions;
        
        // Khởi tạo dòng đầu tiên của ma trận Levenshtein
        std::vector<int> initial_lev_row(prefix.length() + 1);
        for (size_t i = 0; i <= prefix.length(); ++i) {
            initial_lev_row[i] = i;
        }

        // Bắt đầu tìm kiếm từ gốc của Trie
        for (const auto& pair : root->children) {
            find_suggestions_recursive(prefix, pair.second, std::string(1, pair.first), initial_lev_row, collected_suggestions, max_distance);
        }

        // Chuyển kết quả từ map sang vector để sắp xếp
        std::vector<Suggestion> sorted_suggestions;
        for(const auto& pair : collected_suggestions) {
            sorted_suggestions.push_back(pair.second);
        }

        // Sắp xếp theo khoảng cách và tần suất
        std::sort(sorted_suggestions.begin(), sorted_suggestions.end());

        // Trả về 5 gợi ý tốt nhất
        std::vector<std::string> final_suggestions;
        for (int i = 0; i < std::min((int)sorted_suggestions.size(), 5); ++i) {
            final_suggestions.push_back(sorted_suggestions[i].word);
        }
        return final_suggestions;
    }
};


// --- Khai báo các hàm ---
void setup_console();
void clear_screen();
std::string to_lower(std::string s);
std::vector<std::string> expand_keywords(const std::string& input_keywords);
std::string highlight_keyword(const std::string& line, const std::string& keyword, const SearchConfig& config);
void search_in_file(const std::wstring& file_path, const std::vector<std::string>& keywords, const SearchConfig& config, std::vector<SearchResult>& results);
void traverse_directory(const std::wstring& directory, std::vector<std::wstring>& file_paths);
void display_results(const std::vector<SearchResult>& results, const SearchConfig& config);
void prompt_to_open_file(const std::vector<SearchResult>& results);
std::string wide_to_utf8(const std::wstring& wide_string);
std::wstring utf8_to_wide(const std::string& utf8_string);
void build_identifier_index(const std::vector<std::wstring>& file_paths, Trie& index);
std::string get_input_with_autocomplete(const Trie& index);

// --- Logic chính ---

int main() {
    setup_console();

    std::string path_str;
    std::cout << "Công cụ tìm kiếm C++ v2.0 (Multi-keyword TAB)\n";
    std::cout << "Vui lòng nhập đường dẫn đến thư mục cần tìm:\n> ";
    std::getline(std::cin, path_str);

    if (!path_str.empty() && path_str.front() == '"' && path_str.back() == '"') {
        path_str = path_str.substr(1, path_str.length() - 2);
    }

    std::wstring path_wstr = utf8_to_wide(path_str);

    DWORD file_attributes = GetFileAttributesW(path_wstr.c_str());
    if (file_attributes == INVALID_FILE_ATTRIBUTES || !(file_attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        std::cout << "Lỗi: Đường dẫn không hợp lệ hoặc không phải là thư mục.\n";
        Sleep(2000);
        return 1;
    }

    Trie identifier_index;
    std::vector<std::wstring> all_files;
    std::cout << "\nBắt đầu quét thư mục và xây dựng chỉ mục thông minh...\n";
    traverse_directory(path_wstr, all_files);
    build_identifier_index(all_files, identifier_index);
    std::cout << "Đã xây dựng xong chỉ mục. Sẵn sàng tìm kiếm.\n";
    Sleep(1500);

    while (true) {
        clear_screen();
        std::cout << "--------------------------------------------------\n";
        std::cout << "Tìm kiếm trong thư mục: " << path_str << "\n";
        std::cout << "--------------------------------------------------\n\n";
        std::cout << "Nhập từ khóa (nhấn TAB để tự hoàn thành, 'q' để thoát):\n> ";
        std::string keywords_input = get_input_with_autocomplete(identifier_index);

        if (keywords_input == "q" || keywords_input == "Q") break;

        SearchConfig config;
        char choice;
        std::cout << "\nTìm kiếm không phân biệt chữ hoa/thường? (y/n): ";
        std::cin >> choice;
        config.case_insensitive = (choice == 'y' || choice == 'Y');
        std::cout << "Chỉ tìm kiếm toàn bộ từ? (y/n): ";
        std::cin >> choice;
        config.whole_word = (choice == 'y' || choice == 'Y');
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            std::vector<std::string> keywords = expand_keywords(keywords_input);
            std::vector<SearchResult> all_results;
            std::cout << "\nBắt đầu tìm kiếm trên " << all_files.size() << " file...\n";
            for (const auto& file_path : all_files) {
                search_in_file(file_path, keywords, config, all_results);
            }
            std::cout << "Hoàn tất tìm kiếm.\n";
            display_results(all_results, config);
            prompt_to_open_file(all_results);
        } catch (const std::exception& e) {
            std::cerr << "Đã xảy ra lỗi: " << e.what() << std::endl;
            std::cout << "\nNhấn Enter để tiếp tục...";
            std::cin.get();
        }
    }

    std::cout << "\nChương trình đã thoát. Tạm biệt!" << std::endl;
    Sleep(1500);
    return 0;
}


// --- Triển khai các hàm ---

void setup_console() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

void clear_screen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten)) return;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten)) return;
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void build_identifier_index(const std::vector<std::wstring>& file_paths, Trie& index) {
    // Regex tìm các định danh (tên hàm, class, struct,...)
    std::regex identifier_regex(R"(\b([a-zA-Z_][a-zA-Z0-9_]*))");

    for (const auto& file_path : file_paths) {
        std::ifstream file(file_path.c_str());
        if (!file.is_open()) continue;

        std::string line;
        while (std::getline(file, line)) {
            auto words_begin = std::sregex_iterator(line.begin(), line.end(), identifier_regex);
            auto words_end = std::sregex_iterator();

            for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
                std::smatch match = *i;
                // Thay vì dùng set, giờ ta insert trực tiếp để đếm tần suất
                index.insert(match[1].str());
            }
        }
    }
}

std::string get_input_with_autocomplete(const Trie& index) {
    std::string input;
    std::vector<std::string> suggestions;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD start_coord = csbi.dwCursorPosition;

    while (true) {
        int ch = _getch();

        if (ch == 13) { // Enter
            SetConsoleCursorPosition(hConsole, {start_coord.X, (SHORT)(start_coord.Y + 1)});
            for(int i=0; i<6; ++i) {
                 std::cout << std::string(csbi.dwSize.X, ' ');
            }
            SetConsoleCursorPosition(hConsole, { (SHORT)(start_coord.X + input.length()), start_coord.Y });
            break;
        } else if (ch == 8) { // Backspace
            if (!input.empty()) input.pop_back();
        } else if (ch == 9) { // Tab
            if (!suggestions.empty()) {
                // Tìm vị trí của dấu phẩy cuối cùng
                size_t last_comma_pos = input.rfind(',');

                // Lấy phần chuỗi trước từ khóa hiện tại
                std::string base_part = (last_comma_pos == std::string::npos)
                                        ? ""
                                        : input.substr(0, last_comma_pos + 1);
                
                // Thêm dấu cách sau dấu phẩy nếu có
                if (!base_part.empty() && base_part.back() == ',') {
                    base_part += " ";
                }

                // Nối gợi ý và thêm dấu phẩy mới để sẵn sàng cho từ khóa tiếp theo
                input = base_part + suggestions[0] + ", ";
            }
        } else if (ch == 224) { // Arrow keys start with 224
            _getch(); // consume the second byte
        } else if (isprint(ch)) {
            input += static_cast<char>(ch);
        }
        
        // Xác định phần cần tìm gợi ý (phần sau dấu phẩy cuối cùng)
        std::string current_word_part = input;
        size_t last_comma_pos = input.rfind(',');
        if (last_comma_pos != std::string::npos) {
            current_word_part = input.substr(last_comma_pos + 1);
            // Xóa khoảng trắng ở đầu
            current_word_part.erase(0, current_word_part.find_first_not_of(" \t"));
        }


        if (!current_word_part.empty()) {
            suggestions = index.find_suggestions(current_word_part);
        } else {
            suggestions.clear();
        }

        SetConsoleCursorPosition(hConsole, {start_coord.X, (SHORT)(start_coord.Y + 1)});
        for(int i=0; i<6; ++i) {
             std::cout << std::string(csbi.dwSize.X, ' ');
        }
        SetConsoleCursorPosition(hConsole, start_coord);
        std::cout << std::string(csbi.dwSize.X - start_coord.X, ' ');
        SetConsoleCursorPosition(hConsole, start_coord);
        std::cout << input;

        if (!suggestions.empty()) {
            std::cout << COLOR_CYAN << " (Gợi ý: nhấn TAB để chọn)";
            for (size_t i = 0; i < suggestions.size(); ++i) {
                SetConsoleCursorPosition(hConsole, {start_coord.X, (SHORT)(start_coord.Y + i + 1)});
                std::cout << "  -> " << suggestions[i] << COLOR_NORMAL;
            }
        }
        SetConsoleCursorPosition(hConsole, { (SHORT)(start_coord.X + input.length()), start_coord.Y });
    }
    return input;
}

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

std::vector<std::string> expand_keywords(const std::string& input_keywords) {
    std::vector<std::string> keywords;
    std::stringstream ss(input_keywords);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t\n\r"));
        token.erase(token.find_last_not_of(" \t\n\r") + 1);
        if (!token.empty()) keywords.push_back(token);
    }
    return keywords;
}

std::string highlight_keyword(const std::string& line, const std::string& keyword, const SearchConfig& config) {
    std::string line_to_search = config.case_insensitive ? to_lower(line) : line;
    std::string keyword_to_search = config.case_insensitive ? to_lower(keyword) : keyword;
    std::string result;
    size_t start_pos = 0;
    size_t found_pos;
    while ((found_pos = line_to_search.find(keyword_to_search, start_pos)) != std::string::npos) {
        result.append(line, start_pos, found_pos - start_pos);
        result += COLOR_RED_BG;
        result.append(line, found_pos, keyword.length());
        result += COLOR_NORMAL;
        start_pos = found_pos + keyword.length();
    }
    result.append(line, start_pos, line.length() - start_pos);
    return result;
}

void search_in_file(const std::wstring& file_path, const std::vector<std::string>& keywords, const SearchConfig& config, std::vector<SearchResult>& results) {
    std::ifstream file(file_path.c_str());
    if (!file.is_open()) return;

    std::string line;
    int line_num = 0;
    while (std::getline(file, line)) {
        line_num++;
        for (const auto& keyword : keywords) {
            std::string line_to_search = config.case_insensitive ? to_lower(line) : line;
            std::string keyword_to_search = config.case_insensitive ? to_lower(keyword) : keyword;
            size_t pos = line_to_search.find(keyword_to_search);
            if (pos != std::string::npos) {
                if (config.whole_word) {
                    bool is_start_ok = (pos == 0) || !isalnum(static_cast<unsigned char>(line_to_search[pos - 1]));
                    bool is_end_ok = (pos + keyword_to_search.length() >= line_to_search.length()) || !isalnum(static_cast<unsigned char>(line_to_search[pos + keyword_to_search.length()]));
                    if (!is_start_ok || !is_end_ok) continue;
                }
                results.push_back({file_path, line_num, line, keyword});
                break;
            }
        }
    }
}

void traverse_directory(const std::wstring& directory, std::vector<std::wstring>& file_paths) {
    std::wstring search_path = directory + L"\\*";
    WIN32_FIND_DATAW find_data;
    HANDLE hFind = FindFirstFileW(search_path.c_str(), &find_data);
    if (hFind == INVALID_HANDLE_VALUE) return;
    do {
        if (wcscmp(find_data.cFileName, L".") != 0 && wcscmp(find_data.cFileName, L"..") != 0) {
            std::wstring full_path = directory + L"\\" + find_data.cFileName;
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                traverse_directory(full_path, file_paths);
            } else {
                const wchar_t* ext = wcsrchr(find_data.cFileName, L'.');
                if (ext && (wcscmp(ext, L".c") == 0 || wcscmp(ext, L".h") == 0 || wcscmp(ext, L".cpp") == 0)) {
                    file_paths.push_back(full_path);
                }
            }
        }
    } while (FindNextFileW(hFind, &find_data));
    FindClose(hFind);
}

std::wstring utf8_to_wide(const std::string& utf8_string) {
    if (utf8_string.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8_string[0], (int)utf8_string.size(), NULL, 0);
    std::wstring wstr_to(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &utf8_string[0], (int)utf8_string.size(), &wstr_to[0], size_needed);
    return wstr_to;
}

std::string wide_to_utf8(const std::wstring& wide_string) {
    if (wide_string.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wide_string[0], (int)wide_string.size(), NULL, 0, NULL, NULL);
    std::string str_to(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wide_string[0], (int)wide_string.size(), &str_to[0], size_needed, NULL, NULL);
    return str_to;
}

void display_results(const std::vector<SearchResult>& results, const SearchConfig& config) {
    if (results.empty()) {
        std::cout << "\nKhông tìm thấy kết quả nào.\n";
        return;
    }
    std::vector<std::wstring> unique_files;
    for(const auto& res : results) {
        if (std::find(unique_files.begin(), unique_files.end(), res.file_path) == unique_files.end()) {
            unique_files.push_back(res.file_path);
        }
    }

    for (size_t i = 0; i < unique_files.size(); ++i) {
        std::cout << "\n" << COLOR_GREEN << i + 1 << ": " << wide_to_utf8(unique_files[i]) << COLOR_NORMAL << "\n";
        for (const auto& res : results) {
            if (res.file_path == unique_files[i]) {
                std::string cleaned_line = res.line_content;
                cleaned_line.erase(std::remove(cleaned_line.begin(), cleaned_line.end(), '\r'), cleaned_line.end());
                cleaned_line.erase(std::remove(cleaned_line.begin(), cleaned_line.end(), '\n'), cleaned_line.end());
                std::cout << "  " << COLOR_YELLOW << "L" << res.line_number << ":" << COLOR_NORMAL << " "
                          << highlight_keyword(cleaned_line, res.found_keyword, config) << "\n";
            }
        }
    }
}

void prompt_to_open_file(const std::vector<SearchResult>& results) {
    if (results.empty()) {
        std::cout << "\nNhấn Enter để thực hiện tìm kiếm mới...";
        std::string dummy;
        std::getline(std::cin, dummy);
        return;
    }
    std::vector<std::wstring> found_files;
    for(const auto& res : results) {
        if (std::find(found_files.begin(), found_files.end(), res.file_path) == found_files.end()) {
            found_files.push_back(res.file_path);
        }
    }

    while (true) {
        std::cout << "\n--------------------------------------------------\n";
        std::cout << "Nhập số thứ tự của file để mở (1-" << found_files.size() << "), hoặc nhập 0 để tìm kiếm lại: ";
        std::string line;
        std::getline(std::cin, line);
        int choice = -1;
        try {
            choice = std::stoi(line);
        } catch(...) {
            choice = -1;
        }

        if (choice > 0 && choice <= static_cast<int>(found_files.size())) {
            ShellExecuteW(NULL, L"open", found_files[choice - 1].c_str(), NULL, NULL, SW_SHOWNORMAL);
            std::cout << "Đang yêu cầu hệ thống mở file: " << wide_to_utf8(found_files[choice - 1]) << "\n";
        } else if (choice == 0) {
            break;
        } else {
            std::cout << "Lựa chọn không hợp lệ. Vui lòng thử lại.\n";
        }
    }
}
