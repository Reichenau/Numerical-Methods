#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

// -------------------- DEFINES --------------------
#define EPSILON 1e-15 // �������� �����������
#define MAX_ITERATIONS 1000 // ����������� �� ����������� ��������� ���������� �������� 
#define STEP 0.01 // ��� ��� ���������� ������ ������

// -------------------- STRUCTURES --------------------
/**
 * ��������� ��� �������� ��������� [a, b], � ������� ������� ������ ����
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

// ������������� ������ ����������� [a,b]
// ������� ������� ��� ���������, ��� ������� f ������ ����, �������� ���������, ���������� x=0
/*
 * ������� ��� ��������� [a, b], ��� ������� f ������ ����
 * @param f ��������� �� �������
 * @param intervals ������ �������� ��� �������� ����������
 * @param count ��������� �� ���������� ��� �������� ���������� ��������� ����������
 */
void find_sign_changes(double (*f)(double), struct f_interval* intervals, int* count) {
    double a = -100.0; // ����� ������� ������
    double b = 100.0;  // ������ ������� ������
    *count = 0;         // ������������� �������� ��������� ����������

    // �������� �� ����������� � ����� STEP ��� ������ ��������� ����� �������
    while (a < b) {
        double fa = f(a);
        double fb = f(a + STEP);

        // ���������, �� �������� �� �������� x=0
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
 * ���������� ����������� � ���� � ��������� ������� � ������
 * @param function_name �������� ������� (��������, "f1")
 * @param method_name �������� ������ (��������, "Newton")
 * @param error �����������
 * @param f_name ��� ����� ��� ������ �����������
 */
void error_record(const char* function_name, const char* method_name, double error, const char* f_name) {
    FILE* f = fopen(f_name, "a");

    // ���������, ������� �� ������� ����
    if (f == NULL) {
        fprintf(stderr, "������: �� ������� ������� ���� %s ��� ������.\n", f_name);
        return;
    }

    // ���������� ����������� � ��������� ������� � ������
    fprintf(f, "%s:%s:%.15lf\n", function_name, method_name, error);

    fclose(f);
}

// ����� ������� 
/**
 * ������� ����������� ������������ �������� ��������� ������� �������
 * @param f ��������� �� �������
 * @param df ��������� �� ����������� �������� �������
 * @param iteration ��������� �� ���������� ��� �������� ���������� ��������
 * @param x0 ��������� �����������
 * @param function_name �������� ������� ��� ������ ������������
 * @param epsilon �������� �����������
 * @param flag ���� ��� ����������� ���� ������ ����������� (1 - ������������, 0 - ��������)
 * @return ������������ �������� ����� �������
 */
double newton_method(double (*f)(double), double (*df)(double), int* iteration, double x0, const char* function_name, double epsilon, int flag) {
    double x1; // ��������� �����������
    double current_error; // ������� �����������
    int iter = 0; // ������� ��������
    char method_name[] = "Newton"; // �������� ������

    // ������������ ������� ������ �������
    do {
        if (df(x0) == 0) {
            printf("����������� ������ � ����. ����� ������� �� ����� ������������.\n");
            return 0;
        }
        x1 = x0 - f(x0) / df(x0);
        // ������ �����������
        current_error = fabs(x1 - x0);
        // ������ ������ �����������, ���� ���� �� ����������
        if (flag == 1) {
            error_record(function_name, method_name, current_error, "iteration_error_analysis.txt");
        }
        x0 = x1;
        iter++;

    } while (current_error > epsilon && iter < MAX_ITERATIONS);
    *iteration = iter;
    // ������ ������ �����������, ���� ���� �� ����������
    if (flag == 0) {
        error_record(function_name, method_name, current_error, "accuracy_error_analysis.txt");
    }
    return x1;
}

// ����� �������� 
/**
 * ������� ����������� ������������ �������� ��������� ������� ��������
 * @param f ��������� �� �������
 * @param a ����� ������� ���������
 * @param b ������ ������� ���������
 * @param iteration ��������� �� ���������� ��� �������� ���������� ��������
 * @param function_name �������� ������� ��� ������ ������������
 * @param epsilon �������� �����������
 * @param flag ���� ��� ����������� ���� ������ ����������� (1 - ������������, 0 - ��������)
 * @return ������������ �������� ����� ������� ��� NAN � ������ ������
 */
double bisection_method(double (*f)(double), double a, double b, int* iteration, const char* function_name, double epsilon, int flag) {
    double c; // ������� ����� ���������
    int iter = 0; // ������� ��������
    char method_name[] = "Bisection"; // �������� ������
    double current_error; // ������� �����������
    double fa = f(a); // �������� ������� � ����� a
    double fb = f(b); // �������� ������� � ����� b
    double fc; // �������� ������� � ����� c

    // �������� �� ������������ ���������� ��������� 
    if (fa * fb >= 0) {
        printf("����� �������� �� �������� ��� ������� %s: f(a) � f(b) ����� ���������� �����.\n", function_name);
        return NAN;
    }
    // ������������ ������� ������ ��������
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
        // ������ ������������ �����������, ���� ���� ����������
        current_error = fabs(b - a);
        if (flag == 1) {
            error_record(function_name, method_name, current_error, "iteration_error_analysis.txt");
        }

    } while (current_error > epsilon && iter < MAX_ITERATIONS);
    // �������� �� ���������� ������������� ���������� ��������
    if (iter >= MAX_ITERATIONS) {
        printf("����� �������� ��� ������� %s �� ������� �� %d ��������.\n", function_name, MAX_ITERATIONS);
        return NAN;
    }
    *iteration = iter;

    // ������ ������ �����������, ���� ���� �� ����������
    if (flag == 0) {
        error_record(function_name, method_name, current_error, "accuracy_error_analysis.txt");
    }
    return c;
}

// -------------------- MAIN --------------------
int main() {
    setlocale(LC_ALL, "Russian");

    // ������� ����� error_analysis ����� ������� ������
    FILE* f_error_clear = fopen("iteration_error_analysis.txt", "w");
    FILE* f_accuracy_clear = fopen("accuracy_error_analysis.txt", "w");

    // �������� �� �������� �������� � ������� ����� iteration_error_analysis.txt
    if (f_error_clear != NULL) {
        fclose(f_error_clear);
    }
    else {
        fprintf(stderr, "������: �� ������� �������� ���� iteration_error_analysis.txt\n");
        return 1;
    }

    // �������� �� �������� �������� � ������� ����� accuracy_error_analysis.txt
    if (f_accuracy_clear != NULL) {
        fclose(f_accuracy_clear);
    }
    else {
        fprintf(stderr, "������: �� ������� �������� ���� accuracy_error_analysis.txt\n");
        return 1;
    }

    // ����� ���������� ��� ���������
    struct f_interval f1_interval, f2_interval;
    struct f_interval f3_intervals[1000]; // ������ ��� �������� ���������� f3
    int f3_count = 0; // ���������� ��������� ���������� ��� f3

    // ��� f1 � f2 ���������� ������������ �������, ������� ������� ������ ���� ��������
    find_sign_changes(f1, &f1_interval, &(int){1}); 
    find_sign_changes(f2, &f2_interval, &(int){1}); 

    // ��� f3 ������� ��� ���������
    find_sign_changes(f3, f3_intervals, &f3_count); 

    // ���������� ��������� ���������� �������� ��� ������� ������ � �������
    int iteration_newton_f1, iteration_newton_f2, iteration_newton_f3;
    int iteration_bisection_f1, iteration_bisection_f2, iteration_bisection_f3;

    // ���������� ������ ������� � ������ �������
    double root_newton_f1 = newton_method(f1, df1, &iteration_newton_f1, f1_interval.a, "f1", EPSILON, 1);
    double root_newton_f2 = newton_method(f2, df2, &iteration_newton_f2, f2_interval.a, "f2", EPSILON, 1);

    // ��� f3 ���������� �������� �� ���� ��������� ����������
    double root_newton_f3[1000];
    char f3_label_newton[10];
    for (int i = 0; i < f3_count; i++) {
        // ��������� ����� f3_1, f3_2 � �.�.
        snprintf(f3_label_newton, sizeof(f3_label_newton), "f3_%d", i + 1);
        root_newton_f3[i] = newton_method(f3, df3, &iteration_newton_f3, f3_intervals[i].a, f3_label_newton, EPSILON, 1);
    }

    // ���������� ������ �������� � ������ �������
    double root_bisetion_f1 = bisection_method(f1, f1_interval.a, f1_interval.b, &iteration_bisection_f1, "f1", EPSILON, 1);
    double root_bisetion_f2 = bisection_method(f2, f2_interval.a, f2_interval.b, &iteration_bisection_f2, "f2", EPSILON, 1);

    // ��� f3 ���������� �������� �� ���� ��������� ����������
    double root_bisetion_f3[1000];
    char f3_label_bisection[10];
    for (int i = 0; i < f3_count; i++) {
        // ��������� ����� f3_1, f3_2 � �.�.
        snprintf(f3_label_bisection, sizeof(f3_label_bisection), "f3_%d", i + 1);
        root_bisetion_f3[i] = bisection_method(f3, f3_intervals[i].a, f3_intervals[i].b, &iteration_bisection_f3, f3_label_bisection, EPSILON, 1);
    }

    // ����� ���������� ��� ������������ � ����������� ������ �������
    printf("-------------------- ����� ������� --------------------\n");
    printf("f1(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", root_newton_f1, iteration_newton_f1, f1_interval.a, f1_interval.b);
    printf("f2(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", root_newton_f2, iteration_newton_f2, f2_interval.a, f2_interval.b);

    // ����� ����������� ��� ������� ��������� f3
    for (int i = 0; i < f3_count; i++) {
        printf("f3_%d(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", i + 1, root_newton_f3[i], iteration_newton_f3, f3_intervals[i].a, f3_intervals[i].b);
    }

    printf("\n");

    // ����� ���������� ��� ������������ � ����������� ������ ��������
    printf("-------------------- ����� �������� --------------------\n");
    printf("f1(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", root_bisetion_f1, iteration_bisection_f1, f1_interval.a, f1_interval.b);
    printf("f2(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", root_bisetion_f2, iteration_bisection_f2, f2_interval.a, f2_interval.b);

    // ����� ����������� ��� ������� ��������� f3
    for (int i = 0; i < f3_count; i++) {
        printf("f3_%d(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", i + 1, root_bisetion_f3[i], iteration_bisection_f3, f3_intervals[i].a, f3_intervals[i].b);
    }

    printf("\n����������� �������� � ���� accuracy_error_analysis.txt\n");

    // ���������� ��� ������� �����������
    double epsilon_val;

    // ���� �� ��������� ��������� ����������� �� 1e-1 �� 1e-15
    for (int i = -1; i >= -15; i--) {
        epsilon_val = pow(10, i); // ���������� ������� �����������

        // ���������� ������ ������� � ������� ������������
        newton_method(f1, df1, &iteration_newton_f1, f1_interval.a, "f1", epsilon_val, 0);
        newton_method(f2, df2, &iteration_newton_f2, f2_interval.a, "f2", epsilon_val, 0);

        // ��� f3 ���������� �������� �� ���� ��������� ����������
        for (int i_f3 = 0; i_f3 < f3_count; i_f3++) {
            // ��������� ����� f3_1, f3_2 � �.�.
            snprintf(f3_label_newton, sizeof(f3_label_newton), "f3_%d", i_f3 + 1);
            newton_method(f3, df3, &iteration_newton_f3, f3_intervals[i_f3].a, f3_label_newton, epsilon_val, 0);
        }

        // ���������� ������ �������� � ������� ������������
        bisection_method(f1, f1_interval.a, f1_interval.b, &iteration_bisection_f1, "f1", epsilon_val, 0);
        bisection_method(f2, f2_interval.a, f2_interval.b, &iteration_bisection_f2, "f2", epsilon_val, 0);

        // ��� f3 ���������� �������� �� ���� ��������� ����������
        for (int i_f3 = 0; i_f3 < f3_count; i_f3++) {
            // ��������� ����� f3_1, f3_2 � �.�.
            snprintf(f3_label_bisection, sizeof(f3_label_bisection), "f3_%d", i_f3 + 1);
            bisection_method(f3, f3_intervals[i_f3].a, f3_intervals[i_f3].b, &iteration_bisection_f3, f3_label_bisection, epsilon_val, 0);
        }
    }
    return 0;
}
