#include <regex>

#include "../../../inc/Shared/Utils/StringHelpers.hpp"

using std::string, std::vector;

string StringHelpers::leftAlign(const string& str, unsigned int total_width, char pad_with) {

    if (str.length() >= total_width) {
        return str.substr(0, total_width);
    }

    unsigned int padding_needed = total_width - str.length();
    string output = str;
    output += string(padding_needed, pad_with); 

    return output; 
}

string StringHelpers::rightAlign(const string& str, unsigned int total_width, char pad_with) {

    if (str.length() >= total_width) {
        return str.substr(0, total_width);
    }

    unsigned int padding_needed = total_width - str.length();
    string output(padding_needed, pad_with);
    output += str;

    return output; 
}

string StringHelpers::addSeperators(int number, int group_size, char seperator) {
    string str = std::to_string(number);

    for (int i = static_cast<int>(str.size()) - group_size; i > 0; i -= group_size) {
        str.insert(str.begin() + i, seperator);
    }

    return str;
}

vector<string> StringHelpers::splitIntoRows(const string& paragraph,
    int start_column, int max_length) {

    if (paragraph.length() == 0) {
        return {""};
    }

    vector<string> split;

    for (int i = start_column; static_cast<size_t>(i) < paragraph.length(); i += max_length) {
        split.emplace_back(paragraph.substr(i, max_length));
    }

    return split;
}

int StringHelpers::countWords(const string& str) {

    if (str.length() == 0) {
        return 0;
    } 

    auto isWhitespace = [](char c) -> bool { return string(" \t\n").find(c) != string::npos; };

    int current = 0;
    int next = 1;
    int words = (isWhitespace(str.at(0))? 0 : 1);

    while (static_cast<size_t>(next) < str.length()) {
        if (isWhitespace(str.at(current)) && !isWhitespace(str.at(next))) {
            words++;
        }

        current++;
        next++;
    }

    return words;
}

void StringHelpers::uppercase(std::string& str) {
    for (char& c : str) {
        c = std::toupper(c);
    }
}

void StringHelpers::lowercase(std::string& str) {
    for (char& c : str) {
        c = std::tolower(c);
    }
}


void StringHelpers::trimWrapping(std::string& str, char removed) {
    size_t start = str.find_first_not_of(removed);
    size_t end = str.find_last_not_of(removed);

    if (start == std::string::npos || end == std::string::npos) {
        str = "";
        return;
    }
    
    str = str.substr(start, end - start + 1);
}

bool StringHelpers::caselessEquals(const std::string& a, const std::string& b) {
    if (a.length() != b.length()) {
        return false;
    }

    for (size_t i = 0; i < a.length(); i++) {
        if (std::tolower(a[i]) != std::tolower(b[i])) {
            return false;
        }
    }

    return true;
}

std::optional<char> StringHelpers::firstNonSpace(const std::string& str) {
    size_t index = str.find_first_not_of(' ');

    if (index == std::string::npos) {
        return std::nullopt;
    }

    return str.at(index);
}

bool StringHelpers::startsWith(const std::string& str, char first) {
    if (str.empty()) {
        return false;
    }

    return str.at(0) == first;
}

bool StringHelpers::startsWithIgnoringWhitespace(const std::string& str, char first) {
    size_t index = str.find_first_not_of(' ');

    if (index == std::string::npos) {
        return false;
    }

    return str.at(index) == first;
}

bool StringHelpers::startsWithIgnoringWhitespace(const std::string& str, const std::string& start) {
    size_t index = str.find_first_not_of(' ');

    if (index == std::string::npos) {
        return false;
    }

    auto after_whitespace = str.substr(index);
    return after_whitespace.starts_with(start);
}

bool StringHelpers::consistsOnlyOfIgnoringWhitespace(const std::string& str, char comparison) {
    for (char c : str) {
        if (c != comparison && c != ' ') {
            return false;
        }
    }

    return true;
}

bool StringHelpers::consistsOnlyOf(const std::string& str, char comparison) {
    for (char c : str) {
        if (c != comparison) {
            return false;
        }
    }

    return true;
}

size_t StringHelpers::countLeadingSpaces(const std::string& str) {
    size_t index = str.find_first_not_of(' ');

    if (index == std::string::npos) {
        return 0;
    }
    
    return index;
}

size_t StringHelpers::findFirstNonDigit(const std::string& str) {
    return str.find_first_not_of("0123456789");
}


bool StringHelpers::isNumberLiteral(const std::string& str, std::optional<char> number_seperator) {
    return isBinaryNumber(str, number_seperator)
        || isOctalNumber(str, number_seperator)
        || isHexNumber(str, number_seperator)
        || isDecimalNumber(str, number_seperator);
}

bool StringHelpers::isBinaryNumber(const std::string& str, std::optional<char> number_seperator) {
    std::regex expression;
    if (number_seperator == std::nullopt) {
        expression = std::regex("^0b[01]+$");
    }
    else {
        std::string regex_str = "^0b[01]+([" + std::string(1,*number_seperator) + "01]?[01]+)*$";
        expression = std::regex(regex_str);
    }

    return std::regex_match(str, expression);
}

bool StringHelpers::isOctalNumber(const std::string& str, std::optional<char> number_seperator) {
    std::regex expression;
    if (number_seperator == std::nullopt) {
        expression = std::regex("^0o[0-7]+$");
    }
    else {
        std::string regex_str = "^0o[0-7]+([" + std::string(1,*number_seperator) + "0-7]?[0-7]+)*$";
        expression = std::regex(regex_str);
    }

    return std::regex_match(str, expression);
}

bool StringHelpers::isHexNumber(const std::string& str, std::optional<char> number_seperator) {
    std::regex expression;
    if (number_seperator == std::nullopt) {
        expression = std::regex("^0x[0-9a-fA-F]+$");
    }
    else {
        std::string regex_str = "^0x[0-9a-fA-F]+([" + std::string(1,*number_seperator) + "0-9a-fA-F]?[0-9a-fA-F]+)*$";
        expression = std::regex(regex_str);
    }

    return std::regex_match(str, expression);

}

bool StringHelpers::isDecimalNumber(const std::string& str, std::optional<char> number_seperator) {
    std::regex expression;
    if (number_seperator == std::nullopt) {
        expression = std::regex("^[0-9]+$");
    }
    else {
        std::string regex_str = "^[0-9]+(" + std::string(1, *number_seperator) + "[0-9]+)*$";
        expression = std::regex(regex_str);
    }

    return std::regex_match(str, expression);
}
