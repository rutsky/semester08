"""LL(1) parser.
"""

import sys
import pygraph

# TODO: Workaround for "ImportError: No module named _gv"
sys.path.append('/usr/lib/pyshared/python2.6')
import gv

from ll1parser import *

__author__ = "Vladimir Rutsky <altsysrq@gmail.com>"
__copyright__ = "Copyright  2010, Vladimir Rutsky"

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

    gvv = gv.readstring(pygraph.readwrite.dot.write(productions_graph(prods)))
    gv.layout(gvv, "dot")
    gv.render(gvv, "svg", "input_productions.svg")
    if False:
        #for layout in "circo dot fdp neato nop nop1 nop2 twopi".split():
        for layout in "circo dot fdp neato twopi".split():
            gv.layout(gvv, layout)
            gv.render(gvv, "png", "input_productions_%s.png"%(layout,))
    print "Non-terminals: %s."%(', '.join(map(str, prods.nonterms())))
    print "Terminals: %s."%(', '.join(map(str, prods.terms())))
    
    for nonterm in prods.nonterms():
        print "FIRST(%s) = {%s}."%(nonterm, ', '.join(map(str, first(prods, nonterm))))
    for nonterm in prods.nonterms():
        print "FOLLOW(%s) = {%s}."%(nonterm, ', '.join(map(str, follow(prods, nonterm))))
        
    ptable = parse_table(prods)
    print "Grammar parse table:"
    print format_parse_table(prods, ptable)
    
    input_terms = [var, add, var, mult, var, word_end]
    print "Input terminals: %s."%(", ".join(map(str, input_terms)),)
    
    prod_idxs = parse_word(prods, ptable, input_terms)
    
    print "Productions that generates input:"
    print format_productions_idxs(prods, prod_idxs)

    prods_dot = productions_idxs_dot_graph(prods, prod_idxs)
    #with open("syntax_tree.dot", "w") as f:
    #    f.write(prods_dot)
    gvv = gv.readstring(prods_dot)
    gv.layout(gvv, "dot")
    gv.render(gvv, "svg", "syntax_tree.svg")

if __name__ == "__main__":
    main(sys.argv)
