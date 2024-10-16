#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#define EPSILON 1e-15 // �������� �����������
#define MAX_ITERATIONS 1000 // ����������� �� ����������� ��������� ���������� �������� 
#define STEP 1 // ��� ��� ���������� ������ ������

struct f_interval {
    double a, b;
};

// -------------------- FUNCTIONS --------------------
// �������� ������ ������� f1(x) = exp(-x^2)-x+1 [1,2]
double f1(double x) {
    return exp(-pow(x, 2)) + 1 - x;
}

// ����������� ������ ������� df1(x) = -2xexp(-x^2)-1
double df1(double x) {
    return -2 * x * exp(-pow(x, 2)) - 1;
}

// �������� ������ ������� f2(x) = x^3-2x^2-4x-7 [3,4]
double f2(double x) {
    return pow(x, 3) - 2 * pow(x, 2) - 4 * x - 7;
}

// ����������� ������ ������� df2(x) = 3x^2 - 4x - 4
double df2(double x) {
    return 3 * pow(x, 2) - 4 * x - 4;
}

// �������� ������� f3(x) = x^3-2x^2-4x-7/x (� ��������)
double f3(double x) {
    return pow(x, 3) - 2 * pow(x, 2) - 4 * x - 7 / x;
}
// ����������� ������� ������� df3(x) = (3*x^4-4*x^3-4*x^2+7)/(x^2)
double df3(double x) {
    return 3 * pow(x, 2) - 4 * x - 4 + 7 / pow(x, 2);
}

// ���������� ��������� [a,b]
/*
 * ������� �������� [a, b], ��� ������� f ������ ����
 * @param f ��������� �� �������
 * @param method ��������� �� ��������� ��� �������� ���������
 */
void find_sign_changes(double (*f)(double), struct f_interval* method) {
    double a = -100.0; // ����� �������
    double b = 100.0; // ������ �������

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
 * ���������� ����������� � ���� � ��������� ������� � ������
 * @param function_name �������� ������� (��������, "f1")
 * @param method_name �������� ������ (��������, "Newton")
 * @param error �����������
 */
void error_record(const char* function_name, const char* method_name, double error) {
    FILE* f = fopen("errors.txt", "a");

    // ���������, ������� �� ������� ����
    if (f == NULL) {
        fprintf(stderr, "������: �� ������� ������� ���� errors.txt ��� ������.\n");
        return;
    }

    // ���������� ����������� � ��������� ������� � ������
    fprintf(f, "%s:%s:%.15lf\n", function_name, method_name, error);

    fclose(f);
}
void plot_function(double (*f)(double), double x_min, double x_max, double step, const char* f_name) {
    FILE* fp = fopen(f_name, "w+");
    if (f == NULL) {
        fprintf(stderr, "�� ������� ������� ���� %s ��� ������.\n", f_name);
        return;
    }
    for (double x = x_min; x <= x_max; x += step) {
        fprintf(fp, "%lf %.15lf\n", x, f(x));
    }
    fclose(fp);
}
// -------------------- METHODS --------------------
// ����� ������� 
/**
 * ������� ����������� ������������ �������� ��������� �������
 * @param f ��������� �� �������
 * @param df ��������� �� ����������� �������� �������
 * @param iteration ��������� �� ���������� ��� �������� ���������� ��������
 * @param x0 ��������� �����������
 * @param function_name �������� ������� ��� ������ ������������
 */
double newton_method(double (*f)(double), double (*df)(double), int* iteration, double x0, const char* function_name, double epsilon, int flag) {
    double x, x1;
    double current_error;
    int iter = 0;
    char method_name[] = "Newton";
    do {
        // �������� �� ��������� ����������� 
        if (df(x0) == 0) {
            //printf("����������� ������ � ����. ����� ������� �� ����� ������������.\n");
            return 0;
        }
        x1 = x0 - f(x0) / df(x0);
        // ������ �����������
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

// ����� �������� 
/**
 * ������� ����������� ������������ �������� ��������� �������
 * @param f ��������� �� �������
 * @param a ����� ������� ���������
 * @param b ������ ������� ���������
 * @param iteration ��������� �� ���������� ��� �������� ���������� ��������
 * @param function_name �������� ������� ��� ������ ������������
 */
double bisection_method(double (*f)(double), double a, double b, int* iteration, const char* function_name, double epsilon, int flag) {
    double c;
    int iter = 0;
    char method_name[] = "Bisection";
    double current_error;
    double fa = f(a);
    double fb = f(b);
    double fc;
    // �������� �� ������������ ���������� ��������� 
    if (fa * fb >= 0) {
        printf("����� �������� �� �������� ��� ������� %s: f(a) � f(b) ����� ���������� �����.\n", function_name);
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
        // ������ �����������
        current_error = fabs(b - a);
        if (flag == 1) {
            error_record(function_name, method_name, current_error);
        }
    } while (fabs(b - a) > epsilon && iter < MAX_ITERATIONS);
    if (iter >= MAX_ITERATIONS) {
        printf("����� �������� ��� ������� %s �� ������� �� %d ��������.\n", function_name, MAX_ITERATIONS);
        return NAN;
    }
    *iteration = iter;
    return c;
}

// -------------------- MAIN --------------------
int main() {
    setlocale(LC_ALL, "Russian");

    // ������� ���� errors.txt ����� ������� ������
    FILE* f_clear = fopen("errors.txt", "w");
    if (f_clear != NULL) {
        fclose(f_clear);
    }
    else {
        fprintf(stderr, "������: �� ������� �������� ���� errors.txt.\n");
        return 1;
    }

    // ����� ���������� ��� ��������� 
    struct f_interval f1_interval, f2_interval, f3_interval;

    find_sign_changes(f1, &f1_interval);
    find_sign_changes(f2, &f2_interval);
    find_sign_changes(f3, &f3_interval);

    // ������� ���������� �������� ��� ������� ������ � ������� 
    int iteration_newton_f1, iteration_newton_f2, iteration_newton_f3;
    int iteration_bisection_f1, iteration_bisection_f2, iteration_bisection_f3;

    // ���������� ������� �������
    double root_newton_f1 = newton_method(f1, df1, &iteration_newton_f1, f1_interval.a, "f1", EPSILON, 1);
    double root_newton_f2 = newton_method(f2, df2, &iteration_newton_f2, f2_interval.a, "f2", EPSILON, 1);
    double root_newton_f3 = newton_method(f3, df3, &iteration_newton_f3, f3_interval.a, "f3", EPSILON, 1);

    // ���������� ������� ��������
    double root_bisetion_f1 = bisection_method(f1, f1_interval.a, f1_interval.b, &iteration_bisection_f1, "f1", EPSILON, 1);
    double root_bisetion_f2 = bisection_method(f2, f2_interval.a, f2_interval.b, &iteration_bisection_f2, "f2", EPSILON, 1);
    double root_bisetion_f3 = bisection_method(f3, f3_interval.a, f3_interval.b, &iteration_bisection_f3, "f3", EPSILON, 1);

    // ����� ���������� ��� ������������ 
    printf("-------------------- ����� ������� --------------------\n");
    printf("f1(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", root_newton_f1, iteration_newton_f1, f1_interval.a, f1_interval.b);
    printf("f2(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", root_newton_f2, iteration_newton_f2, f2_interval.a, f2_interval.b);
    printf("f3(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", root_newton_f3, iteration_newton_f3, f3_interval.a, f3_interval.b);

    printf("\n");

    printf("-------------------- ����� �������� --------------------\n");
    printf("f1(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", root_bisetion_f1, iteration_bisection_f1, f1_interval.a, f1_interval.b);
    printf("f2(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", root_bisetion_f2, iteration_bisection_f2, f2_interval.a, f2_interval.b);
    printf("f3(x) = %.15lf (�������� : %d) �������� : [%.4lf, %.4lf]\n", root_bisetion_f3, iteration_bisection_f3, f3_interval.a, f3_interval.b);

    printf("\n����������� �������� � ���� errors.txt\n");

    // ���������� �������� ������� ��� ������������
    plot_function(f1, -10, 10, 0.1, "f1_plot.txt");
    plot_function(f2, -10, 10, 0.1, "f2_plot.txt");
    plot_function(f3, -15, 15, 0.1, "f3_plot.txt");

    printf("������ ��� ���������� �������� ��������� � ����� f1_plot.txt, f2_plot.txt, f3_plot.txt\n");

    FILE* f_accuracy = fopen("accuracy_error_analysis.txt", "w");
    if (f_accuracy == NULL) {
        fprintf(stderr, "�� ������� ������� ���� %s ��� ������.\n");
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
