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
        E: [[E,  add,  T], [T]],
        T: [[T,  mult,  F], [F]],
        F: [[lbracket, E,  rbracket],  [var]]
    })

def main(args):
    print "Productions:"
    print prods

    gvv = gv.readstring(pygraph.readwrite.dot.write(prods.graph()))
    #for layout in "circo dot fdp neato nop nop1 nop2 twopi".split():
    for layout in "circo dot fdp neato twopi".split():
        gv.layout(gvv, layout)
        gv.render(gvv, 'png', 'output/input_productions_%s.png'%(layout,))

if __name__ == "__main__":
    main(sys.argv)
