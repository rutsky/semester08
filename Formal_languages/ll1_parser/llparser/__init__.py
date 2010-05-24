"""LL(1) parser.
"""

import sys
import pygraph
from itertools import chain, ifilter
from ordereddict import OrderedDict

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
        UnsupportedGrammarException.__init__(self, message)

class InvalidProductionsSyntax(Exception):
    def __init__(self, message):
        self.message = message
    def __str__(self):
        return self.message

class Productions(OrderedDict):
    def check(self):
        """Check is productions is syntaxically valid."""
        # TODO: Not all cases handled.
        lefts = set(self.keys())
        left_nonterms = list(filter(lambda s: not isinstance(s, NonTerm), lefts))
        if len(left_nonterms) != 0:
            ex = "Left part has not non-terminals: "%(", ".join(map(str, left_nonterms)),)
            raise InvalidProductionsSyntax(ex)
        rights = set(chain(*list(chain(*list(self.values())))))
        right_nonsymbs = list(filter(lambda s: not isinstance(s, (Term, NonTerm)) and not s == empty_symbol, rights))
        if len(right_nonsymbs) != 0:
            ex = "Right part has something that is not terminal, or non-terminal, or e: %s."%(
                ", ".join(map(str, right_nonsymbs)),)
            raise InvalidProductionsSyntax(ex)
        rights_nonterms = set(filter(lambda s: isinstance(s, NonTerm), rights))
        only_rights_nonterms = rights_nonterms.difference(lefts)
        if len(only_rights_nonterms) != 0:
            ex = "Right part has non-terminals which are not in left part: "
            ex += ", ".join(map(str, only_rights_nonterms))
            ex += "."
            raise InvalidProductionsSyntax(ex)
        for left, rights in self.iteritems():
            for right in rights:
                if len(right) == 0:
                    ex = "Empty production: %s."%(left,)
                    raise InvalidProductionsSyntax(ex)
                if len(right) >= 2 and next(ifilter(lambda s: s == empty_symbol, right), None) is not None:
                    ex = "Production right part has e and some symbols: %s -> "%(left,)
                    ex += " ".join(map(str, right))
                    raise InvalidProductionsSyntax(ex)
        return True
    
    def symbols(self):
        return set(chain(*list(chain(*self.values()))))
    
    def terms(self):
        return filter(lambda symb: isinstance(symb, Term), self.symbols())
        
    def nonterms(self):
        return self.keys()
        
    def start_nonterm(self):
        return next(iter(self))
    
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
    
    def _first(self, cur_symbol, start_symbol):
        if isinstance(cur_symbol, Term):
            # Terminal.
            return [cur_symbol]
        elif cur_symbol == empty_symbol:
            # Empty symbol.
            return [cur_symbol]
        else:
            assert cur_symbol in self
        
            result = set()
            for right in self[cur_symbol]:
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
                        if symb != start_symbol:
                            # TODO: Cache.
                            firsts = list(self._first(symb, start_symbol))
                            result.update(firsts)
                            if empty_symbol not in firsts:
                                # Reach "N -> [e|'t'] ... [e|'t'] 't'".
                                break
                        else:
                            # Left recursion.
                            raise LeftRecursionException("Left recursion for nonterminal %s."%(start_symbol,))
            return result
        
    def first(self, *symbols):
        if len(symbols) == 1:
            # FIRST() for single nonterminal.
            return self._first(symbols[0], symbols[0])
        elif len(symbols) >= 2:
            # FIRST("N1 N2 .. NN") is union of FIRST(Ni) for i in 1..K, 
            # where K = argmax for k (e not in FIRST(Ni) for all i=1..k).
            result = set()
            for symbol in symbols:
                firsts = list(self._first(symbol, symbol))
                result.update(firsts)
                if empty_symbol not in firsts:
                    # Found nonterminal which not produce e.
                    break
            return result
    
    def _follow(self, cur_nonterm, start_nonterm):
        result = set()
        if cur_nonterm == self.start_nonterm():
            # Start nonterm has $ in FOLLOW().
            result.add(word_end)
        # Look all productions for current nonterminal in right part.
        for left, rights in self.iteritems():
            if left == cur_nonterm:
                # Skip production "N -> ... N ..." for FOLLOW(N).
                continue
            for right in rights:
                for i, symb in enumerate(right):
                    if symb == cur_nonterm:
                        # Found rule and place where current nonterminal
                        # occures in right part.
                        if i + 1 < len(right):
                            # N -> ... X S1 ... SK
                            firsts = set(self.first(*right[i + 1:]))
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
                        if empty_symbol in firsts or len(firsts) == 0:
                            # Add FOLLOW(N) to FOLLOW(X).
                            if left != start_nonterm:
                                result.update(self._follow(left, start_nonterm))
                            else:
                                # Recursion.
                                raise UnsupportedGrammarException("Recursion in FOLLOW(%s)."%(start_nonterm))
        assert empty_symbol not in result
        return result
        
    def follow(self, nonterm):
        return self._follow(nonterm, nonterm)
