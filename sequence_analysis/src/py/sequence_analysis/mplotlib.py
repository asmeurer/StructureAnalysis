from . import __sequence_analysis
from stat_tool.mplotlib import get_plotables

def load_ipython_extension(ipython):

    get_plotables(__sequence_analysis.sequence_analysis)

def unload_ipython_extension(ipython):

    pass
