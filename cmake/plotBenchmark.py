import matplotlib.pyplot as plt
import csv
import io

import sys
import os

if len(sys.argv) != 3:
    print("Unexpected number of command line arguments")
    exit()

inFile = os.path.abspath(os.path.join(os.getcwd(), sys.argv[1]))  # csv
outFile = os.path.abspath(os.path.join(os.getcwd(), sys.argv[2]))  # html / svg


class CSV:
    def __init__(self, csvTable, kept) -> None:
        self.header = {name: index for index, name in enumerate(
            csvTable[0]) if name in kept
        }

        self.data = [
            [row[i] for i in range(len(row)) if i in self.header.values()]
            for row in csvTable
        ]

        # update header indices
        self.header = {name: index for index, name in enumerate(self.data[0])}
        self.data = self.data[1:]


class Benchmark:
    def __init__(self, csvTable) -> None:

        # TODO: May break on different benchmark types (ie without Problem Space)

        # csv file ref
        # https://github.com/DigitalInBlue/Celero
        kept = [
            "Group",
            "Experiment",
            "Problem Space",
            "Failure",
            "Baseline",
            "us/Iteration",
            "Iterations/sec"
        ]

        self.csv = CSV(csvTable, kept)
        self.__regroup()

    def toHtml(self, f):
        f.write("""
        <!DOCTYPE html>
        <html>
            <head>
                <style>
                table{
                    margin: auto;
                    border-spacing: 0px;
                }

                th, td {
                    padding: 10px;
                    border: 1px solid black;
                }

                svg {
                    width: 100%;
                    height: 75vh;
                }
                </style>
            </head>
        """)

        f.write("<body>")

        removed = ["Failure", "Group"]
        for group in self.__groups.keys():
            graphs = self.__processGroup(group)
            for g in graphs:
                f.write(f"<div>{g}</div>")
    
            # TODO: Filter by experiment
            f.write("<table>")
            f.write("<tr>")
            for h in self.csv.header.keys():
                if h not in removed:
                    f.write(f"<th>{h}</th>")
            f.write("</tr>")

            for row in self.csv.data:
                f.write("<tr>")
                for h in self.csv.header.keys():
                    if h not in removed:
                        f.write(f"<td>{row[self.csv.header[h]]}</td>")
                f.write("</tr>")

            f.write("</table>")

        f.write("</body>")
        f.write("</html>")

    def __regroup(self):
        self.__groups = {}
        for row in self.csv.data:
            if row[self.csv.header["Failure"]] == 1:
                continue

            name = row[self.csv.header["Group"]]

            if name in self.__groups.keys():
                self.__groups[name].append(row)
            else:
                self.__groups[name] = [row]

    def __processGroup(self, group: str) -> tuple:
        periodGraphSvg = io.StringIO()
        self.__plot(group, "Problem Space", "us/Iteration", periodGraphSvg)
        periodGraphSvg.seek(0)

        frequencyGraphSvg = io.StringIO()
        self.__plot(group, "Problem Space",
                    "Iterations/sec", frequencyGraphSvg)
        frequencyGraphSvg.seek(0)

        baselineGraphSvg = io.StringIO()
        self.__plot(group, "Problem Space",
                    "Baseline", baselineGraphSvg)
        baselineGraphSvg.seek(0)

        return (periodGraphSvg.read(), frequencyGraphSvg.read(), baselineGraphSvg.read())

    def __plot(self, group: str, xKey: str, yKey: str, ioStream) -> None:
        data = self.__groups[group]

        plotLines = {}
        for row in data:
            name = row[self.csv.header["Experiment"]]
            x = int(row[self.csv.header[xKey]])
            y = float(row[self.csv.header[yKey]])

            if name in plotLines.keys():
                plotLines[name][0].append(x)
                plotLines[name][1].append(y)
            else:
                plotLines[name] = ([x], [y])

        fig, ax = plt.subplots()

        for name, (x, y) in plotLines.items():
            ax.plot(x, y, label=name)

        plt.xlabel(xKey)
        plt.ylabel(yKey)
        plt.title(group)

        # Adding legend, which helps us recognize the curve according to it's color
        plt.legend()

        fig.savefig(ioStream, format="svg")


if __name__ == "__main__":

    with open(inFile, "r") as f:
        reader = csv.reader(f)
        bench = Benchmark(list(reader))

    with open(outFile, "w") as f:
        bench.toHtml(f)
