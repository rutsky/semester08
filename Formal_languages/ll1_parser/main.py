"""LL(1) parser.
"""

import sys
import pydot

from llparser import *

__author__ = "Vladimir Rutsky <altsysrq@gmail.com>"
__copyright__ = "Copyright  2006, Vladimir Rutsky"

E = NonTerm('E')
T = NonTerm('T')
F = NonTerm('F')
nonterms = [E, T, F]

var = Term('a')
add = Term('+')
mult = Term('*')
lbracket = Term('(')
rbracket = Term(')')
terms = [lbracket, rbracket, var]

productions = {
    E: [[E,  add,  T], [T]],
    T: [[T,  mult,  F], [F]],
    F: [[lbracket, E,  rbracket],  [var]]
    }

def format_productins(productions):
    result = ""
    for left, rights in productions.iteritems():
        result += "%s -> "%(left,)
        result += " | ".join(" ".join([str(symb) for symb in right]) for right in rights)
        result += "\n"
    return result

def productions_edges(productions):
    edges = list()
    edges_set = set()
    for left, rights in productions.iteritems():
        for right in rights:
            for symb in right:
                edge = (str(left), str(symb))
                if edge not in edges_set:
                    edges.append(edge)
                    edges_set.add(edge)
    return edges

def main(args):
    print "Productions:"
    print format_productins(productions)
    pydot.graph_from_edges(productions_edges(productions), directed=True).write_ps("productions.ps")

if __name__ == "__main__":
    main(sys.argv)
