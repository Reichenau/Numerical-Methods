import matplotlib.pyplot as plt


# Функция для чтения данных из файла и их преобразования
def read_data(file_name):
    with open(file_name, 'r') as f:
        return [line.strip().split(':') for line in f.readlines()]


# Функция для сортировки ошибок по методам и функциям
def sort_errors(data):
    errors = {
        'f1': {'Newton': [], 'Bisection': []},
        'f2': {'Newton': [], 'Bisection': []},
        'f3': {'Newton': [], 'Bisection': []}
    }
    for func, method, error in data:
        error = float(error.replace(',', '.'))
        if func in errors and method in errors[func]:
            errors[func][method].append(error)
    return errors


# Построение графика с общими параметрами
def plot_error(x, y, title):
    plt.figure(figsize=(12, 7))
    plt.plot(x, y, color='green', marker='o', markersize=3)
    plt.xlabel("Количество итераций")
    plt.ylabel("Погрешность")
    plt.yscale("symlog", linthresh=1e-15)
    plt.title(title)
    plt.grid()


# Чтение и сортировка ошибок
errors_data = read_data('errors.txt')
errors = sort_errors(errors_data)

# Построение графиков для метода Ньютона и Бисекции
for func in ['f1', 'f2', 'f3']:
    for method in ['Newton', 'Bisection']:
        x = list(range(1, len(errors[func][method]) + 1))
        plot_error(x, errors[func][method], f"Метод {method}\n{func}(x)")


# Универсальная функция для чтения данных и построения графиков из файлов
def plot_from_file(file_name, title):
    with open(file_name, 'r') as f:
        values = [line.strip().split(" ") for line in f.readlines()]
    x = [float(i[0].replace(',', '.')) for i in values]
    y = [float(i[1].replace(',', '.')) for i in values]

    plt.figure(figsize=(12, 7))
    plt.plot(x, y, color='green')
    plt.xlim(-15, 15)
    plt.ylim(-40, 15)
    plt.grid()
    plt.title(title)


# Построение графиков для f1_plot, f2_plot, f3_plot
for i in range(1, 4):
    plot_from_file(f"f{i}_plot.txt", f"f{i}(x)")

# Чтение погрешностей для accuracy_error_analysis.txt
accuracy_data = read_data("accuracy_error_analysis.txt")
accuracy_errors = sort_errors(accuracy_data)

# Построение графиков для accuracy_error_analysis
x = [10 ** i for i in range(-15, 0, 1)]
for func in ['f1', 'f2', 'f3']:
    for method in ['Newton', 'Bisection']:
        plt.figure(figsize=(12, 7))
        plt.title(f"{func} - {method}")
        plt.plot(x, accuracy_errors[func][method], color='green', marker='o', markersize=3)
        plt.xscale("symlog", linthresh=1e-15)
        plt.grid()

plt.show()