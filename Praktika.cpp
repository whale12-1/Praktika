
#define _CRT_SECURE_NO_WARNINGS
/***************************************************************************
* Project Type: win32_console_Application                                  *
* Project Name: C:\Users\Lenovo\source\repos\Praktika\Praktika.cpp         *
* File Name: Practica.cpp                                                  *
* Language: C++, Microsoft  Visual Studio 2026                             *
* Programmers: Горбунов Родион Вячеславович                                *
* Modified by:                                                             *
* Created: 07.05.2026                                                      *
* Last Revision: 26.05.2026                                                *
* Comment:                                                                 *
* Тема:«Сортировка пользовательских структур»                              *
* Вариант: 2 (летящие – по номеру рейса, посадка – по времени)            *
***************************************************************************/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cstring>
#include <cctype>

const char* filenames[] = {
    "Test1.txt", "Test2.txt", "Test3.txt", "Test4.txt", "Test5.txt",
    "Test6.txt", "Test7.txt", "Test8.txt", "Test9.txt", "Test10.txt",
    "Bad1.txt"
};
const int FILE_COUNT = sizeof(filenames) / sizeof(filenames[0]);

struct Flight {
    char* Type;
    char* TailNum;
    char* FlightNum;
    char* LandTime;
    bool tailValid;     // корректность бортового номера
    bool flightValid;   // корректность номера рейса
    bool timeValid;     // корректность времени посадки
};

struct ErrorRecord {
    int lineNumber;
    char message[256];
};

// Прототипы функций
int StringCount(FILE* file);
void PrintFlightInfo(const Flight* flight);
int StrLen(const char* str);
char* StrCopy(const char* w);
int StringToInt(const char* str);
int FindFlightNum(const char* flightNum);
bool BiggerTime(const char* t1, const char* t2);
void SortFlying(Flight** base, int len);
void SortLanding(Flight** base, int len);
Flight* StringToFlight(char* str, int lineNum, ErrorRecord* errors, int& errCount);
void FlightsResults(Flight** flights, int len, ErrorRecord* errors, int errCount);
void FreeFlight(Flight* f);
bool ValidateTailNum(const char* tail, char* errMsg);
bool ValidateFlightNum(const char* flightNum, char* errMsg);
bool ValidateLandTime(const char* landTime, char* errMsg);
void AddError(ErrorRecord* errors, int& errCount, int line, const char* msg);
void ProcessFile(const char* filename);
void StrCopyToBuffer(char* dest, const char* src, int destSize);
void SkipRestOfLine(FILE* file);


int StringCount(FILE* file) {
    fseek(file, 0, SEEK_SET);
    int count = 0;
    int ch;
    int lastChar = '\n';
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') count++;
        lastChar = ch;
    }
    if (lastChar != '\n' && lastChar != EOF) {
        count++;
    }
    rewind(file);
    return count;
}


void PrintFlightInfo(const Flight* flight) {
    std::cout << "Type: " << flight->Type << "\t"
        << "Tail number : " << flight->TailNum << "\t"
        << "Flight number: " << flight->FlightNum << "\t"
        << "Landing Time : " << flight->LandTime << "\n";
}


int StrLen(const char* str) {
    int i = 0;
    while (str[i] != '\0') i++;
    return i;
}


char* StrCopy(const char* w) {
    int len = StrLen(w);
    char* h = new char[len + 1];
    for (int i = 0; i < len; i++) h[i] = w[i];
    h[len] = '\0';
    return h;
}

int StringToInt(const char* str) {
    int res = 0;
    int i = 0;
    if (str[0] == '-') i++;
    for (; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9')
            res = res * 10 + (str[i] - '0');
        else
            break;
    }
    return (str[0] == '-') ? -res : res;
}


int FindFlightNum(const char* flightNum) {
    int i = 0;
    while (flightNum[i] != '\0' && !isdigit((unsigned char)flightNum[i])) {
        i++;
    }
    if (flightNum[i] == '\0') return -1;
    return StringToInt(&flightNum[i]);
}


bool BiggerTime(const char* t1, const char* t2) {
    int hours1 = 0, minutes1 = 0, hours2 = 0, minutes2 = 0;
    sscanf(t1, "%d:%d", &hours1, &minutes1);
    sscanf(t2, "%d:%d", &hours2, &minutes2);
    return (hours1 > hours2) || (hours1 == hours2 && minutes1 > minutes2);
}


void SortFlying(Flight** base, int len) {
    int flyingCount = 0;
    // Сначала найдём количество летящих
    for (int i = 0; i < len; i++) {
        if (base[i]->LandTime[0] == '-' && base[i]->flightValid)
            flyingCount++;
    }
    if (flyingCount == 0) return;

    for (int i = 0; i < flyingCount; i++) {
        for (int j = i + 1; j < flyingCount; j++) {
            int numI = FindFlightNum(base[i]->FlightNum);
            int numJ = FindFlightNum(base[j]->FlightNum);
            if (numI > numJ) {
                Flight* tmp = base[i];
                base[i] = base[j];
                base[j] = tmp;
            }
        }
    }
}


void SortLanding(Flight** base, int len) {
    int landingCount = 0;
    // Найдём количество садящихся
    for (int i = 0; i < len; i++) {
        if (base[i]->LandTime[0] != '-' && base[i]->timeValid)
            landingCount++;
    }
    if (landingCount == 0) return;

    // Пузырьковая сортировка по времени (раньше -> меньше)
    for (int i = 0; i < landingCount; i++) {
        for (int j = i + 1; j < landingCount; j++) {
            if (BiggerTime(base[i]->LandTime, base[j]->LandTime)) {
                Flight* tmp = base[i];
                base[i] = base[j];
                base[j] = tmp;
            }
        }
    }
}


void StrCopyToBuffer(char* dest, const char* src, int destSize) {
    int i = 0;
    while (src[i] != '\0' && i < destSize - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

bool ValidateTailNum(const char* tail, char* errMsg) {
    const char* dash = strchr(tail, '-');
    if (!dash) {
        sprintf(errMsg, "отсутствует дефис в формате Б-XXXX");
        return false;
    }
    if (*(dash + 1) == '\0') {
        sprintf(errMsg, "отсутствуют цифры после дефиса");
        return false;
    }
    dash++;
    // Проверяем только то, что ПОСЛЕ дефиса (там гарантированно однобайтовые цифры ASCII)
    while (*dash) {
        if (!isdigit((unsigned char)*dash)) {
            sprintf(errMsg, "после дефиса должны быть только цифры");
            return false;
        }
        dash++;
    }
    return true;
}

bool ValidateFlightNum(const char* flightNum, char* errMsg) {
    int len = StrLen(flightNum);
    if (len == 0) {
        sprintf(errMsg, "Empty number");
        return false;
    }
    bool hasDigit = false;
    for (int i = 0; i < len; i++) {
        if (isdigit((unsigned char)flightNum[i])) {
            hasDigit = true;
            break;
        }
    }
    if (!hasDigit) {
        sprintf(errMsg, "Incorrect letters in flight number");
        return false;
    }
    return true;
}


bool ValidateLandTime(const char* landTime, char* errMsg) {
    if (StringToInt(landTime) < 0)
        return true;  // корректный статус "летит"
    int h, m;
    if (sscanf(landTime, "%d:%d", &h, &m) != 2) {
        sprintf(errMsg, "Data is not in h:m format");
        return false;
    }
    if (h < 0 || h > 23 || m < 0 || m > 59) {
        sprintf(errMsg, "Incorect hours (0-23) or minutes (0-59)");
        return false;
    }
    return true;
}


void AddError(ErrorRecord* errors, int& errCount, int line, const char* msg) {
    if (errCount >= 100) return; // массив фиксированного размера
    errors[errCount].lineNumber = line;
    StrCopyToBuffer(errors[errCount].message, msg, 256);
    errCount++;
}


void SkipRestOfLine(FILE* file) {
    int ch;
    while ((ch = fgetc(file)) != '\n' && ch != EOF);
}


Flight* StringToFlight(char* str, int lineNum, ErrorRecord* errors, int& errCount) {
    char t[64] = { 0 }, ta[64] = { 0 }, f[64] = { 0 }, l[64] = { 0 }, extra[64] = { 0 };
    int u = sscanf(str, "%63s %63s %63s %63s %63s", t, ta, f, l, extra);

    Flight* res = new Flight();
    res->Type = nullptr;
    res->TailNum = nullptr;
    res->FlightNum = nullptr;
    res->LandTime = nullptr;
    res->tailValid = true;
    res->flightValid = true;
    res->timeValid = true;

    if (u != 4) {
        char msg[256];
        sprintf(msg, "Incorrect amount of data (found %d, needed 4)", (u > 4) ? 5 : u);
        AddError(errors, errCount, lineNum, msg);
        res->Type = StrCopy("?");
        res->TailNum = StrCopy("?");
        res->FlightNum = StrCopy("0000");
        res->LandTime = StrCopy("-");
        // Все флаги валидности сбрасываем, чтобы запись не участвовала в сортировке
        res->tailValid = false;
        res->flightValid = false;
        res->timeValid = false;
        return res;
    }

    res->Type = StrCopy(t);
    res->TailNum = StrCopy(ta);
    res->FlightNum = StrCopy(f);
    res->LandTime = StrCopy(l);

    char errMsg[256];

    // Валидация бортового номера
    if (!ValidateTailNum(res->TailNum, errMsg)) {
        char full[300];
        sprintf(full, "Tail number '%s' – %s", res->TailNum, errMsg);
        AddError(errors, errCount, lineNum, full);
        res->tailValid = false;
    }

    // Валидация номера рейса
    if (!ValidateFlightNum(res->FlightNum, errMsg)) {
        char full[300];
        sprintf(full, "Flight number '%s' – %s", res->FlightNum, errMsg);
        AddError(errors, errCount, lineNum, full);
        res->flightValid = false;
    }

    // Валидация времени посадки
    if (!ValidateLandTime(res->LandTime, errMsg)) {
        char full[300];
        sprintf(full, "Landing time '%s' – %s", res->LandTime, errMsg);
        AddError(errors, errCount, lineNum, full);
        res->timeValid = false;
    }

    return res;
}


void FlightsResults(Flight** flights, int len, ErrorRecord* errors, int errCount) {
    if (errCount > 0) {
        std::cout << "\nErrors in data\n";
        for (int i = 0; i < errCount; i++)
            std::cout << "String " << errors[i].lineNumber << ": " << errors[i].message << "\n";
    }
    else {
        std::cout << "\nAll data is correct\n";
    }

    std::cout << "\nPlanes currently flying\n";
    for (int i = 0; i < len; i++) {
        if (flights[i]->LandTime[0] == '-') {
            PrintFlightInfo(flights[i]);
        }
    }

    std::cout << "\nPlanes currently landing\n";
    for (int i = 0; i < len; i++) {
        if (flights[i]->LandTime[0] != '-') {
            PrintFlightInfo(flights[i]);
        }
    }
}


void FreeFlight(Flight* f) {
    delete[] f->Type;
    delete[] f->TailNum;
    delete[] f->FlightNum;
    delete[] f->LandTime;
    delete f;
}


void ProcessFile(const char* filename) {
    std::cout << "\n========== Файл: " << filename << " ==========\n";
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Ошибка открытия файла");
        return;
    }

    int stringCount = StringCount(file);
    Flight** data = new Flight * [stringCount];

    // Массив для ошибок (до 100 записей, достаточно для тестов)
    ErrorRecord* errors = new ErrorRecord[100];
    int errorCount = 0;

    char buff[256];
    int idx = 0;
    int lineNum = 1;

    while (idx < stringCount && fgets(buff, 255, file) != NULL) {
        // Проверка, что строка целиком поместилась
        size_t len = strlen(buff);
        if (len > 0 && buff[len - 1] != '\n' && !feof(file)) {
            char msg[100];
            sprintf(msg, "строка превышает 255 символов (обрезана)");
            AddError(errors, errorCount, lineNum, msg);
            // Пропускаем остаток строки, чтобы не сбить нумерацию строк
            SkipRestOfLine(file);
        }

        // Удаляем символы перевода строки
        buff[strcspn(buff, "\r\n")] = '\0';
        data[idx] = StringToFlight(buff, lineNum, errors, errorCount);
        idx++;
        lineNum++;
    }

    fclose(file);

    // Если строк в файле оказалось больше, чем мы насчитали (из-за длинных строк),
    // то фактически прочитано idx строк, надо подстроить stringCount
    int actualCount = idx; // количество реально созданных структур

    // Сортировка: сначала переместим все валидные летящие в начало
    // (сортировка написана так, что она работает только с валидными записями)
    SortFlying(data, actualCount);
    SortLanding(data, actualCount);

    // Вывод результатов
    FlightsResults(data, actualCount, errors, errorCount);

    // Освобождение памяти
    for (int i = 0; i < actualCount; i++) {
        FreeFlight(data[i]);
    }
    delete[] data;
    delete[] errors;
}
//int main() {
//    // Автоматическая настройка под UTF-8 локаль системы (идеально для Mac/Linux)
//    std::setlocale(LC_ALL, "");
//
//    // Синхронизируем стандартные потоки C++ с локалью
//    std::ios_base::sync_with_stdio(false);
//    std::cout.imbue(std::locale(""));
//
//    ProcessFile(filenames[0]);
//    return 0;
//}
int main() {
    std::setlocale(LC_ALL, "en_US.UTF-8");
    //setlocale(LC_ALL, "Russ");
    ProcessFile(filenames[FILE_COUNT - 1]);
    return 0;
}
