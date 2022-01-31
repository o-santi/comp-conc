from matplotlib import pyplot as plt
from matplotlib import cm
import matplotlib.ticker as mtick
from subprocess import Popen, PIPE, STDOUT
import re
import numpy as np
from sympy import integrate, Symbol, poly

polinomios = [
    # inicio, fim, grau, coeficientes, título
    [0, 10, 1, [2, 3], r"$\int_0^{10} 2 + 3x ~dx$"],
    [-5, 5, 1, [2, 3], r"$\int_{-5}^{5} 2 + 3x ~dx$"],
    [
        -5,
        5,
        5,
        [1, 2, 3, 4, 5, 6],
        r"$\int_{-5}^{5} 1 + 2x + 3x^2 + 4x^3 + 5x^4 + 6x^5 ~dx$",
    ],
    [0, 10, 3, [4, 3, 2, 1], r"$\int_{0}^{10} 4 + 3x + 2x^2 + x^3 ~dx$"],
    [0, 10, 0, [2], r"$\int_{0}^{10} 2 ~dx$"],
]
labels = ["1 thread", "2 threads", "3 threads", "4 threads"]
n_threads = [1, 2, 3, 4]
n_divisoes = [10**2, 10**3, 10**4, 10**5, 10**6, 10**7]
divisoes_labels = ["$10^2$", "$10^3$", "$10^4$", "$10^5$", "$10^6$", "$10^7$"]


def rodar_programa(inicio, fim, n_divisoes, n_thread, grau, coeficientes):
    processo = Popen(
        ["./implementacao", str(inicio), str(fim), str(n_divisoes), str(n_thread)],
        stdout=PIPE,
        stdin=PIPE,
    )
    output, errors = processo.communicate(
        input=str.encode(f"{grau}\n{' '.join([str(i) for i in coeficientes])}")
    )
    pattern = re.compile(
        r"Digite o grau do polinomio:\nDigite os coeficientes do polinomio:\nResultado obtido foi de (\d+\.\d+) com tempo de execucao de (\d+\.\d+) segundos.\n"
    )
    soma, sec = re.search(pattern, output.decode()).groups()
    return float(soma), float(sec)


def corretude():
    x = Symbol("x")
    markers = ["o", "s", "d", "v"]
    xs = list(range(len(n_divisoes)))
    fig, axs = plt.subplots(len(polinomios), figsize=(10, 20))
    for index, (inicio, fim, grau, coeficientes, title) in enumerate(polinomios):
        f = sum(c * (x**i) for (i, c) in enumerate(coeficientes))
        valor_exato = integrate(
            f, (x, inicio, fim)
        )  # calcula o valor exato do polinomio
        for (i, d) in enumerate(n_divisoes):
            for (j, t) in enumerate(n_threads):
                soma, _ = rodar_programa(
                    inicio, fim, d, t, grau, coeficientes
                )  # acha a soma
                axs[index].plot(
                    i,
                    abs(soma - valor_exato),
                    c=cm.Dark2(j / 4),  # faz o plot do ponto (distancia, erro)
                    marker=markers[j],
                )  # e utiliza o número de threads pra pegar o marcador correto
        axs[index].set_xticks(xs, divisoes_labels)  # ajeitamos as labels de x
        axs[index].legend(labels, loc="upper right")
        axs[index].set_xlabel("Número de divisões do intervalo")
        axs[index].set_ylabel("Erro (log)")  # escala de log para ficar mais legível
        axs[index].set_yscale("log")
        axs[index].set_title(title + f" = {valor_exato}")
    fig.tight_layout()
    fig.savefig(f"corretude_fig.png", dpi=fig.dpi)  # salvamos a imagem


def desempenho():
    markers = ["o", "s", "d", "v"]
    lines = ["-", "--", "-.", ":"]
    xs = list(range(len(n_divisoes)))
    fig, axs = plt.subplots(len(polinomios), 2, figsize=(15, 20))
    for index, (inicio, fim, grau, coeficientes, title) in enumerate(polinomios):
        tempos_total = {}
        for (i, t) in enumerate(n_threads):
            tempos = [
                rodar_programa(inicio, fim, d, t, grau, coeficientes)[1]
                for d in n_divisoes
            ]
            tempos_total.update({t: tempos})
            axs[index][0].plot(
                xs,
                tempos,
                color=cm.Dark2(i / 4),
                marker=markers[i],
                linestyle=lines[i],
            )  # e utiliza o número de threads pra pegar o marcador correto
        base = tempos_total[1]  #
        for n_thread, tempos in tempos_total.items():
            ys = [t_base / t for t_base, t in zip(base, tempos)]
            axs[index][1].plot(
                xs, ys, color=cm.Dark2(n_thread / 4), linestyle=lines[n_thread - 1]
            )
        axs[index][0].set_xticks(xs, divisoes_labels)  # ajeitamos as labels de x
        axs[index][0].legend(labels, loc="upper left")
        axs[index][0].set_xlabel("Número de divisões do intervalo")
        axs[index][0].set_ylabel(
            "Tempo em segundos (log)"
        )  # escala de log para ficar mais legível
        axs[index][0].set_yscale("log")
        axs[index][0].set_title(title)
        axs[index][1].set_xticks(xs, divisoes_labels)  # ajeitamos as labels de x
        axs[index][1].legend(labels, loc="upper left")
        axs[index][1].set_xlabel("Número de divisões do intervalo")
        axs[index][1].set_ylabel(
            "Ganho de desempenho"
        )  # escala de log para ficar mais legível
        axs[index][1].yaxis.set_major_formatter(
            mtick.PercentFormatter(xmax=1, decimals=None, symbol="%", is_latex=False)
        )
        axs[index][1].set_title(title)
    fig.tight_layout()
    fig.savefig(f"desempenho_fig.png", dpi=fig.dpi)  # salvamos a imagem


if __name__ == "__main__":
    corretude()
    desempenho()
