# Simple Vector (Упрощенный аналог std::vector)

## Описание
Реализация шаблонного динамического массива на C++ с поддержкой основных операций стандартного вектора. Проект разработан для глубокого понимания работы контейнеров STL и механизмов управления памятью в C++.

## Ключевые особенности
Полная поддержка move-семантики

Оптимизированные операции вставки/удаления

Поддержка initializer_list для инициализации

Строгая гарантия безопасности исключений

Полный набор операторов сравнения

Поддержка reserve/resize

## Технологический стек
C++17 (шаблоны, move-семантика, initializer_list)


## Системные требования
Компилятор с поддержкой C++17 (GCC 9+, Clang 10+, MSVC 19.28+)

## Тестирование
Проект включает набор unit-тестов, проверяющих:

Корректность работы с памятью

Безопасность исключений

Move-семантику

Граничные случаи (пустой вектор, операции на границах)

## Особенности реализации
Оптимизация памяти - удвоение емкости при необходимости расширения

Итераторы - полная поддержка random access iterator концепции

Безопасность исключений - строгие гарантии для критических операций

Move-семантика - эффективная передача владения ресурсами

## Пример использования

#include "simple_vector.h"

int main() {

    SimpleVector<std::string> words;
    
    words.PushBack("Hello");
    
    words.PushBack("world");
    
    for (const auto& word : words) {
    
        std::cout << word << " ";
    
    }
    
    // Output: Hello world
    
    return 0;

}

## Производительность
Ключевые операции:

PushBack: O(1) амортизированное время

Insert: O(n) в худшем случае

Erase: O(n) в худшем случае

Доступ по индексу: O(1)
