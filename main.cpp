#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::string getFirstUtf8Char(const std::string& str) {
    if (str.empty()) return "";
    unsigned char lead = str[0];
    size_t length = 1;

    if ((lead & 0xF8) == 0xF0) length = 4;
    else if ((lead & 0xF0) == 0xE0) length = 3;
    else if ((lead & 0xE0) == 0xC0) length = 2;

    return str.substr(0, length);
}

std::string normalizeUtf8(const std::string& ch) {
    if (ch.size() == 2) {
        unsigned char first = static_cast<unsigned char>(ch[0]);
        unsigned char second = static_cast<unsigned char>(ch[1]);
        if (first == 0xD0 && ((second >= 0x90 && second <= 0xAF) || second == 0x81)) {
            if (second == 0x81)
                return std::string("\xD1\x91");
            return std::string("\xD0") + char(second + 0x20);
        }
    }
    return ch;
}

std::vector<std::string> readLines(const std::string& filename) {
    std::ifstream infile(filename);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(infile, line)) {
        lines.push_back(line);
    }
    return lines;
}

size_t findMinLength(const std::vector<std::string>& lines) {
    size_t minLength = lines[0].size();
    for (const auto& l : lines) {
        if (l.size() < minLength) minLength = l.size();
    }
    return minLength;
}

void printMinLines(const std::vector<std::string>& lines, size_t minLength) {
    std::cout << "Строки с минимальной длиной (" << minLength << " символов):\n";
    for (const auto& l : lines) {
        if (l.size() == minLength) {
            std::cout << l << "\n";
        }
    }
}

int countLinesStartingWith(const std::vector<std::string>& lines, const std::string& letter) {
    int count = 0;
    for (const auto& l : lines) {
        if (!l.empty()) {
            std::string firstChar = normalizeUtf8(getFirstUtf8Char(l));
            if (firstChar == letter) count++;
        }
    }
    return count;
}

int countNonSpaceChars(const std::string& str) {
    int count = 0;
    for (char c : str) {
        if (c != ' ') count++;
    }
    return count;
}

int countInMinLengthLine(const std::vector<std::string>& lines, size_t minLength) {
    for (const auto& l : lines) {
        if (l.size() == minLength) {
            return countNonSpaceChars(l);
        }
    }
    return 0;
}

void appendResultsToFile(const std::string& filename, size_t minLength, int startCount, int charCount) {
    std::ofstream outfile(filename, std::ios::app);
    outfile << "\nРезультаты: \n";
    outfile << "Минимальная длина строки: " << minLength << "\n";
    outfile << "Строк, начинающихся с заданной буквы: " << startCount << "\n";
    outfile << "Количество символов в самой короткой строке: " << charCount << "\n";
    outfile.close();
}

int main() {
    std::string filename = "input.txt";
    std::vector<std::string> lines = readLines(filename);

    if (lines.empty()) {
        std::cout << "Файл пуст.\n";
        return 0;
    }

    size_t minLength = findMinLength(lines);
    printMinLines(lines, minLength);

    std::string inputChar;
    std::cout << "\nВведите первую букву:";
    std::getline(std::cin, inputChar);
    inputChar = normalizeUtf8(getFirstUtf8Char(inputChar));

    int startCount = countLinesStartingWith(lines, inputChar);
    int minStrCharCount = countInMinLengthLine(lines, minLength);

    std::cout << "\nСтрок, начинающихся с \"" << inputChar << "\": " << startCount << "\n";
    std::cout << "Символов в самой короткой строке: " << minStrCharCount << "\n";

    appendResultsToFile(filename, minLength, startCount, minStrCharCount);

    std::cout << "Результаты записаны в конец файла.\n";
    return 0;
}