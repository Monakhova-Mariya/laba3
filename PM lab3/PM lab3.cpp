#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <unordered_set>

/*!
    \brief Класс, задающий абитуриента
*/
struct enrollee
{
public:
    std::string name; ///< ФИО абитуриента
    std::string faculty; ///< Факультет
    std::string speciality; ///< Специальность
    int result; ///< Количество набранных баллов


    /*!
        \brief Конструктор абитуриента
    */
    enrollee(std::string name = "", std::string faculty = "", std::string speciality = "", int result = 0)
    {
        this->name = name;
        this->faculty = faculty;
        this->speciality = speciality;
        this->result = result;
    }

    /*!
        Оператор сравнения равно
        \param[in] right правое сравниваемое значение
        \return Возвращает true, если левый объект равен, false - иначе
    */
    bool operator==(const enrollee& right) const
    {
        return this->name == right.name && this->faculty == right.faculty && this->speciality == right.speciality && this->result == right.result;
    }
};


/*!
    \brief Класс, задающий первый хэш
*/
class hash1
{
public:

    /*!
        Оператор взятия хэша от объекта
        \param[in] enrollee ссылка на объект, от которого берется хэш
        \return Возвращает хэш объекта
    */
    size_t operator()(const enrollee& enrollee) const
    {
        unsigned int result = 0;

        for (int i = 0; i < enrollee.name.length(); i++)
        {
            result += (enrollee.name[i] * i) << (i % 4);
        }
        return result;
    }
};

/*!
    \brief Класс, задающий первый хэш
*/
class hash2
{
public:
    /*!
        Оператор взятия хэша от объекта
        \param[in] enrollee ссылка на объект, от которого берется хэш
        \return Возвращает хэш объекта
    */
    size_t operator()(const enrollee& enrollee) const
    {
        unsigned int result = 0;

        for (int i = 0; i < enrollee.name.length(); i++)
        {
            result += ((enrollee.name[i] * i) << (i % 4)) * i;
        }
        return result;
    }
};

int main()
{
    int sizes[10] = { 100, 1000, 5000, 10000, 20000, 30000, 40000, 50000, 75000, 100000 };

    for (int size : sizes)
    {
        std::unordered_set<enrollee, hash1> data1;
        std::unordered_set<enrollee, hash2> data2;
        enrollee* array = new enrollee[size];


        std::vector<size_t> hashes1;
        std::vector<size_t> hashes2;
        hash1 hasher1;
        hash2 hasher2;


        std::ifstream in;
        char filename[80];
        memset(filename, 0, 80);
        sprintf_s(filename, "%s%d%s", "data_", size, ".txt");

        in.open(filename);

        for (int i = 0; i < size; i++)
        {
            std::string name, faculty, speciality;
            int result;
            in >> name >> faculty >> speciality >> result;
            array[i] = enrollee(name, faculty, speciality, result);

            data1.insert(array[i]);
            data2.insert(array[i]);

            size_t hash1 = hasher1(array[i]);
            size_t hash2 = hasher2(array[i]);

            bool isHash1 = false;
            bool isHash2 = false;

            for (int j = 0; j < hashes1.size(); j++)
                if (hashes1[j] == hash1)
                {
                    isHash1 = true;
                    break;
                }

            for (int j = 0; j < hashes2.size(); j++)
                if (hashes2[j] == hash2)
                {
                    isHash2 = true;
                    break;
                }

            if (!isHash1)
                hashes1.push_back(hash1);

            if (!isHash2)
                hashes2.push_back(hash2);
        }

        int search_index = (int)(rand() / RAND_MAX * size);

        auto begin = std::chrono::high_resolution_clock::now();

        enrollee finded_1 = *data1.find(array[search_index]);

        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Search with first hash " << size << " elements " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << " ns" << std::endl;

        begin = std::chrono::high_resolution_clock::now();

        enrollee finded_2 = *data2.find(array[search_index]);

        end = std::chrono::high_resolution_clock::now();

        std::cout << "Search with second hash " << size << " elements " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << " ns" << std::endl;

        std::cout << "Collisions with first hash " << size - hashes1.size() << " on " << size << " elements" << std::endl;
        std::cout << "Collisions with second hash " << size - hashes2.size() << " on " << size << " elements" << std::endl;

        delete[] array;
    }
}
