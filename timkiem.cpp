// HƯỚNG DẪN BIÊN DỊCH QUAN TRỌNG
// ---------------------------------
// Chương trình này yêu cầu chuẩn C++17 hoặc mới hơn.
// Vui lòng sử dụng cờ -std=c++17 khi biên dịch với g++.
//
// Ví dụ: g++ -std=c++17 -o TimKiem.exe your_source_file.cpp
//
// ** KHÔNG CẦN cờ -lstdc++fs với phiên bản này **
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
#include <conio.h>   // Để che mật khẩu khi nhập

// Define color codes for output
const std::string COLOR_GREEN = "\x1B[32m";
const std::string COLOR_YELLOW = "\x1B[33m";
const std::string COLOR_RED_BG = "\x1B[41m";
const std::string COLOR_NORMAL = "\x1B[0m";

// Struct to hold search configuration
struct SearchConfig {
    bool case_insensitive = false;
    bool whole_word = false;
};

// Struct to hold a single search result
struct SearchResult {
    std::wstring file_path;
    int line_number;
    std::string line_content;
    std::string found_keyword;
};

// --- Function Prototypes ---
void setup_console();
bool handle_login();
std::string to_lower(std::string s);
void generate_variants(const std::string& original, std::vector<std::string>& variants);
std::vector<std::string> expand_keywords(const std::string& input_keywords);
std::string highlight_keyword(const std::string& line, const std::string& keyword, const SearchConfig& config);
void search_in_file(const std::wstring& file_path, const std::vector<std::string>& keywords, const SearchConfig& config, std::vector<SearchResult>& results);
void traverse_directory(const std::wstring& directory, std::vector<std::wstring>& file_paths);
void display_results(const std::vector<SearchResult>& results, const SearchConfig& config);
void prompt_to_open_file(const std::vector<SearchResult>& results);
std::string wide_to_utf8(const std::wstring& wide_string);
std::wstring utf8_to_wide(const std::string& utf8_string);

// --- Main Logic ---

int main() {
    setup_console();

    if (!handle_login()) {
        std::cout << "\nĐăng nhập thất bại. Chương trình sẽ thoát." << std::endl;
        Sleep(2000);
        return 1;
    }

    std::cout << "\nĐăng nhập thành công!\n" << std::endl;

    std::string path_str;
    std::cout << "Công cụ tìm kiếm mã nguồn C/C++ v1.0\n";
    std::cout << "Vui lòng nhập đường dẫn đến thư mục cần tìm:\n> ";
    std::getline(std::cin, path_str);

    // Clean up path input (remove quotes)
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

    // Main search loop
    while (true) {
        system("cls"); // Clear the console for a new search

        std::cout << "--------------------------------------------------\n";
        std::cout << "Tìm kiếm trong thư mục: " << path_str << "\n";
        std::cout << "--------------------------------------------------\n\n";

        std::string keywords_input;
        std::cout << "Vui lòng nhập các từ khóa (cách nhau bằng dấu phẩy, hoặc nhập 'q' để thoát):\n> ";
        std::getline(std::cin, keywords_input);

        if (keywords_input == "q" || keywords_input == "Q") {
            break; // Exit the loop
        }

        SearchConfig config;
        char choice;
        std::cout << "Tìm kiếm không phân biệt chữ hoa/thường? (y/n): ";
        std::cin >> choice;
        config.case_insensitive = (choice == 'y' || choice == 'Y');

        std::cout << "Chỉ tìm kiếm toàn bộ từ? (y/n): ";
        std::cin >> choice;
        config.whole_word = (choice == 'y' || choice == 'Y');
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            std::vector<std::string> keywords = expand_keywords(keywords_input);
            std::vector<SearchResult> all_results;
            std::vector<std::wstring> files_to_search;

            std::cout << "\nBắt đầu quét thư mục...\n";
            traverse_directory(path_wstr, files_to_search);
            std::cout << "Đã tìm thấy " << files_to_search.size() << " file mã nguồn. Bắt đầu tìm kiếm...\n";

            for (const auto& file_path : files_to_search) {
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


// --- Function Implementations ---

void setup_console() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

/**
 * @brief Handles the user login process.
 * @return True if login is successful, false otherwise.
 */
bool handle_login() {
    const std::string correct_user = "longurara";
    const std::string correct_pass = "test1234";
    int attempts = 0;

    while (attempts < 3) {
        std::string username;
        std::string password;
        
        std::cout << "Tên đăng nhập: ";
        std::getline(std::cin, username);

        std::cout << "Mật khẩu: ";
        char ch;
        while ((ch = _getch()) != '\r') {
            if (ch == '\b') {
                if (!password.empty()) {
                    password.pop_back();
                    std::cout << "\b \b";
                }
            } else {
                password += ch;
                std::cout << '*';
            }
        }
        std::cout << std::endl;

        if (username == correct_user && password == correct_pass) {
            return true;
        } else {
            std::cout << "Tên đăng nhập hoặc mật khẩu không chính xác. Vui lòng thử lại.\n";
            attempts++;
            std::cout << "Số lần thử còn lại: " << 3 - attempts << "\n\n";
        }
    }
    return false;
}

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

void generate_variants(const std::string& original, std::vector<std::string>& variants) {
    std::string no_space_out;
    std::string underscore_out;
    for (size_t i = 0; i < original.length();) {
        unsigned char c1 = original[i];
        char base_char = 0;
        int step = 1;
        if (c1 == ' ') {
            underscore_out += '_';
            i++;
            continue;
        } else if (c1 < 128) {
            base_char = c1;
        } else {
            unsigned char c2 = (i + 1 < original.length()) ? original[i + 1] : 0;
            unsigned char c3 = (i + 2 < original.length()) ? original[i + 2] : 0;
            if ((c1==0xC3 && (c2>=0xA0 && c2<=0xA5)) || (c1==0xE1 && c2==0xBA && (c3>=0xA1 && c3<=0xB3)) || (c1==0xE1 && c2==0xBB && (c3>=0x81 && c3<=0x87))) {
                base_char = 'a'; step = (c1==0xC3)?2:3;
            } else if ((c1==0xC3 && (c2>=0xA8 && c2<=0xAB)) || (c1==0xE1 && c2==0xBA && (c3>=0xB5 && c3<=0xBF))) {
                base_char = 'e'; step = (c1==0xC3)?2:3;
            } else if (c1==0xC3 && (c2>=0xAC && c2<=0xAF)) {
                base_char = 'i'; step = 2;
            } else if ((c1==0xC3 && (c2>=0xB2 && c2<=0xB8)) || (c1==0xE1 && c2==0xBB && (c3>=0x8D && c3<=0x9F))) {
                base_char = 'o'; step = (c1==0xC3)?2:3;
            } else if ((c1==0xC3 && (c2>=0xB9 && c2<=0xBC)) || (c1==0xE1 && c2==0xBB && (c3>=0xA9 && c3<=0xB9))) {
                base_char = 'u'; step = (c1==0xC3)?2:3;
            } else if (c1==0xC3 && (c2==0xBD || c2==0xBF) || (c1==0xE1 && c2==0xBB && c3>=0xB3)) {
                base_char = 'y'; step = (c1==0xC3)?2:3;
            } else if (c1==0xC4 && c2==0x91) {
                base_char = 'd'; step = 2;
            }
        }
        if(base_char) {
            no_space_out += base_char;
            underscore_out += base_char;
        }
        i += step;
    }
    if (!no_space_out.empty()) variants.push_back(no_space_out);
    if (!underscore_out.empty()) variants.push_back(underscore_out);
}

std::vector<std::string> expand_keywords(const std::string& input_keywords) {
    std::vector<std::string> keywords;
    std::stringstream ss(input_keywords);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t\n\r"));
        token.erase(token.find_last_not_of(" \t\n\r") + 1);
        if (!token.empty()) {
            keywords.push_back(token);
            generate_variants(token, keywords);
        }
    }
    std::sort(keywords.begin(), keywords.end());
    keywords.erase(std::unique(keywords.begin(), keywords.end()), keywords.end());
    return keywords;
}

std::string highlight_keyword(const std::string& line, const std::string& keyword, const SearchConfig& config) {
    std::string line_to_search = config.case_insensitive ? to_lower(line) : line;
    std::string keyword_to_search = config.case_insensitive ? to_lower(keyword) : keyword;
    std::string result;
    size_t start_pos = 0;
    size_t found_pos = line_to_search.find(keyword_to_search, start_pos);
    while (found_pos != std::string::npos) {
        result.append(line, start_pos, found_pos - start_pos);
        result += COLOR_RED_BG;
        result.append(line, found_pos, keyword.length());
        result += COLOR_NORMAL;
        start_pos = found_pos + keyword.length();
        found_pos = line_to_search.find(keyword_to_search, start_pos);
    }
    result.append(line, start_pos, line.length() - start_pos);
    return result;
}

void search_in_file(const std::wstring& file_path, const std::vector<std::string>& keywords, const SearchConfig& config, std::vector<SearchResult>& results) {
    std::ifstream file(file_path.c_str()); 
    if (!file.is_open()) {
        return;
    }
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
                    if (!is_start_ok || !is_end_ok) {
                        continue;
                    }
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

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

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
    std::wstring current_file;
    int file_index = 0;
    for (const auto& res : results) {
        if (res.file_path != current_file) {
            current_file = res.file_path;
            file_index++;
            std::cout << "\n" << COLOR_GREEN << file_index << ": " << wide_to_utf8(current_file) << COLOR_NORMAL << "\n";
        }
        std::string cleaned_line = res.line_content;
        if (!cleaned_line.empty() && (cleaned_line.back() == '\n' || cleaned_line.back() == '\r')) {
            cleaned_line.pop_back();
        }
        if (!cleaned_line.empty() && (cleaned_line.back() == '\n' || cleaned_line.back() == '\r')) {
            cleaned_line.pop_back();
        }
        std::cout << "  " << COLOR_YELLOW << "L" << res.line_number << ":" << COLOR_NORMAL << " "
                  << highlight_keyword(cleaned_line, res.found_keyword, config) << "\n";
    }
}

void prompt_to_open_file(const std::vector<SearchResult>& results) {
    if (results.empty()) {
        // Nếu không có kết quả, đợi người dùng nhấn Enter để tiếp tục vòng lặp
        std::cout << "\nNhấn Enter để thực hiện tìm kiếm mới...";
        std::cin.get();
        return;
    }
    std::vector<std::wstring> found_files;
    for(const auto& res : results) {
        if (std::find(found_files.begin(), found_files.end(), res.file_path) == found_files.end()) {
            found_files.push_back(res.file_path);
        }
    }

    int choice = -1;
    while (true) {
        std::cout << "\n--------------------------------------------------\n";
        std::cout << "Nhập số thứ tự của file để mở (1-" << found_files.size() << "), hoặc nhập 0 để tìm kiếm lại: ";
        std::cin >> choice;
        if (std::cin.fail() || choice < 0 || choice > static_cast<int>(found_files.size())) {
            std::cout << "Lựa chọn không hợp lệ. Vui lòng nhập một số.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        if (choice == 0) {
            // Thoát khỏi vòng lặp mở file để quay lại vòng lặp tìm kiếm chính
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
        
        ShellExecuteW(NULL, L"open", found_files[choice - 1].c_str(), NULL, NULL, SW_SHOWNORMAL);
        
        std::string path_utf8 = wide_to_utf8(found_files[choice - 1]);
        std::cout << "Đang yêu cầu hệ thống mở file: " << path_utf8 << "\n";
    }
}
