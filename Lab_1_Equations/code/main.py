import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter
from math import exp

# Определяем функции с использованием numpy для векторизации
def f1(x):
    return np.exp(-x**2) + 1 - x

def f2(x):
    return x**3 - 2 * x**2 - 4 * x - 7

def f3(x):
    # Избегаем деления на ноль
    return np.where(x != 0, x**3 - 2 * x**2 - 4 * x - 7 / x, np.nan)

def get_values_for_method(values, method_name, function_name):
    return [float(i[2].replace(',', '.')) for i in values if i[0] == function_name and i[1] == method_name]

def read_file(file_name):
    try:
        with open(file_name, "r") as f:
            return [line.rstrip().split(':') for line in f.readlines()]
    except FileNotFoundError:
        print(f"Файл {file_name} не найден.")
        return []

# Функция для построения графиков (зависимость погрешности от точности) с добавлением биссектрисы
def plot_function_accuracy_error(x_epsilon, y_values, label, color):
    ax = plt.gca()
    ax.yaxis.set_major_formatter(ScalarFormatter())
    ax.ticklabel_format(useOffset=False, style='plain', axis='y')

    # Построение метода
    plt.plot(x_epsilon, y_values, label=label, color=color, marker='o', markersize=3)
    plt.xscale("symlog", linthresh=1e-15)
    plt.yscale("symlog", linthresh=1e-15)
    plt.xlabel("Эпсилон (логарифмическая шкала)")
    plt.ylabel("Значение функции")
    plt.legend()

# Функция для построения графиков (зависимость погрешности от количества итераций) с добавлением биссектрисы
def plot_function_iteration_error(x, y, title, label, color):
    plt.figure(figsize=(10, 8))
    ax = plt.gca()
    ax.yaxis.set_major_formatter(ScalarFormatter())
    ax.ticklabel_format(useOffset=False, style='plain', axis='y')

    # Построение метода
    plt.plot(x, y, label=label, color=color, marker='o', markersize=3)
    plt.grid()
    plt.yscale("symlog", linthresh=1e-15)
    plt.xlabel("Количество итераций")
    plt.ylabel("Погрешность")
    plt.legend()
    plt.title(title)
    plt.show()

# Функция для нахождения верхней границы корней многочлена
def find_upper_bound(coefficients):
    # Теорема верхней границы корней для многочленов
    # bound = 1 + max(|a_i|) / |a_n|
    bound = 1 + max([abs(coeff) for coeff in coefficients[:-1]]) / abs(coefficients[-1])
    return bound

# Функция для иллюстрации слияния корней при крупном шаге
def plot_root_merging(function, func_name, x_range=(-10, 10), fine_points=1000, coarse_points=50):
    x_min, x_max = x_range
    x_fine = np.linspace(x_min, x_max, fine_points)  # Мелкий шаг
    x_coarse = np.linspace(x_min, x_max, coarse_points)  # Крупный шаг

    y_fine = function(x_fine)
    y_coarse = function(x_coarse)

    plt.figure(figsize=(10, 8))
    
    # График с мелким шагом
    plt.plot(x_fine, y_fine, label="Точный шаг", color='g')
    
    # График с крупным шагом
    plt.plot(x_coarse, y_coarse, label="Крупный шаг", color='r', marker='o')

    plt.axhline(0, color='black', linewidth=1)  # Линия оси x
    plt.axvline(0, color='black', linewidth=1)  # Линия оси y

    plt.xlabel('x')
    plt.ylabel('y')
    plt.title(f"Иллюстрация слияния корней для {func_name} при крупном шаге")
    plt.legend()
    plt.grid()
    plt.show()

# Читаем данные из файла с обработкой ошибок
values_accuracy_error = read_file("accuracy_error_analysis.txt")
values_error = read_file("iteration_error_analysis.txt")

# Определяем значения epsilon для методов Ньютона и Бисекции
x_epsilon = [10**i for i in range(-1, -16, -1)]

# Инициализация списков для результатов погрешностей методов Ньютона и Бисекции от заданной точности
y_f1_newton_accuracy_error = get_values_for_method(values_accuracy_error, "Newton", "f1")
y_f2_newton_accuracy_error = get_values_for_method(values_accuracy_error, "Newton", "f2")
y_f3_1_newton_accuracy_error = get_values_for_method(values_accuracy_error, "Newton", "f3_1")
y_f3_2_newton_accuracy_error = get_values_for_method(values_accuracy_error, "Newton", "f3_2")

y_f1_bisection_accuracy_error = get_values_for_method(values_accuracy_error, "Bisection", "f1")
y_f2_bisection_accuracy_error = get_values_for_method(values_accuracy_error, "Bisection", "f2")
y_f3_1_bisection_accuracy_error = get_values_for_method(values_accuracy_error, "Bisection", "f3_1")
y_f3_2_bisection_accuracy_error = get_values_for_method(values_accuracy_error, "Bisection", "f3_2")

# Инициализация списков для результатов погрешностей методов Ньютона и Бисекции от количества итераций
y_f1_newton_iteration_error = get_values_for_method(values_error, "Newton", "f1")
x_f1_newton_iteration_error = list(range(1, len(y_f1_newton_iteration_error) + 1))

y_f2_newton_iteration_error = get_values_for_method(values_error, "Newton", "f2")
x_f2_newton_iteration_error = list(range(1, len(y_f2_newton_iteration_error) + 1))

y_f3_1_newton_iteration_error = get_values_for_method(values_error, "Newton", "f3_1")
x_f3_1_newton_iteration_error = list(range(1, len(y_f3_1_newton_iteration_error) + 1))

y_f3_2_newton_iteration_error = get_values_for_method(values_error, "Newton", "f3_2")
x_f3_2_newton_iteration_error = list(range(1, len(y_f3_2_newton_iteration_error) + 1))

# метод бисекции
y_f1_bisection_iteration_error = get_values_for_method(values_error, "Bisection", "f1")
x_f1_bisection_iteration_error = list(range(1, len(y_f1_bisection_iteration_error) + 1))

y_f2_bisection_iteration_error = get_values_for_method(values_error, "Bisection", "f2")
x_f2_bisection_iteration_error = list(range(1, len(y_f2_bisection_iteration_error) + 1))

y_f3_1_bisection_iteration_error = get_values_for_method(values_error, "Bisection", "f3_1")
x_f3_1_bisection_iteration_error = list(range(1, len(y_f3_1_bisection_iteration_error) + 1))

y_f3_2_bisection_iteration_error = get_values_for_method(values_error, "Bisection", "f3_2")
x_f3_2_bisection_iteration_error = list(range(1, len(y_f3_2_bisection_iteration_error) + 1))

# Генерация значений для построения графиков функций
x_f = np.linspace(-20, 20, 1000)
y_f1 = f1(x_f)
y_f2 = f2(x_f)
y_f3 = f3(x_f)

# -------- Построение Графиков Функций --------
plt.figure(figsize=(9, 9))
plt.grid()

ax = plt.gca()
ax.spines['left'].set_position('center')  # Ось y по центру
ax.spines['bottom'].set_position('center')  # Ось x по центру
ax.spines['top'].set_visible(False)  # Убираем верхнюю ось
ax.spines['right'].set_visible(False)  # Убираем правую ось

# Построение функций
plt.plot(x_f, y_f1, label="f1(x)", color='g')
plt.plot(x_f, y_f2, label="f2(x)", color='r')
plt.plot(x_f, y_f3, label="f3(x)", color='b')
plt.xticks([i for i in range(-10, 11)])
plt.axhline(0, color='black', linewidth=1)  # Добавляем линию по оси x
plt.axvline(0, color='black', linewidth=1)  # Добавляем линию по оси y

plt.xlabel('x', loc='right')
plt.ylabel('y', loc='top')
plt.title("Графики функций f1, f2, f3")
plt.ylim(-20, 20)
plt.xlim(-10, 10)
plt.legend()
plt.show()

# -------- Аналитическое Определение Промежутков для Корней --------

# Для f2(x) = x^3 - 2x^2 - 4x - 7
coefficients_f2 = [1, -2, -4, -7]  # Коэффициенты многочлена
upper_bound_f2 = find_upper_bound(coefficients_f2)

print(f"Верхняя граница корней для f2(x): {upper_bound_f2}")

# Теорема о верхней границе для f1 и f3 неприменима, так как f1 не является многочленом, а f3 имеет член -7/x.

# -------- Иллюстрация Слияния Корней для Всех Трех Функций --------

# f1(x): exp(-x^2) + 1 - x
plot_root_merging(f1, "f1(x)", x_range=(-3, 3), fine_points=1000, coarse_points=50)

# f2(x): x^3 - 2x^2 - 4x - 7
plot_root_merging(f2, "f2(x)", x_range=(-5, 5), fine_points=1000, coarse_points=50)

# f3(x): x^3 - 2x^2 - 4x - 7/x
# Избегаем x=0 из-за разрыва
plot_root_merging(f3, "f3(x)", x_range=(-5, -0.1), fine_points=1000, coarse_points=50)
plot_root_merging(f3, "f3(x)", x_range=(0.1, 5), fine_points=1000, coarse_points=50)

# -------- Построение Графиков Зависимости Погрешности от Точности --------

# Для f1
plt.figure(figsize=(10, 8))
plt.plot(x_epsilon, x_epsilon, label="Биссектриса y=x", linestyle='--', color='gray')  # Биссектриса
plt.grid()
plot_function_accuracy_error(x_epsilon, y_f1_newton_accuracy_error, "f1(x) - Метод Ньютона", 'r')
plot_function_accuracy_error(x_epsilon, y_f1_bisection_accuracy_error, "f1(x) - Метод бисекции", 'g')
plt.title("Зависимость погрешности от точности для f1(x)")
plt.show()

# Для f2
plt.figure(figsize=(10, 8))
plt.plot(x_epsilon, x_epsilon, label="Биссектриса y=x", linestyle='--', color='gray')  # Биссектриса
plt.grid()
plot_function_accuracy_error(x_epsilon, y_f2_newton_accuracy_error, "f2(x) - Метод Ньютона", 'r')
plot_function_accuracy_error(x_epsilon, y_f2_bisection_accuracy_error, "f2(x) - Метод бисекции", 'g')
plt.title("Зависимость погрешности от точности для f2(x)")
plt.show()

# Для f3_1
plt.figure(figsize=(10, 8))
plt.plot(x_epsilon, x_epsilon, label="Биссектриса y=x", linestyle='--', color='gray')  # Биссектриса
plt.grid()
plot_function_accuracy_error(x_epsilon, y_f3_1_newton_accuracy_error, "f3_1(x) - Метод Ньютона", 'r')
plot_function_accuracy_error(x_epsilon, y_f3_1_bisection_accuracy_error, "f3_1(x) - Метод бисекции", 'g')
plt.title("Зависимость погрешности от точности для f3(x)")
plt.show()

# Для f3_2
plt.figure(figsize=(10, 8))
plt.plot(x_epsilon, x_epsilon, label="Биссектриса y=x", linestyle='--', color='gray')  # Биссектриса
plt.grid()
plot_function_accuracy_error(x_epsilon, y_f3_2_newton_accuracy_error, "f3_2(x) - Метод Ньютона", 'r')
plot_function_accuracy_error(x_epsilon, y_f3_2_bisection_accuracy_error, "f3_2(x) - Метод бисекции", 'g')
plt.title("Зависимость погрешности от точности для f3(x)")
plt.show()
# -------- Построение Графиков Зависимости Погрешности от Количества Итераций --------

# Для f1 - Метод Ньютона
plot_function_iteration_error(x_f1_newton_iteration_error, y_f1_newton_iteration_error, "f1(x) Метод Ньютона", "f1(x)", 'r')

# Для f2 - Метод Ньютона
plot_function_iteration_error(x_f2_newton_iteration_error, y_f2_newton_iteration_error, "f2(x) Метод Ньютона", "f2(x)", 'g')

# Для f3_1 - Метод Ньютона
plot_function_iteration_error(x_f3_1_newton_iteration_error, y_f3_1_newton_iteration_error, "f3(x) Метод Ньютона", "f3(x)", 'b')

# Для f3_2 - Метод Ньютона
plot_function_iteration_error(x_f3_2_newton_iteration_error, y_f3_2_newton_iteration_error, "f3(x) Метод Ньютона", "f3(x)", 'b')

# Для f1 - Метод Бисекции
plot_function_iteration_error(x_f1_bisection_iteration_error, y_f1_bisection_iteration_error, "f1(x) Метод Бисекции", "f1(x)", 'r')

# Для f2 - Метод Бисекции
plot_function_iteration_error(x_f2_bisection_iteration_error, y_f2_bisection_iteration_error, "f2(x) Метод Бисекции", "f2(x)", 'g')

# Для f3_1 - Метод Бисекции
plot_function_iteration_error(x_f3_1_bisection_iteration_error, y_f3_1_bisection_iteration_error, "f3(x) Метод Бисекции", "f3(x)", 'b')

# Для f3_2 - Метод Бисекции
plot_function_iteration_error(x_f3_2_bisection_iteration_error, y_f3_2_bisection_iteration_error, "f3(x) Метод Бисекции", "f3(x)", 'b')
