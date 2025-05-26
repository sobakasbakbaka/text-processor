#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

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

int countNonSpaceChars(const std::string& str) {
    int count = 0;
    for (char c : str) {
        if (c != ' ') count++;
    }
    return count;
}

int main() {
    std::string filename = "input.txt";
    std::ifstream infile(filename);
    std::vector<std::string> lines;

    if (!infile) {
        std::cerr << "Ошибка открытия файла.\n";
        return 1;
    }

    std::string line;
    while (std::getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();

    if (lines.empty()) {
        std::cout << "Файл пуст.\n";
        return 0;
    }

    size_t minLength = lines[0].size();
    for (const auto& l : lines) {
        if (l.size() < minLength) {
            minLength = l.size();
        }
    }

    std::cout << "Строки с минимальной длиной (" << minLength << " символов):\n";
    for (const auto& l : lines) {
        if (l.size() == minLength) {
            std::cout << l << "\n";
        }
    }

    std::string inputChar;
    std::cout << "\nВведите первую букву (UTF-8): ";
    std::getline(std::cin, inputChar);
    inputChar = normalizeUtf8(getFirstUtf8Char(inputChar));

    int startCount = 0;
    for (const auto& l : lines) {
        if (l.empty()) continue;
        std::string firstChar = normalizeUtf8(getFirstUtf8Char(l));
        if (firstChar == inputChar) {
            startCount++;
        }
    }

    int minStrCharCount = 0;
    for (const auto& l : lines) {
        if (l.size() == minLength) {
            minStrCharCount = countNonSpaceChars(l);
            break;
        }
    }

    std::cout << "\nСтрок, начинающихся с \"" << inputChar << "\": " << startCount << "\n";
    std::cout << "Символов (без пробелов) в самой короткой строке: " << minStrCharCount << "\n";

    std::ofstream outfile(filename, std::ios::app);
    outfile << "\nРезультаты: \n";
    outfile << "Минимальная длина строки (в символах): " << minLength << "\n";
    outfile << "Строк, начинающихся с заданной буквы: " << startCount << "\n";
    outfile << "Количество символов (без пробелов) в самой короткой строке: " << minStrCharCount << "\n";
    outfile.close();

    std::cout << "Результаты записаны в конец файла.\n";
    return 0;
}