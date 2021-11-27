import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
from textwrap import wrap

""" How to use this file:
python3.7 plotter.py
"""

#Plot "mode" on the y axis and  number of threads on the x axis.
def graph(xvals, list_of_yvals, list_of_names, list_of_colors,
 filename, title, xlabel, ylabel):
  """ Creates a graph with the given inputs using Matplotlib.
  Parameters:
    xvals -- a list of values that represent the x values for the input data.

    list_of_yvals -- a list of lists. Each such list corresponds to a line drawn
    in the graph.
    precondition: len(xvals) = len(list_of_yvals[i]) for every i.

    list_of_names -- a list of strings that name each of the lines in the graph.
    precondition: len(list_of_yvals) = len(list_of_names).

    list_of_colors -- a list of colors that should be used to draw each line of
    the graph.
    precondition: len(list_of_colors) == len(list_of_yvals)

    filename -- the name of the png file where the image should be saved.

    title -- the title of the graph. It will be wrapped in 60 characters per
    line.

    xlabel, ylabel -- the name of the x/y axis, respectively.

  Example usage:
    graph([1,2,3,4], [[2,4,6,8], [1,2,3,4]], ["first_line", "second_line"],
      ["red", "blue"], "test.png", "MyTitle", "my_xlabel", "my_ylabel")
  """

  for yvals in list_of_yvals:
    assert(len(xvals) == len(yvals))
  assert(len(list_of_yvals) == len(list_of_names))
  assert(len(list_of_yvals) == len(list_of_colors))

  legends = []
  for i in range(len(list_of_names)):
    patch = mpatches.Patch(color=list_of_colors[i], label=list_of_names[i])
    legends += [patch]

  plt.clf()
  xvals = np.array(xvals, dtype=np.int)
  for i in range(len(list_of_yvals)):
      plt.plot(xvals.astype('str'), list_of_yvals[i], list_of_colors[i])

  # plt.xticks(xvals, xvals)
  plt.legend(handles=legends)
  plt.xlabel(xlabel)
  plt.ylabel(ylabel)
  plt.subplots_adjust(top=0.85)
  plt.title(wrap(title, 60), y = 1.08)
  plt.savefig(filename, bbox_inches='tight')


