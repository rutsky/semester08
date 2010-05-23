"""LL(1) parser.
"""

import sys
import pydot

from llparser import *

__author__ = "Vladimir Rutsky <altsysrq@gmail.com>"
__copyright__ = "Copyright  2006, Vladimir Rutsky"

E = Term('E')
T = Term('T')
F = Term('F')
terms = [E, T, F]

var = NonTerm('a')
add = NonTerm('+')
mult = NonTerm('*')
lbracket = NonTerm('(')
rbracket = NonTerm(')')
nonterms = [lbracket, rbracket, var]

productions = {
    E: [[E,  add,  T], [T]],
    T: [[T,  mult,  F], [F]],
    F: [[lbracket, E,  rbracket],  [var]]
    }

def format_productins(productions):
    result = ""
    for left, rights in productions.iteritems():
        result += "%s -> "%(left.name)
        result += " | ".join(" ".join([symb.name for symb in right]) for right in rights)
        result += "\n"
    return result

def productions_edges(productions):
    edges = set()
    for left, rights in productions.iteritems():
        for right in rights:
            for symb in right:
                edges.add((left.name, symb.name))
    return edges

def main(args):
    print "Productions:"
    print format_productins(productions)
    pydot.graph_from_edges(productions_edges(productions), directed=True).write_ps("productions.ps")

if __name__ == "__main__":
    main(sys.argv)
