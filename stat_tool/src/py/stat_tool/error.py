from functools import wraps

import _stat_tool
import __stat_tool

def wrapper(f):
    @wraps(f)
    def __str__(self):
        return f(self, _stat_tool.__stat_tool.stat_tool.error_type.ERROR)
    return __str__

_stat_tool.__stat_tool.stat_tool.StatError.__str__ = wrapper(_stat_tool.__stat_tool.stat_tool.StatError.ascii_write)