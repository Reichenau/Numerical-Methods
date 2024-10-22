#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

// -------------------- DEFINES --------------------
#define EPSILON 1e-15 // Заданная погрешность
#define MAX_ITERATIONS 1000 // Ограничение на максимально возможное количество итераций 
#define STEP 0.01 // Шаг для промежутка поиска корней

// -------------------- STRUCTURES --------------------
/**
 * Структура для хранения интервала [a, b], в котором функция меняет знак
 */
struct f_interval {
    double a, b;
};

// -------------------- FUNCTIONS --------------------

double f1(double x) {
    return exp(-pow(x, 2)) + 1 - x;
}

double df1(double x) {
    return -2 * x * exp(-pow(x, 2)) - 1;
}

double f2(double x) {
    return pow(x, 3) - 2 * pow(x, 2) - 4 * x - 7;
}

double df2(double x) {
    return 3 * pow(x, 2) - 4 * x - 4;
}

double f3(double x) {
    return pow(x, 3) - 2 * pow(x, 2) - 4 * x - 7 / x;
}

double df3(double x) {
    return 3 * pow(x, 2) - 4 * x - 4 + 7 / pow(x, 2);
}

// Автоматизация поиска промежутков [a,b]
// функция находит все интервалы, где функция f меняет знак, исключая интервалы, содержащие x=0
/*
 * Находит все интервалы [a, b], где функция f меняет знак
 * @param f Указатель на функцию
 * @param intervals Массив структур для хранения интервалов
 * @param count Указатель на переменную для хранения количества найденных интервалов
 */
void find_sign_changes(double (*f)(double), struct f_interval* intervals, int* count) {
    double a = -100.0; // Левая граница поиска
    double b = 100.0;  // Правая граница поиска
    *count = 0;         // Инициализация счетчика найденных интервалов

    // Итерация по промежуткам с шагом STEP для поиска изменения знака функции
    while (a < b) {
        double fa = f(a);
        double fb = f(a + STEP);

        // Проверяем, не содержит ли интервал x=0
        if ((a < 0 && (a + STEP) > 0)) {
            a += STEP;
            continue;
        }

        if (fa * fb < 0) {
            intervals[*count].a = a;
            intervals[*count].b = a + STEP;
            (*count)++;
            if (*count >= 1000) { 
                break;
            }
            a += STEP; 
        }
        else {
            a += STEP;
        }
    }
}

/*
 * Записывает погрешность в файл с указанием функции и метода
 * @param function_name Название функции (например, "f1")
 * @param method_name Название метода (например, "Newton")
 * @param error Погрешность
 * @param f_name Имя файла для записи погрешности
 */
void error_record(const char* function_name, const char* method_name, double error, const char* f_name) {
    FILE* f = fopen(f_name, "a");

    // Проверяем, удалось ли открыть файл
    if (f == NULL) {
        fprintf(stderr, "Ошибка: не удалось открыть файл %s для записи.\n", f_name);
        return;
    }

    // Записываем погрешность с указанием функции и метода
    fprintf(f, "%s:%s:%.15lf\n", function_name, method_name, error);

    fclose(f);
}

// Метод Ньютона 
/**
 * Находит максимально приближенное значение указанным методом Ньютона
 * @param f Указатель на функцию
 * @param df Указатель на производную исходной функции
 * @param iteration Указатель на переменную для хранения количества итераций
 * @param x0 Начальное приближение
 * @param function_name Название функции для записи погрешностей
 * @param epsilon Заданная погрешность
 * @param flag Флаг для определения типа записи погрешности (1 - итерационная, 0 - точность)
 * @return Приближенное значение корня функции
 */
double newton_method(double (*f)(double), double (*df)(double), int* iteration, double x0, const char* function_name, double epsilon, int flag) {
    double x1; // Следующее приближение
    double current_error; // Текущая погрешность
    int iter = 0; // Счетчик итераций
    char method_name[] = "Newton"; // Название метода

    // Итерационный процесс метода Ньютона
    do {
        if (df(x0) == 0) {
            printf("Производная близка к нулю. Метод Ньютона не может продолжаться.\n");
            return 0;
        }
        x1 = x0 - f(x0) / df(x0);
        // Запись погрешности
        current_error = fabs(x1 - x0);
        // Запись точной погрешности, если флаг не установлен
        if (flag == 1) {
            error_record(function_name, method_name, current_error, "iteration_error_analysis.txt");
        }
        x0 = x1;
        iter++;

    } while (current_error > epsilon && iter < MAX_ITERATIONS);
    *iteration = iter;
    // Запись точной погрешности, если флаг не установлен
    if (flag == 0) {
        error_record(function_name, method_name, current_error, "accuracy_error_analysis.txt");
    }
    return x1;
}

// Метод бисекции 
/**
 * Находит максимально приближенное значение указанным методом бисекции
 * @param f Указатель на функцию
 * @param a Левая граница интервала
 * @param b Правая граница интервала
 * @param iteration Указатель на переменную для хранения количества итераций
 * @param function_name Название функции для записи погрешностей
 * @param epsilon Заданная погрешность
 * @param flag Флаг для определения типа записи погрешности (1 - итерационная, 0 - точность)
 * @return Приближенное значение корня функции или NAN в случае ошибки
 */
double bisection_method(double (*f)(double), double a, double b, int* iteration, const char* function_name, double epsilon, int flag) {
    double c; // Средняя точка интервала
    int iter = 0; // Счетчик итераций
    char method_name[] = "Bisection"; // Название метода
    double current_error; // Текущая погрешность
    double fa = f(a); // Значение функции в точке a
    double fb = f(b); // Значение функции в точке b
    double fc; // Значение функции в точке c

    // Проверка на правильность указанного интервала 
    if (fa * fb >= 0) {
        printf("Метод бисекции не применим для функции %s: f(a) и f(b) имеют одинаковые знаки.\n", function_name);
        return NAN;
    }
    // Итерационный процесс метода бисекции
    do {
        c = (a + b) / 2.0;
        fc = f(c);
        if (fa * fc < 0) {
            b = c;
            fb = fc;
        }
        else {
            a = c;
            fa = f(a);
        }
        iter++;
        // Запись итерационной погрешности, если флаг установлен
        current_error = fabs(b - a);
        if (flag == 1) {
            error_record(function_name, method_name, current_error, "iteration_error_analysis.txt");
        }

    } while (current_error > epsilon && iter < MAX_ITERATIONS);
    // Проверка на достижение максимального количества итераций
    if (iter >= MAX_ITERATIONS) {
        printf("Метод бисекции для функции %s не сошелся за %d итераций.\n", function_name, MAX_ITERATIONS);
        return NAN;
    }
    *iteration = iter;

    // Запись точной погрешности, если флаг не установлен
    if (flag == 0) {
        error_record(function_name, method_name, current_error, "accuracy_error_analysis.txt");
    }
    return c;
}

// -------------------- MAIN --------------------
int main() {
    setlocale(LC_ALL, "Russian");

    // Очищаем файлы error_analysis перед началом записи
    FILE* f_error_clear = fopen("iteration_error_analysis.txt", "w");
    FILE* f_accuracy_clear = fopen("accuracy_error_analysis.txt", "w");

    // Проверка на успешное открытие и очистку файла iteration_error_analysis.txt
    if (f_error_clear != NULL) {
        fclose(f_error_clear);
    }
    else {
        fprintf(stderr, "Ошибка: не удалось очистить файл iteration_error_analysis.txt\n");
        return 1;
    }

    // Проверка на успешное открытие и очистку файла accuracy_error_analysis.txt
    if (f_accuracy_clear != NULL) {
        fclose(f_accuracy_clear);
    }
    else {
        fprintf(stderr, "Ошибка: не удалось очистить файл accuracy_error_analysis.txt\n");
        return 1;
    }

    // Поиск интервалов для уравнений
    struct f_interval f1_interval, f2_interval;
    struct f_interval f3_intervals[1000]; // Массив для хранения интервалов f3
    int f3_count = 0; // Количество найденных интервалов для f3

    // Для f1 и f2 используем оригинальную функцию, которая находит только один интервал
    find_sign_changes(f1, &f1_interval, &(int){1}); 
    find_sign_changes(f2, &f2_interval, &(int){1}); 

    // Для f3 находим все интервалы
    find_sign_changes(f3, f3_intervals, &f3_count); 

    // Объявление счетчиков количества итераций для каждого метода и функции
    int iteration_newton_f1, iteration_newton_f2, iteration_newton_f3;
    int iteration_bisection_f1, iteration_bisection_f2, iteration_bisection_f3;

    // Применение метода Ньютона к каждой функции
    double root_newton_f1 = newton_method(f1, df1, &iteration_newton_f1, f1_interval.a, "f1", EPSILON, 1);
    double root_newton_f2 = newton_method(f2, df2, &iteration_newton_f2, f2_interval.a, "f2", EPSILON, 1);

    // Для f3 необходимо пройтись по всем найденным интервалам
    double root_newton_f3[1000];
    char f3_label_newton[10];
    for (int i = 0; i < f3_count; i++) {
        // Генерация метки f3_1, f3_2 и т.д.
        snprintf(f3_label_newton, sizeof(f3_label_newton), "f3_%d", i + 1);
        root_newton_f3[i] = newton_method(f3, df3, &iteration_newton_f3, f3_intervals[i].a, f3_label_newton, EPSILON, 1);
    }

    // Применение метода бисекции к каждой функции
    double root_bisetion_f1 = bisection_method(f1, f1_interval.a, f1_interval.b, &iteration_bisection_f1, "f1", EPSILON, 1);
    double root_bisetion_f2 = bisection_method(f2, f2_interval.a, f2_interval.b, &iteration_bisection_f2, "f2", EPSILON, 1);

    // Для f3 необходимо пройтись по всем найденным интервалам
    double root_bisetion_f3[1000];
    char f3_label_bisection[10];
    for (int i = 0; i < f3_count; i++) {
        // Генерация метки f3_1, f3_2 и т.д.
        snprintf(f3_label_bisection, sizeof(f3_label_bisection), "f3_%d", i + 1);
        root_bisetion_f3[i] = bisection_method(f3, f3_intervals[i].a, f3_intervals[i].b, &iteration_bisection_f3, f3_label_bisection, EPSILON, 1);
    }

    // Вывод информации для пользователя о результатах метода Ньютона
    printf("-------------------- метод Ньютона --------------------\n");
    printf("f1(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", root_newton_f1, iteration_newton_f1, f1_interval.a, f1_interval.b);
    printf("f2(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", root_newton_f2, iteration_newton_f2, f2_interval.a, f2_interval.b);

    // Вывод результатов для каждого интервала f3
    for (int i = 0; i < f3_count; i++) {
        printf("f3_%d(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", i + 1, root_newton_f3[i], iteration_newton_f3, f3_intervals[i].a, f3_intervals[i].b);
    }

    printf("\n");

    // Вывод информации для пользователя о результатах метода бисекции
    printf("-------------------- метод бисекции --------------------\n");
    printf("f1(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", root_bisetion_f1, iteration_bisection_f1, f1_interval.a, f1_interval.b);
    printf("f2(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", root_bisetion_f2, iteration_bisection_f2, f2_interval.a, f2_interval.b);

    // Вывод результатов для каждого интервала f3
    for (int i = 0; i < f3_count; i++) {
        printf("f3_%d(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", i + 1, root_bisetion_f3[i], iteration_bisection_f3, f3_intervals[i].a, f3_intervals[i].b);
    }

    printf("\nПогрешности записаны в файл accuracy_error_analysis.txt\n");

    // Переменная для текущей погрешности
    double epsilon_val;

    // Цикл по различным значениям погрешности от 1e-1 до 1e-15
    for (int i = -1; i >= -15; i--) {
        epsilon_val = pow(10, i); // Вычисление текущей погрешности

        // Применение метода Ньютона с текущей погрешностью
        newton_method(f1, df1, &iteration_newton_f1, f1_interval.a, "f1", epsilon_val, 0);
        newton_method(f2, df2, &iteration_newton_f2, f2_interval.a, "f2", epsilon_val, 0);

        // Для f3 необходимо пройтись по всем найденным интервалам
        for (int i_f3 = 0; i_f3 < f3_count; i_f3++) {
            // Генерация метки f3_1, f3_2 и т.д.
            snprintf(f3_label_newton, sizeof(f3_label_newton), "f3_%d", i_f3 + 1);
            newton_method(f3, df3, &iteration_newton_f3, f3_intervals[i_f3].a, f3_label_newton, epsilon_val, 0);
        }

        // Применение метода бисекции с текущей погрешностью
        bisection_method(f1, f1_interval.a, f1_interval.b, &iteration_bisection_f1, "f1", epsilon_val, 0);
        bisection_method(f2, f2_interval.a, f2_interval.b, &iteration_bisection_f2, "f2", epsilon_val, 0);

        // Для f3 необходимо пройтись по всем найденным интервалам
        for (int i_f3 = 0; i_f3 < f3_count; i_f3++) {
            // Генерация метки f3_1, f3_2 и т.д.
            snprintf(f3_label_bisection, sizeof(f3_label_bisection), "f3_%d", i_f3 + 1);
            bisection_method(f3, f3_intervals[i_f3].a, f3_intervals[i_f3].b, &iteration_bisection_f3, f3_label_bisection, epsilon_val, 0);
        }
    }
    return 0;
}
