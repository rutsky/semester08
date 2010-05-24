"""LL(1) parser.
"""

import sys
import pygraph

# TODO: Workaround for "ImportError: No module named _gv"
sys.path.append('/usr/lib/pyshared/python2.6')
import gv

from llparser import *

__author__ = "Vladimir Rutsky <altsysrq@gmail.com>"
__copyright__ = "Copyright  2006, Vladimir Rutsky"

"""
E = NonTerm('E')
T = NonTerm('T')
F = NonTerm('F')

var = Term('a')
add = Term('+')
mult = Term('*')
lbracket = Term('(')
rbracket = Term(')')

prods = Productions(E, 
    {
        E: [[E, add, T], [T]],
        T: [[T, mult, F], [F]],
        F: [[lbracket, E, rbracket], [var]]
    })
"""

E = NonTerm('E')
E1 = NonTerm('E1')
T = NonTerm('T')
T1 = NonTerm('T1')
F = NonTerm('F')

var = Term('a')
add = Term('+')
mult = Term('*')
lbracket = Term('(')
rbracket = Term(')')

prods = Productions([ 
        (E, [[T, E1]]),
        (E1, [[add, T, E1], [empty_symbol]]),
        (T, [[F, T1]]),
        (T1, [[mult, F, T1], [empty_symbol]]),
        (F, [[lbracket, E,  rbracket],  [var]])
    ])

def main(args):
    print "Productions:"
    print prods
    prods.check()

    if False:
        gvv = gv.readstring(pygraph.readwrite.dot.write(prods.graph()))
        #for layout in "circo dot fdp neato nop nop1 nop2 twopi".split():
        for layout in "circo dot fdp neato twopi".split():
            gv.layout(gvv, layout)
            gv.render(gvv, 'png', 'output/input_productions_%s.png'%(layout,))
    print "Non-terminals: %s."%(', '.join(map(str, prods.nonterms())))
    print "Terminals: %s."%(', '.join(map(str, prods.terms())))
    for nonterm in prods.nonterms():
        print "FIRST(%s) = {  %s  }."%(nonterm, ', '.join(map(str, prods.first(nonterm))))
    for nonterm in prods.nonterms():
        print "FOLLOW(%s) = {  %s  }."%(nonterm, ', '.join(map(str, prods.follow(nonterm))))

if __name__ == "__main__":
    main(sys.argv)
