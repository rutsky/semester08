"""LL(1) parser.
"""

import sys
import pygraph
from itertools import chain

__author__ = "Vladimir Rutsky <altsysrq@gmail.com>"
__copyright__ = "Copyright  2006, Vladimir Rutsky"

class Symbol(object):
    pass

class Term(Symbol):
    def __init__(self, name):
        self.name = name
    def __str__(self):
        return '\'' + str(self.name) + '\''

class NonTerm(Symbol):
    def __init__(self, name):
        self.name = name
    def __str__(self):
        return str(self.name)

class _EmptySymbol(Symbol):
    def __str__(self):
        return "e"

class _WordEnd(Symbol):
    def __str__(self):
        return "$"

empty_symbol = _EmptySymbol()
word_end = _WordEnd()
    
class UnsupportedGrammarException(Exception):
    def __init__(self, message):
        self.message = message
    def __str__(self):
        return self.message
    
class LeftRecursionException(UnsupportedGrammarException):
    def __init__(self, message):
        UnsupportedGrammarException(self, message)

class Productions(dict):
    def __init__(self, start_nonterm, productions):
        dict.__init__(self, productions)
        self.start_nonterm = start_nonterm
    
    def symbols(self):
        return set(chain(self.keys(), chain(*list(chain(*self.values())))))
    
    def terms(self):
        return filter(lambda symb: isinstance(symb, Term), self.symbols())
        
    def nonterms(self):
        return filter(lambda symb: isinstance(symb, NonTerm), self.symbols())
    
    def __str__(self):
        result = ""
        for left, rights in self.iteritems():
            result += "%s -> "%(left,)
            result += " | ".join(" ".join([str(symb) for symb in right]) for right in rights)
            result += "\n"
        return result
        
    def _graph_edges(self):
        edges = list()
        edges_set = set()
        for left, rights in self.iteritems():
            for right in rights:
                for symb in right:
                    edge = (left, symb)
                    if edge not in edges_set:
                        edges.append(edge)
                        edges_set.add(edge)
        return edges
    
    def graph(self):
        graph = pygraph.digraph()
        graph.add_nodes(self.symbols())
        for edge_from, edge_to in self._graph_edges():
            graph.add_edge(edge_from, edge_to)
        return graph
    
    def _first(self, cur_nonterm, start_nonterm):
        result = set()
        for right in self[nonterm]:
            assert len(right) >= 1
            symb = right[0] # Production like "N -> " is incorrect.
            if isinstance(symb, Term):
                # N -> 't' ... - add 't' to result.
                result.add(symb)
            elif symb == empty_symbol:
                # N -> e - add e to result.
                assert len(right) == 1 # Production like "N -> e..." is incorrect.
                result.add(symb)
            else:
                # N -> N1
                assert isinstance(symb, NonTerm)
                # Look for first nonterminal which will not produce e,
                # meantime all previous symbols FIRST's are addded to result.
                for symb in right:
                    if symb != start_nonterm:
                        # TODO: Cache.
                        firsts = list(self._first(symb, start_nonterm))
                        result.update(firsts)
                        if empty_symbol not in firsts:
                            # Reach "N -> [e|'t'] ... [e|'t'] 't'".
                            break
                    else:
                        # Left recursion.
                        raise LeftRecursionException("Left recursion for nonterminal %s."%(start_nonterm,))
        return result
        
    def first(self, *nonterms):
        assert nonterm in self
        if len(nonterms) == 1:
            # FIRST() for single nonterminal.
            return self._first(nonterm, nonterm)
        elif len(nonterms) >= 2:
            # FIRST("N1 N2 .. NN") is union of FIRST(Ni) for i in 1..K, 
            # where K = argmax for k (e not in FIRST(Ni) for all i=1..k).
            result = set()
            for nonterm in nonterms:
                firsts = list(self._first(nonterm, nonterm))
                result.update(firsts)
                if empty_symbol not in firsts:
                    # Found nonterminal which not produce e.
                    break
            return result
    
    def _follow(self, cur_nonterm, start_nonterm):
        result = set()
        if nonterm == self.start_nonterm:
            # Start nonterm has $ in FOLLOW().
            result.add(word_end)
        # Look all productions for current nonterminal in right part.
        for left, rights in self.iteritems():
            for right in rights:
                for i, symb in enumerate(right):
                    if symb == cur_nonterm:
                        # Found rule and place where current nonterminal
                        # occures in right part.
                        if i + 1 < len(right):
                            # N -> ... X S1 ... SK
                            firsts = set(self.first(right[i + 1:]))
                            firsts_not_e = firsts.difference([empty_symbol])
                        else:
                            # N -> ... X
                            firsts = []
                            firsts_not_e = []
                            
                        if len(firsts_not_e) >= 1:
                            # Add FIRST(S1 ... SK) \ {e} to FOLLOW(X).
                            result.update(firsts_not_e)
                        # TODO: On well composed LL(1) grammar "firsts" can contain
                        # either e or non-e's.
                        if empty_word in firsts or len(firsts) == 0:
                            # Add FOLLOW(N) to FOLLOW(X).
                            if left != start_nonterm:
                                result.update(self._follow(left, start_nonterm))
                            else:
                                # Recursion.
                                raise UnsupportedGrammarException("Recursion in FOLLOW(%s)"%(start_nonterm))
        assert empty_word not in result
        return result
        
    def follow(self, nonterm):
        return self._follow(nonterm, nonterm)
