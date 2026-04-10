#include <fstream>
#include <filesystem>
#include <functional>
#include <algorithm>

#include "../../../inc/Controller/IO/FileException.hpp"
#include "../../../inc/Controller/IO/FileHandler.hpp"
#include "../../../inc/Shared/Utils/StringHelpers.hpp"

using std::string, std::filesystem::path;
namespace {

    string constructDefaultFilename(int counter) {
        string default_file_name = "new_file";
        string default_file_ending = ".txt";

        string numbered_name;
        if (counter == 0) {
            return default_file_name + default_file_ending;
        }
        
        return default_file_name + "_(" + std::to_string(counter) + ")" + default_file_ending;
    }

    std::ofstream openOutputFile(const TextFile& file) {
        path file_path = file.getFilepath();

        path abs_path = std::filesystem::absolute(file_path);
        path parent_directory = abs_path.parent_path();

        if (!parent_directory.empty() && !std::filesystem::exists(parent_directory)) {
            std::filesystem::create_directories(parent_directory);
        }

        std::ofstream output_file(abs_path.string());
        if (!output_file.is_open()) {
            throw FileException("Unable to open file "); 
        }
        return output_file; 
    }

    void writeToFile(std::ofstream& output_file, TextFile& file) {
        for (int i = 0; i < file.getNumberOfParagrahps(); i++) {
            const string& line = file.getParagraph(i);
            output_file << line; 
    
            if (!line.ends_with("\n") && i < file.getNumberOfParagrahps() - 1) {
                output_file << "\n";
            }
        }

        file.markAsSaved();
    }

    TextFile readFromFile(std::ifstream& input_file, const string& file_path) {
        TextFile file(file_path, SaveState::SAVED);
        string line;

        while (getline(input_file, line)) {
            file.writeToEnd(line, false);
        }

        file.calculateMetadata();
        file.markAsSaved();
        return file;
    }

    std::optional<char> parseOptionalChar(const string& argument) {
        //consume up to including { 
        size_t end_of_skip = argument.find_first_of('{');
        if (end_of_skip == string::npos || end_of_skip == argument.length() - 1) {
            return std::nullopt;
        }

        string remaining = argument.substr(end_of_skip + 1);

        // consume } at the end
        if (remaining.ends_with("}")) {
            remaining.pop_back();
        }
        
        // trim leading & trailing space
        StringHelpers::trimWrapping(remaining);

        if (remaining == "SPACE") {
            return ' ';
        }

        if (remaining.length() == 1) {
            return remaining[0];
        }

        return std::nullopt; 
    }

    std::vector<string> parseStringVector(const string& argument) {
        size_t end_of_skip = argument.find_first_of('{');
        if (end_of_skip == string::npos || end_of_skip == argument.length() - 1) {
            return {};
        }

        string remaining = argument.substr(end_of_skip + 1);
        std::vector<string> parsed;

        while (true) {
            size_t index = remaining.find_first_of(' ');
            if (index == string::npos) {
                break;
            }

            string token = remaining.substr(0, index);
            StringHelpers::trimWrapping(token);

            remaining = remaining.substr(index + 1);

            if (!token.empty() && token != "}") {
                parsed.push_back(token);
            }
        }

        return parsed;
    }

    string parseString(const string& argument) {
        size_t string_start = argument.find_first_of('"');
        if (string_start == string::npos || string_start == argument.length() - 1) {
            return "";
        }

        size_t string_end = argument.find_last_of('"');
        if (string_end == string_start) {
            return "";
        }

        return argument.substr(string_start + 1, string_end - string_start - 1);
    }

    bool parseBool(const std::string& argument) {
        if (argument.ends_with("true")) {
            return true;
        }

        return false; // invalid input -> assume false
    }

    std::vector<std::pair<string, string>> parsePairVector(const std::string& argument) {
        auto pairs = parseStringVector(argument);

        std::vector<std::pair<string, string>> result;

        for (auto pair_string : pairs) {
            if (!pair_string.starts_with('(') || !pair_string.ends_with(')')) {
                continue;
            }

            pair_string = pair_string.substr(1, pair_string.length() - 2);

            size_t seperator = pair_string.find_first_of(',');
            
            if (seperator == std::string::npos) {
                continue;
            }

            result.emplace_back(pair_string.substr(0, seperator), pair_string.substr(seperator + 1));

        }

        return result;
    }

    void parseArgument(const std::string& full_line, CodeHighlightInfo& info) {
        size_t seperator = full_line.find_first_of(' ');
        if (seperator == std::string::npos) {
            return;
        }

        std::string keyword = full_line.substr(0, seperator);
        std::string argument = full_line.substr(seperator + 1);

        if (keyword == "ignore_case") {
            info.m_ignore_case = parseBool(argument);
            return;
        }

        if (keyword == "language_extensions") {
            info.m_file_extensions = parseStringVector(argument);
            return;
        }

        if (keyword == "language_names") {
            info.m_language_names = parseStringVector(argument);
            return;
        }
        
        if (keyword == "language_keywords") {
            info.m_language_keywords = parseStringVector(argument);
            if (info.m_ignore_case) {
                std::sort(info.m_language_keywords.begin(), info.m_language_keywords.end(), StringHelpers::caselessCompare);
            }
            else {
                std::sort(info.m_language_keywords.begin(), info.m_language_keywords.end());
            }
            return;
        }

        if (keyword == "language_builtins") {
            info.m_language_builtins = parseStringVector(argument);
            if (info.m_ignore_case) {
                std::sort(info.m_language_builtins.begin(), info.m_language_builtins.end(), StringHelpers::caselessCompare);
            }
            else {
                std::sort(info.m_language_builtins.begin(), info.m_language_builtins.end());
            }
            return;
            return;
        }

        if (keyword == "string_indicators") {
            info.m_string_indicators = parsePairVector(argument);
            return;
        }

        if (keyword == "token_splitters") {
            info.m_token_splitters = parseString(argument);
            return;
        }

        if (keyword == "token_groups") {
            info.m_token_groups = parseStringVector(argument);
            return;
        }

        if (keyword == "ranged_comment") {
            info.m_ranged_comments = parsePairVector(argument);
            return;
        }

        if (keyword == "rest_of_line_comment") {
            info.m_rest_of_line_comments = parseStringVector(argument);
            return;
        }
        
        if (keyword == "number_seperator") {
            info.m_number_seperator = parseOptionalChar(argument);
            return;
        }

        throw std::logic_error("unknown keyword: " + keyword);
    }

    CodeHighlightInfo parseCodeLanguageFile(std::filesystem::path file_path) {
        std::filesystem::path absolute = std::filesystem::absolute(file_path);
        if (!std::filesystem::exists(absolute)) {
            throw std::logic_error("Path does not exist: " + absolute.string() + 
                " | Current WorkDir: " + std::filesystem::current_path().string());
            // return CodeHighlightInfo();
        }
        
        std::ifstream input_file(absolute);
        if (!input_file.is_open()) {
            throw FileException("Unable to open input file!");
        }

        CodeHighlightInfo info;

        string line;

        while (getline(input_file, line)) {
            parseArgument(line, info); 
        }

        input_file.close();

        return info;
    }
} // anonymous namespace


TextFile FileHandler::openFile(const string& file_path) {
    
    std::filesystem::path absolute = std::filesystem::absolute(file_path);
    if (!std::filesystem::exists(absolute)) {
        return createFile(absolute);
    }
    
    std::ifstream input_file(absolute);
    if (!input_file.is_open()) {
        throw FileException("Unable to open input file!");
    }

    TextFile file = readFromFile(input_file, absolute);
    input_file.close();

    return file;
}

TextFile FileHandler::createFile(path file_path) {
    TextFile file(file_path, SaveState::NEW_FILE);
    file.writeToEnd("");
    return file;
}


void FileHandler::saveFile(TextFile& file) {
    try {
        std::ofstream output_file = openOutputFile(file);
        writeToFile(output_file, file); 
        output_file.close();
    }
    catch (const std::filesystem::filesystem_error& e) {
        throw FileException("A filesytem error occurred when saving!");
    }
}

void FileHandler::renameFile(TextFile& file, string new_path) {
    path new_file_path(new_path);

    if (new_file_path.is_absolute()) {
        file.setFilepath(new_file_path);
    }
    else {
        path base_directory = file.getFilepath().parent_path();
        file.setFilepath(base_directory/new_path); // '/' operator is concatination here
    }
}

path FileHandler::getDefaultName() {
    int counter = 0;

    while (true) {
        string default_name = constructDefaultFilename(counter);
        path default_path = std::filesystem::absolute(default_name);

        if (!std::filesystem::exists(default_path)) {
            return default_path;
        }

        counter++;
    }
}

path FileHandler::createBackupLocation(path executable_path) {
    std::filesystem::path backup_dir = executable_path.parent_path() / "backups"; 
    std::filesystem::create_directories(backup_dir);

    return backup_dir;
}

path FileHandler::getBackupPath(path file_path, path backup_directory) {
    string backup_filename = file_path.stem().string()
        + " (backup)" + file_path.extension().string();

    return backup_directory / backup_filename;
}

std::vector<CodeHighlightInfo> FileHandler::parseAllCodeLanguageFiles(std::filesystem::path folder_path) {
    try {
        std::vector<CodeHighlightInfo> infos;
        if (!std::filesystem::exists(folder_path) || !std::filesystem::is_directory(folder_path)) {
            return {};
        }

        for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
            if (entry.path().filename().string() == "template.txt") {
                continue;
            }
            
            infos.push_back(parseCodeLanguageFile(entry.path()));
        }

        return infos;
    } catch (const std::filesystem::filesystem_error&) {
        return {}; // No need to crash, just continue without highlighting
    }
}