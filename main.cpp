 #include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

struct Video {                         // Definicja struktury Video
    int number = -1;                   // Numer filmu, domyœlnie -1
    std::string title = "wrong";       // Tytu³ filmu, domyœlnie "wrong"
    float rating = -1.0;               // Ocena filmu, domyœlnie -1.0
};

class Dynamic_array {                 // Definicja klasy Dynamic_array
    Video* _array = nullptr;          // WskaŸnik na tablicê dynamiczn¹ elementów typu Video, zainicjowany jako nullptr
    int _capacity = 1;                 // Pojemnoœæ tablicy dynamicznej, pocz¹tkowo 1
    int _size = 0;                     // Aktualny rozmiar tablicy dynamicznej, pocz¹tkowo 0

public:
    Dynamic_array();                  // Deklaracja konstruktora domyœlnego
    Dynamic_array(int capacity, Dynamic_array* arr);  // Deklaracja konstruktora z argumentami
    ~Dynamic_array();                 // Deklaracja destruktora

private:
    // Funkcje pomocnicze
    void grow_array();                // Deklaracja funkcji zwiêkszaj¹cej rozmiar tablicy dynamicznej
    void prepare_data();              // Deklaracja funkcji wczytuj¹cej dane z pliku do tablicy dynamicznej
    bool right_sorted(Video* arr);    // Deklaracja funkcji sprawdzaj¹cej, czy tablica jest posortowana
    double calc_median(Video* arr);   // Deklaracja funkcji obliczaj¹cej medianê
    double calc_mean(Video* arr);     // Deklaracja funkcji obliczaj¹cej œredni¹ arytmetyczn¹
    void time_measure();              // Deklaracja funkcji mierz¹cej czas sortowania

    // Algorytmy sortowania
    void quick_sort(Video* arr, int const start, int const end);   // Deklaracja funkcji sortowania szybkiego (quicksort)
    void merge(Video* arr, int const start, int const mid, int const end);  // Deklaracja funkcji scalaj¹cej (mergesort)
    void merge_sort(Video* arr, int const start, int const end);   // Deklaracja funkcji sortowania przez scalanie (mergesort)
    void heapify(Video* arr, int const n, int const i);            // Deklaracja funkcji naprawy kopca (heapsort)
    void heap_sort(Video* arr, int const start, int const end);    // Deklaracja funkcji sortowania przez kopcowanie (heapsort)
    void insertion_sort(Video* arr, int const start, int const end);  // Deklaracja funkcji sortowania przez wstawianie (insertion sort)
    void intro_sort(Video* arr, int const start, int const end, int const max_depth);  // Deklaracja funkcji sortowania introspkective (introsort)
};

Dynamic_array::Dynamic_array() {
    _array = new Video[_capacity];    // Tworzenie nowej instancji dynamicznej tablicy Video
    prepare_data();                   // Wczytywanie danych z pliku do tablicy
    std::cout << "Done importing\n";  // Komunikat wyœwietlany po zakoñczeniu wczytywania danych
}

Dynamic_array::Dynamic_array(int capacity, Dynamic_array* arr) : _capacity(capacity) {
    // Zapewnienie, ¿e pojemnoœæ tablicy nie bêdzie wiêksza ni¿ rozmiar tablicy Ÿród³owej
    if (_capacity > arr->_size) {
        _capacity = arr->_size;
    }

    _array = new Video[_capacity];    // Tworzenie nowej instancji dynamicznej tablicy Video
    for (int i = 0; i < _capacity; i++) {
        if (_size == _capacity) {
            grow_array();             // Zwiêkszanie rozmiaru tablicy, jeœli jest to konieczne
        }
        this->_array[i] = arr->_array[i];  // Kopiowanie elementów z tablicy Ÿród³owej do nowej tablicy
        _size++;                    // Zwiêkszanie rozmiaru nowej tablicy
    }

    time_measure();                 // Pomiar czasu sortowania
}


Dynamic_array::~Dynamic_array() {   // Destruktor klasy Dynamic_array
    delete[] _array;                // Usuwanie dynamicznie zaalokowanej tablicy _array
}

void Dynamic_array::grow_array() {  // Funkcja zwiêkszaj¹ca rozmiar tablicy dynamicznej
    _capacity = _capacity * 2;       // Podwajanie pojemnoœci tablicy
    Video* temporary_array = new Video[_capacity];  // Tworzenie tymczasowej tablicy o nowej pojemnoœci

    for (int i = 0; i < _size; i++) {  // Przenoszenie elementów ze starej tablicy do nowej
        temporary_array[i] = _array[i];
    }

    delete[] _array;                 // Usuwanie starej tablicy
    _array = temporary_array;        // Przypisanie nowej tablicy do wskaŸnika _array
}

void Dynamic_array::prepare_data() {  // Funkcja wczytuj¹ca dane z pliku do tablicy dynamicznej
    std::ifstream database;           // Deklaracja obiektu strumienia pliku
    database.open("PAA.projekt1.dane.csv");  // Otwarcie pliku
    if (database.is_open()) {
        std::cout << "File opened\n";
    }
    else {
        std::cout << "Could not find the file\n";
        return;
    }

    std::string line;
    bool first_row = true;
    while (std::getline(database, line)) {
        // skiping the first line without data
        if (first_row) {
            first_row = false;
            continue;
        }
        std::istringstream iss(line);
        std::string token;
        int j = 0; // ensuring end parsing
        Video temporary;
        bool has_comma = false;
        while (std::getline(iss, token, ',')) {
            switch (j) {
            case 0:
                temporary.number = std::stoi(token);
                j++;
                break;
            case 1:
                if (token[0] == '"') {
                    temporary.title = token;
                    has_comma = true;
                    break;
                }
                if (has_comma) {
                    temporary.title += token;
                    if (token[token.size() - 1] == '"') {
                        j++;
                    }
                    break;
                }
                else {
                    temporary.title = token;
                    j++;
                    break;
                }
            case 2:
                temporary.rating = std::stof(token);
                j++;
                break;
            default:
                break;
            }
        }
        // filtering the _array
        if (!(temporary.rating == -1 || temporary.number == -1 ||
            temporary.title == "wrong")) {
            if (_size == _capacity) {
                grow_array();
            }
            _array[_size] = temporary;
            _size++;
        }
    }
    database.close();
}


bool Dynamic_array::right_sorted(Video* arr) {
    for (int i = _size - 1; i > 0; i--) {  // Iteracja od ostatniego elementu do drugiego
        if (!(arr[i].rating >= arr[i - 1].rating)) {  // Sprawdzenie, czy elementy s¹ posortowane w kolejnoœci malej¹cej
            return false;  // Zwrócenie fa³szu, jeœli wyst¹pi³ b³¹d w kolejnoœci
        }
    }
    return true;  // Zwrócenie prawdy, jeœli wszystkie elementy s¹ posortowane poprawnie
}

double Dynamic_array::calc_median(Video* arr) {
    int const center = _size / 2;  // Obliczenie indeksu œrodkowego
    if (_size % 2 == 0) {  // Sprawdzenie, czy liczba elementów w tablicy jest parzysta
        return (arr[center].rating + arr[center + 1].rating) / 2;  // Obliczenie mediany dla tablicy o parzystej liczbie elementów
    }
    else {
        return arr[center + 1].rating;  // Zwrócenie oceny œrodkowego elementu dla tablicy o nieparzystej liczbie elementów
    }
}

double Dynamic_array::calc_mean(Video* arr) {
    double sum = 0;  // Zmienna przechowuj¹ca sumê ocen
    for (int i = 0; i < _size; i++) {  // Iteracja po wszystkich elementach tablicy
        sum += arr[i].rating;  // Dodanie oceny aktualnego elementu do sumy
    }
    return sum / _size;  // Obliczenie œredniej arytmetycznej ocen
}

void Dynamic_array::time_measure() {
    using namespace std::chrono;  // U¿ycie przestrzeni nazw std::chrono do korzystania z mechanizmu pomiaru czasu

    for (int i = 0; i < 3; i++) {  // Pêtla iteruj¹ca przez trzy algorytmy sortowania
        Video* copy_for_sort = new Video[_size];  // Utworzenie kopii tablicy do sortowania

        // Skopiowanie danych do tablicy kopii
        for (int k = 0; k < _size; k++) {
            copy_for_sort[k] = _array[k];
        }

        switch (i) {  // Wybór algorytmu sortowania na podstawie indeksu
        case 0: {  // Sortowanie szybkie
            steady_clock::time_point begin = steady_clock::now();  // Pocz¹tek pomiaru czasu
            quick_sort(copy_for_sort, 0, _size - 1);  // Wywo³anie algorytmu sortowania szybkiego
            steady_clock::time_point end = steady_clock::now();  // Koniec pomiaru czasu
            auto duration = duration_cast<milliseconds>(end - begin).count();  // Obliczenie czasu trwania sortowania w milisekundach
            std::cout << "Finished sorting array of size " << _size
                << " in: " << duration << " ms\n";  // Wyœwietlenie czasu trwania sortowania
            if (right_sorted(copy_for_sort)) {  // Sprawdzenie, czy tablica zosta³a posortowana prawid³owo
                std::cout << "The array was sorted correctly\n";  // Wyœwietlenie informacji o poprawnym posortowaniu
            }
            break;
        }
        case 1: {  // Sortowanie przez scalanie
            steady_clock::time_point begin = steady_clock::now();  // Pocz¹tek pomiaru czasu
            merge_sort(copy_for_sort, 0, _size - 1);  // Wywo³anie algorytmu sortowania przez scalanie
            steady_clock::time_point end = steady_clock::now();  // Koniec pomiaru czasu
            auto duration = duration_cast<milliseconds>(end - begin).count();  // Obliczenie czasu trwania sortowania w milisekundach
            std::cout << "Finished sorting array of size " << _size
                << " in: " << duration << " ms\n";  // Wyœwietlenie czasu trwania sortowania
            if (right_sorted(copy_for_sort)) {  // Sprawdzenie, czy tablica zosta³a posortowana prawid³owo
                std::cout << "The array was sorted correctly\n";  // Wyœwietlenie informacji o poprawnym posortowaniu
            }
            break;
        }
        case 2: {  // Sortowanie introspkective
            int max_depth = 2 * std::log2(_size);  // Obliczenie maksymalnej g³êbokoœci rekursji dla sortowania introspkective
            steady_clock::time_point begin = steady_clock::now();  // Pocz¹tek pomiaru czasu
            intro_sort(copy_for_sort, 0, _size - 1, max_depth);  // Wywo³anie algorytmu sortowania introspkective
            steady_clock::time_point end = steady_clock::now();  // Koniec pomiaru czasu
            auto duration = duration_cast<milliseconds>(end - begin).count();  // Obliczenie czasu trwania sortowania w milisekundach
            std::cout << "Finished sorting array of size " << _size
                << " in: " << duration << " ms\n";  // Wyœwietlenie czasu trwania sortowania
            if (right_sorted(copy_for_sort)) {  // Sprawdzenie, czy tablica zosta³a posortowana prawid³owo
                std::cout << "The array was sorted correctly\n";  // Wyœwietlenie informacji o poprawnym posortowaniu
            }
            break;
        }
        }

        if (i == 0) {  // Dla pierwszego algorytmu sortowania
            std::cout << "The median of this dataset is " << calc_median(copy_for_sort)
                << std::endl;  // Obliczenie i wyœwietlenie mediany danych
            std::cout << "The arithmetic mean of this dataset is "
                << calc_mean(copy_for_sort) << std::endl;  // Obliczenie i wyœwietlenie œredniej arytmetycznej danych
        }
        delete[] copy_for_sort;  // Zwolnienie pamiêci po tablicy kopii
    }
}


//--------------------------------------------------Sorting

void Dynamic_array::quick_sort(Video* arr, int const start, int const end) {
    // base case
    if (start >= end)
        return;

    int const p = arr[(start + end) / 2].rating;
    int i = start;
    int j = end;

    while (i <= j) {
        while (arr[i].rating < p) {
            i++;
        }

        while (arr[j].rating > p) {
            j--;
        }

        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    // _sorting the start part
    quick_sort(arr, start, j);
    // _sorting the end part
    quick_sort(arr, i, end);
}

void Dynamic_array::merge(Video* arr, int const start, int const mid,
    int const end) {
    int const sub_array_first = mid - start + 1;
    int const sub_array_second = end - mid;

    // Create temp arrays
    Video* start_array = new Video[sub_array_first],
        * end_array = new Video[sub_array_second];

    // Copy data to temp arrays start_array[]
    // and end_array[]
    for (int i = 0; i < sub_array_first; i++) {
        start_array[i] = arr[start + i];
    }
    for (int i = 0; i < sub_array_second; i++) {
        end_array[i] = arr[mid + 1 + i];
    }

    // Initial index of first sub-array
    // Initial index of second sub-array
    int index_sub_array_first = 0, index_sub_array_second = 0;

    // Initial index of merged array
    int index_merged_array = start;

    // Merge the temp arrays back into
    // array[start..end]
    while (index_sub_array_first < sub_array_first && index_sub_array_second < sub_array_second) {
        if (start_array[index_sub_array_first].rating <=
            end_array[index_sub_array_second].rating) {
            arr[index_merged_array] = start_array[index_sub_array_first];
            index_sub_array_first++;
        }
        else {
            arr[index_merged_array] = end_array[index_sub_array_second];
            index_sub_array_second++;
        }
        index_merged_array++;
    }

    // Copy the remaining elements of
    // start array, if there are any
    while (index_sub_array_first < sub_array_first) {
        arr[index_merged_array] = start_array[index_sub_array_first];
        index_sub_array_first++;
        index_merged_array++;
    }

    // the same for the end arra
    while (index_sub_array_second < sub_array_second) {
        arr[index_merged_array] = end_array[index_sub_array_second];
        index_sub_array_second++;
        index_merged_array++;
    }
}

void Dynamic_array::merge_sort(Video* arr, int const start, int const end) {
    // Returns recursively
    if (start >= end)
        return;

    int const mid = start + (end - start) / 2;
    merge_sort(arr, start, mid);
    merge_sort(arr, mid + 1, end);
    merge(arr, start, mid, end);
}

void Dynamic_array::heapify(Video* arr, int const n, int const i) {
    int largest = i;
    int start = 2 * i + 1;
    int end = 2 * i + 2;

    if (start < n && arr[start].rating > arr[largest].rating)
        largest = start;

    if (end < n && arr[end].rating > arr[largest].rating)
        largest = end;

    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void Dynamic_array::heap_sort(Video* arr, int const start, int const end) {
    int const n = end - start + 1;
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i + start);
    }

    for (int i = n - 1; i > 0; i--) {
        std::swap(arr[start], arr[start + i]);
        heapify(arr, i, start);
    }
}

void Dynamic_array::insertion_sort(Video* arr, int const start, int const end) {
    for (int i = start + 1; i <= end; ++i) {
        Video key = arr[i];
        int j = i - 1;

        while (j >= start && arr[j].rating > key.rating) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void Dynamic_array::intro_sort(Video* arr, int const start, int const end,
    int const max_depth) {

    int const current_size = end - start;
    if (current_size < 16) {
        insertion_sort(arr, start, end);
        return;
    }
    if (max_depth == 0) {
        heap_sort(arr, start, end);
        return;
    }

    int const p = arr[(start + end) / 2].rating;
    int i = start;
    int j = end;

    while (i <= j) {
        while (arr[i].rating < p) {
            i++;
        }

        while (arr[j].rating > p) {
            j--;
        }

        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    // _sorting the start part
    intro_sort(arr, start, j, max_depth - 1);
    // _sorting the end part
    intro_sort(arr, i, end, max_depth - 1);
    return;
}

int main() {
    Dynamic_array table_of_everything;
    Dynamic_array ten(10000, &table_of_everything);
    Dynamic_array hundred(100000, &table_of_everything);
    Dynamic_array five_hundred(500000, &table_of_everything);
    Dynamic_array million(1000000, &table_of_everything);

    return 0;
}