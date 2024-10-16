#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#define EPSILON 1e-15 // Заданная погрешность
#define MAX_ITERATIONS 1000 // Ограничение на максимально возможное количество итераций 
#define STEP 1 // Шаг для промежутка поиска корней

struct f_interval {
    double a, b;
};

// -------------------- FUNCTIONS --------------------
// Исходная первая функция f1(x) = exp(-x^2)-x+1 [1,2]
double f1(double x) {
    return exp(-pow(x, 2)) + 1 - x;
}

// Производная первой функции df1(x) = -2xexp(-x^2)-1
double df1(double x) {
    return -2 * x * exp(-pow(x, 2)) - 1;
}

// Исходная вторая функция f2(x) = x^3-2x^2-4x-7 [3,4]
double f2(double x) {
    return pow(x, 3) - 2 * pow(x, 2) - 4 * x - 7;
}

// Производная второй функции df2(x) = 3x^2 - 4x - 4
double df2(double x) {
    return 3 * pow(x, 2) - 4 * x - 4;
}

// Исходная функция f3(x) = x^3-2x^2-4x-7/x (с разрывом)
double f3(double x) {
    return pow(x, 3) - 2 * pow(x, 2) - 4 * x - 7 / x;
}
// Производная третьей функции df3(x) = (3*x^4-4*x^3-4*x^2+7)/(x^2)
double df3(double x) {
    return 3 * pow(x, 2) - 4 * x - 4 + 7 / pow(x, 2);
}

// Нахождение интервала [a,b]
/*
 * Находит интервал [a, b], где функция f меняет знак
 * @param f Указатель на функцию
 * @param method Указатель на структуру для хранения интервала
 */
void find_sign_changes(double (*f)(double), struct f_interval* method) {
    double a = -100.0; // левая граница
    double b = 100.0; // правая граница

    while (a < b) {
        if (f(a) * f(a + STEP) < 0) {
            method->a = a;
            method->b = a + STEP;
            break;
        }
        a += STEP;
    }
}

/*
 * Записывает погрешность в файл с указанием функции и метода
 * @param function_name Название функции (например, "f1")
 * @param method_name Название метода (например, "Newton")
 * @param error Погрешность
 */
void error_record(const char* function_name, const char* method_name, double error) {
    FILE* f = fopen("errors.txt", "a");

    // Проверяем, удалось ли открыть файл
    if (f == NULL) {
        fprintf(stderr, "Ошибка: не удалось открыть файл errors.txt для записи.\n");
        return;
    }

    // Записываем погрешность с указанием функции и метода
    fprintf(f, "%s:%s:%.15lf\n", function_name, method_name, error);

    fclose(f);
}
void plot_function(double (*f)(double), double x_min, double x_max, double step, const char* f_name) {
    FILE* fp = fopen(f_name, "w+");
    if (f == NULL) {
        fprintf(stderr, "Не удалось открыть файл %s для записи.\n", f_name);
        return;
    }
    for (double x = x_min; x <= x_max; x += step) {
        fprintf(fp, "%lf %.15lf\n", x, f(x));
    }
    fclose(fp);
}
// -------------------- METHODS --------------------
// Метод Ньютона 
/**
 * Находит максимально приближенное значение указанным методом
 * @param f Указатель на функцию
 * @param df Указатель на производную исходной функции
 * @param iteration Указатель на переменную для хранения количества итераций
 * @param x0 Начальное приближение
 * @param function_name Название функции для записи погрешностей
 */
double newton_method(double (*f)(double), double (*df)(double), int* iteration, double x0, const char* function_name, double epsilon, int flag) {
    double x, x1;
    double current_error;
    int iter = 0;
    char method_name[] = "Newton";
    do {
        // Проверка на зануление производной 
        if (df(x0) == 0) {
            //printf("Производная близка к нулю. Метод Ньютона не может продолжаться.\n");
            return 0;
        }
        x1 = x0 - f(x0) / df(x0);
        // Запись погрешности
        current_error = fabs(x1 - x0);
        if (flag == 1) {
            error_record(function_name, method_name, current_error);
        }
        x0 = x1;

        iter++;
    } while (current_error > epsilon && iter < MAX_ITERATIONS);
    if (iteration) {
        *iteration = iter;
    }
    return x1;
}

// Метод бисекции 
/**
 * Находит максимально приближенное значение указанным методом
 * @param f Указатель на функцию
 * @param a Левая граница интервала
 * @param b Правая граница интервала
 * @param iteration Указатель на переменную для хранения количества итераций
 * @param function_name Название функции для записи погрешностей
 */
double bisection_method(double (*f)(double), double a, double b, int* iteration, const char* function_name, double epsilon, int flag) {
    double c;
    int iter = 0;
    char method_name[] = "Bisection";
    double current_error;
    double fa = f(a);
    double fb = f(b);
    double fc;
    // Проверка на правильность указанного интервала 
    if (fa * fb >= 0) {
        printf("Метод бисекции не применим для функции %s: f(a) и f(b) имеют одинаковые знаки.\n", function_name);
        return NAN;
    }
    do {
        c = (a + b) / 2.0;
        fc = f(c);
        if (fa * fc < 0) {
            b = c;
        }
        else {
            a = c;
        }
        iter++;
        // Запись погрешности
        current_error = fabs(b - a);
        if (flag == 1) {
            error_record(function_name, method_name, current_error);
        }
    } while (fabs(b - a) > epsilon && iter < MAX_ITERATIONS);
    if (iter >= MAX_ITERATIONS) {
        printf("Метод бисекции для функции %s не сошелся за %d итераций.\n", function_name, MAX_ITERATIONS);
        return NAN;
    }
    *iteration = iter;
    return c;
}

// -------------------- MAIN --------------------
int main() {
    setlocale(LC_ALL, "Russian");

    // Очищаем файл errors.txt перед началом записи
    FILE* f_clear = fopen("errors.txt", "w");
    if (f_clear != NULL) {
        fclose(f_clear);
    }
    else {
        fprintf(stderr, "Ошибка: не удалось очистить файл errors.txt.\n");
        return 1;
    }

    // Поиск интервалов для уравнений 
    struct f_interval f1_interval, f2_interval, f3_interval;

    find_sign_changes(f1, &f1_interval);
    find_sign_changes(f2, &f2_interval);
    find_sign_changes(f3, &f3_interval);

    // Счетчик количества итераций для каждого метода и функции 
    int iteration_newton_f1, iteration_newton_f2, iteration_newton_f3;
    int iteration_bisection_f1, iteration_bisection_f2, iteration_bisection_f3;

    // Применение методов Ньютона
    double root_newton_f1 = newton_method(f1, df1, &iteration_newton_f1, f1_interval.a, "f1", EPSILON, 1);
    double root_newton_f2 = newton_method(f2, df2, &iteration_newton_f2, f2_interval.a, "f2", EPSILON, 1);
    double root_newton_f3 = newton_method(f3, df3, &iteration_newton_f3, f3_interval.a, "f3", EPSILON, 1);

    // Применение методов бисекции
    double root_bisetion_f1 = bisection_method(f1, f1_interval.a, f1_interval.b, &iteration_bisection_f1, "f1", EPSILON, 1);
    double root_bisetion_f2 = bisection_method(f2, f2_interval.a, f2_interval.b, &iteration_bisection_f2, "f2", EPSILON, 1);
    double root_bisetion_f3 = bisection_method(f3, f3_interval.a, f3_interval.b, &iteration_bisection_f3, "f3", EPSILON, 1);

    // Вывод информации для пользователя 
    printf("-------------------- метод Ньютона --------------------\n");
    printf("f1(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", root_newton_f1, iteration_newton_f1, f1_interval.a, f1_interval.b);
    printf("f2(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", root_newton_f2, iteration_newton_f2, f2_interval.a, f2_interval.b);
    printf("f3(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", root_newton_f3, iteration_newton_f3, f3_interval.a, f3_interval.b);

    printf("\n");

    printf("-------------------- метод бисекции --------------------\n");
    printf("f1(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", root_bisetion_f1, iteration_bisection_f1, f1_interval.a, f1_interval.b);
    printf("f2(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", root_bisetion_f2, iteration_bisection_f2, f2_interval.a, f2_interval.b);
    printf("f3(x) = %.15lf (итераций : %d) интервал : [%.4lf, %.4lf]\n", root_bisetion_f3, iteration_bisection_f3, f3_interval.a, f3_interval.b);

    printf("\nПогрешности записаны в файл errors.txt\n");

    // Построение графиков функций для визуализации
    plot_function(f1, -10, 10, 0.1, "f1_plot.txt");
    plot_function(f2, -10, 10, 0.1, "f2_plot.txt");
    plot_function(f3, -15, 15, 0.1, "f3_plot.txt");

    printf("Данные для построения графиков сохранены в файлы f1_plot.txt, f2_plot.txt, f3_plot.txt\n");

    FILE* f_accuracy = fopen("accuracy_error_analysis.txt", "w");
    if (f_accuracy == NULL) {
        fprintf(stderr, "Не удалось открыть файл %s для записи.\n");
        return;
    }
    double tolerance;
    for (int i = -15; i <= -1; i++) {
        tolerance = pow(10, i);
        fprintf(f_accuracy, "%s:%s:%.15lf\n", "f1", "Newton", newton_method(f1, df1, &iteration_newton_f1, f1_interval.a, "f1", tolerance, 0));
        fprintf(f_accuracy, "%s:%s:%.15lf\n", "f2", "Newton", newton_method(f2, df2, &iteration_newton_f2, f2_interval.a, "f2", tolerance, 0));
        fprintf(f_accuracy, "%s:%s:%.15lf\n", "f3", "Newton", newton_method(f3, df3, &iteration_newton_f3, f3_interval.a, "f3", tolerance, 0));

        fprintf(f_accuracy, "%s:%s:%.15lf\n", "f1", "Bisection", bisection_method(f1, f1_interval.a, f1_interval.b, &iteration_bisection_f1, "f1", tolerance, 0));
        fprintf(f_accuracy, "%s:%s:%.15lf\n", "f2", "Bisection", bisection_method(f2, f2_interval.a, f2_interval.b, &iteration_bisection_f2, "f2", tolerance, 0));
        fprintf(f_accuracy, "%s:%s:%.15lf\n", "f3", "Bisection", bisection_method(f3, f3_interval.a, f3_interval.b, &iteration_bisection_f3, "f3", tolerance, 0));
    }
    fclose(f_accuracy);

    return 0;
}
