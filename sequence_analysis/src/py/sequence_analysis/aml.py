def load_ipython_extension(ipython):

    from multipledispatch import dispatch

    import sequence_analysis
    from sequence_analysis import __sequence_analysis

    def Sequences(filename, old_format=False):
        """
        """
        return __sequence_analysis.sequence_analysis.Sequences.ascii_read(filename, old_format)

    sequence_analysis.Sequences = Sequences
    del Sequences

def unload_ipython_extension(ipython):

    import sequence_analysis

    del sequence_analysis.Sequences