#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <regex>
int StringCount(FILE* file) {
    fseek(file, 0, SEEK_SET);
    int count = 0;
    int ch;
    int lch= '\n';
    while ((ch  = fgetc(file)) != EOF) {
        if (ch  == '\n') {
           count++;
        }
        lch = ch;
    }
    if ((lch != '\n') && (lch != EOF)) {
        count++;
    }
    rewind(file);
    return count;
}
//int CharToInt(char h) {
//    switch (h) {
//    case '1':
//        return 1;
//    case '2':
//        return 2;
//    case '3':
//        return 3;
//    case '4':
//        return 4;
//    case '5':
//        return 5;
//    case '6':
//        return 6;
//    case '7':
//        return 7;
//    case '8':
//        return 8;
//    case '9':
//        return 9;
//    case '0':
//        return 0;
//    }
//}
struct Flight {
    char* Type;
    char* TailNum;
    char* FlightNum;
    char* LandTime;
};
void PrintFlightInfo(Flight* flight) {
    std::cout << "Type: " << flight->Type << "\t" 
        << "Tail number : " << flight->TailNum << "\t" 
        << "Flight number: " << flight->FlightNum << "\t" 
        << "Landing Time : " << flight->LandTime << "\n";
}
int StrLen(const char* str) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}
char* StrCopy(const char* w) {
    int wSize = StrLen(w);
    char* h = new char[wSize+1];
    for (int i = 0;i < wSize;i++) {
        h[i] = w[i];
    }
    h[wSize] = '\0';
    return h;
}
char* String() {
    int size = 16;
    int len = 0;
    char* str = (char*)malloc(size * sizeof(char));
    char ch;
    if (!str) {
        return NULL;
    }
    ch = getchar();
    while (ch != '\n' && ch != EOF) {
        if (len >= size - 1) {
            size *= 2;
            char* newStr = (char*)realloc(str, size * sizeof(char));
            if (!newStr) {
                free(str);
                return NULL;
            }
            str = newStr;
        }
        str[len++] = ch;
        ch = getchar();
    }
    str[len] = '\0';
    return str;
}
int StringToInt(const char* str) {
    int res = 0;
    int j = 0;
    if (str[0] == '-') {
        j++;
    }
    for (;str[j] != '\0';j++) {
        if (str[j] >= '0' && str[j] <= '9') {
            res = res * 10 + (str[j] - '0');
        }
        else {
            break;
        }
    }
    return str[0] != '-' ? res : -res;
}
int FindFlightNum(char* flightNum) {
    std::regex number(R"([0-9]+)");
    std::cmatch match;
    if (std::regex_search(flightNum, match,number )) {
        return StringToInt(match.str().c_str());
    }
}
bool BiggerTime(char* t1, char* t2) {
    std::regex time(R"([0-9]+)");
    std::cmatch match;
    auto current = std::cregex_iterator(t1, t1+StrLen(t1), time);
    auto end = std::cregex_iterator();
    int hours1 = 0;
    int hours2 = 0;
    int minutes1 = 0;
    int minutes2 = 0;
    int u = 0;
    for (; current != end; ++current) {
        std::cmatch match = *current;
        if (u == 0) {
            hours1 = StringToInt(match.str().c_str());
        }
        else {
            minutes1 = StringToInt(match.str().c_str());
        }
        u++;
    }
    current = std::cregex_iterator(t2, t2 + StrLen(t2), time);
    end = std::cregex_iterator();
    u = 0;
    for (; current != end; ++current) {
        std::cmatch match = *current;
        if (u == 0) {
            hours2 = StringToInt(match.str().c_str());
        }
        else {
            minutes2 = StringToInt(match.str().c_str());
        }
        u++;
    }
    return (hours1 > hours2) || ((hours1 == hours2) && (minutes1 > minutes2));
}
void SortFlying(Flight** base,int len) {
    Flight* temp = nullptr;
    int j = 0;
    int curFlightNum = 0;
    while (j < len-1) {
        while ((base[j]->LandTime[0] != '-') && j < len-1) {
            j++;
        }
        Flight* min = nullptr;
        int minIndex = -1;
        int minFlightNum = INFINITY;
        for (int i = j;i < len;i++) {
            curFlightNum = FindFlightNum(base[i]->FlightNum);
            if (((base[i]->LandTime[0]) == '-') && (((min != nullptr) && (minFlightNum > curFlightNum)) || (min == nullptr))) {
                min = base[i];
                minIndex = i;
                minFlightNum = curFlightNum;
            }
        }
        if (min != nullptr) {
           temp = base[j];
           base[j] = min;
           base[minIndex] = temp;
        }
        j++;
    }
}

void SortLanding(Flight ** base,int len) {
    Flight* temp = nullptr;
    int j = 0;
    while (j < len - 1) {
        while ((j < len - 1) && (base[j]->LandTime[0] == '-')){
            j++;
        }
        Flight* min = nullptr;
        int minIndex = -1;
        for (int i = j;i < len;i++) {
            if ((base[i]->LandTime[0] != '-') && (((min != nullptr) && (BiggerTime(min->LandTime,base[i]->LandTime))) || (min == nullptr))) {
                min = base[i];
                minIndex = i;
            }
        }
        if (min != nullptr) {
            temp = base[j];
            base[j] = min;
            base[minIndex] = temp;
        }
        j++;
    }
}
Flight* StringToFlight(char* str) {
    int u = 0;
    char* type = nullptr;
    char* tailNum = nullptr;
    char* flightNum = nullptr;
    char* landTime = nullptr;
    Flight* res = new Flight;
    std::regex word(R"(\S+)");
    auto words_begin = std::cregex_iterator(str, str + StrLen(str), word);
    auto words_end = std::cregex_iterator();
    for (std::cregex_iterator i = words_begin; i != words_end; ++i) {
        if (u == 0) {
            std::cmatch match = *i;
            type = StrCopy(match.str().c_str());
        }
        else if (u == 1) {
            std::cmatch match = *i;
            tailNum = StrCopy(match.str().c_str());
        }
        else if (u == 2) {
            std::cmatch match = *i;
            flightNum = StrCopy(match.str().c_str());
        }
        else if (u == 3) {
            std::cmatch match = *i;
            landTime = StrCopy(match.str().c_str());
        }
        else {
            break;
        }
        u++;
    }
    res->Type = type;
    res->TailNum = tailNum;
    res->FlightNum = flightNum;
    res->LandTime = landTime;
    return res;
}
//Flight* StringToFlight(char* str) {
//    int i = 0;
//    int j = 0;
//    int t = 0;
//    char* type = {0};
//    char* tailNum = {0};
//    int flightNum = 0;
//    char* landTime = {0};
//    Flight* res = new Flight;
//    
//    res->TailNum = tailNum;
//    res->FlightNum = flightNum;
//    res->LandTime = landTime;
//    while (!(isalpha(str[i]) || (('0' <= str[i]) && (str[i] <= '9')))) {
//        i++;
//    }
//    while (isalpha(str[i]) || (('0' <= str[i]) && (str[i] <= '9'))){
//        type[j] = str[i];
//        i++;
//        j++;
//    }
//    type[j] = '\0';
//    j = 0;
//    res->Type = type;
//    while (!(isalpha(str[i]) || (('0' <= str[i]) && (str[i] <= '9')))) {
//        i++;
//    }
//    while (isalpha(str[i]) || (('0' <= str[i]) && (str[i] <= '9'))) {
//        tailNum[j] = str[i];
//        i++;
//        j++;
//    }
//    tailNum[j] = '\0';
//    j = 0;
//    res->TailNum = tailNum;
//    while (!(('0' <= str[i]) && (str[i] <= '9'))) {
//        i++;
//    }
//    while (('0' <= str[i]) && (str[i] <= '9')) {
//        i++;
//        t++;
//    }
//    i -= t;
//    while (('0' <= str[i]) && (str[i] <= '9')) {
//        i++;
//        t++;
//    }
//}
void FlightsResults(Flight **flights,int len) {
    std::cout << "\nPlanes currently flying\n";
    for (int i = 0;i < len;i++) {
        if (flights[i]->LandTime[0] == '-') {
            PrintFlightInfo(flights[i]);
        }
    }
    std::cout << "\nPlanes currently landing\n";
    for (int i = 0;i < len;i++) {
        if (flights[i]->LandTime[0] != '-') {
            PrintFlightInfo(flights[i]);
        }
    }
}
int main()
{
    setlocale(LC_ALL, "RUSSIAN");
    system("color f0");
    char* fileName;
    Flight** data = nullptr;
    std::cout << "Enter file name";
    fileName = String();
    FILE* file = fopen(fileName,"r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    int stringCount = StringCount(file);
    data = new Flight * [stringCount];
    char buff[256];
    int i = 0;
    while (fgets(buff, 255, file) != NULL) {
        data[i] = StringToFlight(buff);
        i++;
    }
    SortFlying(data, stringCount);
    SortLanding(data, stringCount);
    FlightsResults(data, stringCount);
    return 0;
}

