"""LL(1) parser.
"""

import sys

__author__ = "Vladimir Rutsky <altsysrq@gmail.com>"
__copyright__ = "Copyright  2006, Vladimir Rutsky"

class Term(object):
    def __init__(self, name):
        self.name = name
    def __str__(self):
        return '\'' + str(self.name) + '\''

class NonTerm():
    def __init__(self, name):
        self.name = name
