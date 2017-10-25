from functools import wraps

from stat_tool import _stat_tool
from stat_tool import __stat_tool

from . import _sequence_analysis
from . import __sequence_analysis

def wrapper(f):
    @wraps(f)
    def ascii_read(filename, old_format = False):
        error = __stat_tool.stat_tool.StatError(__stat_tool.stat_tool.nb_error)
        data = f(error, filename, old_format)
        if not data:
            raise Exception(str(error))
        return data
    return ascii_read

__sequence_analysis.sequence_analysis.Sequences.ascii_read = staticmethod(wrapper(__sequence_analysis.sequence_analysis.Sequences.ascii_read))
del wrapper

def wrapper(f):
    @wraps(f)
    def segmentation(self, identifier, max_nb_segment, model_type, criterion, *args, **kwargs):
        error = __stat_tool.stat_tool.StatError(__stat_tool.stat_tool.nb_error)
        criterion = __stat_tool.stat_tool.model_selection_criterion.names[criterion]
        if model_type == 'NEGATIVE_BINOMIAL':
            shape_parameter = kwargs.pop('shape_parameter')
        else:
            shape_parameter = []
        if criterion == 'LIKELIHOOD_SLOPE':
            min_nb_segment = kwargs.pop('min_nb_segment')
        else:
            min_nb_segment = 0
        if not isinstance(model_type, (list, tuple)):
            model_type = [model_type]
        model_type = [__sequence_analysis.sequence_analysis.segment_model.names[model_type + "_CHANGE"] for model_type in model_type]
        output = kwargs.pop('output', "SEQUENCE")
        output = __sequence_analysis.sequence_analysis.sequence_type.names[output]
        res = f(self, error,
                kwargs.pop('display', True),
                identifier, 
                max_nb_segment, 
                model_type, 
                kwargs.pop('common_contrast', False), 
                shape_parameter, 
                criterion, 
                min_nb_segment, 
                kwargs.pop('penalty_shape_type', 2), 
                output)
        if error.get_nb_error() > 0:
            raise Exception(str(error))
        return res
    return segmentation

__sequence_analysis.sequence_analysis.Sequences.segmentation = wrapper(__sequence_analysis.sequence_analysis.Sequences.segmentation)
del wrapper

def wrapper(f):
    @wraps(f)
    def segment_profile(self, identifier, nb_segment, model_type, *args, **kwargs):
        error = __stat_tool.stat_tool.StatError(__stat_tool.stat_tool.nb_error)
        if model_type == 'NEGATIVE_BINOMIAL':
            shape_parameter = kwargs.pop('shape_parameter')
        else:
            shape_parameter = []
        if not isinstance(model_type, (list, tuple)):
            model_type = [model_type]
        model_type = [__sequence_analysis.sequence_analysis.segment_model.names[model_type + "_CHANGE"] for model_type in model_type]
        output = kwargs.pop('output', "SEGMENT")
        output = __sequence_analysis.sequence_analysis.change_point_profile.names[output]
        res = f(self, error,
                identifier, 
                nb_segment, 
                model_type, 
                kwargs.pop('common_contrast', False), 
                shape_parameter, 
                output)
        if error.get_nb_error() > 0:
            raise Exception(str(error))
        return res
    return segment_profile

__sequence_analysis.sequence_analysis.Sequences.segment_profile = wrapper(__sequence_analysis.sequence_analysis.Sequences.segment_profile_plotable_write)
del wrapper, __sequence_analysis.sequence_analysis.Sequences.segment_profile_plotable_write
