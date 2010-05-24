"""LL(1) parser.
"""

import sys
import pygraph
from itertools import chain, ifilter
from ordereddict import OrderedDict
import prettytable

__author__ = "Vladimir Rutsky <altsysrq@gmail.com>"
__copyright__ = "Copyright  2010, Vladimir Rutsky"

# TODO: Rename to ll1parser.py

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

class _WordEnd(Term):
    def __init__(self):
        Term.__init__(self, "$")

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
        if len(self) == 0:
            ex = "Empty productions list."
            raise InvalidProductionsSyntax(ex)
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
            result += format_production(left, rights)
            result += "\n"
        return result
    
    def with_number(self, prod_idx, subprod_idx):
        assert prod_idx < len(self)
        left = self.keys()[prod_idx]
        rights = self[left]
        assert subprod_idx < len(rights)
        return left, rights[subprod_idx]

def format_production(left, rights):
    result = "%s -> "%(left,)
    result += " | ".join(" ".join([str(symb) for symb in right]) for right in rights)
    return result
        
def _productions_graph_edges(prods):
    edges = list()
    edges_set = set()
    for left, rights in prods.iteritems():
        for right in rights:
            for symb in right:
                edge = (left, symb)
                if edge not in edges_set:
                    edges.append(edge)
                    edges_set.add(edge)
    return edges

def productions_graph(prods):
    graph = pygraph.digraph()
    graph.add_nodes(prods.symbols())
    for edge_from, edge_to in _productions_graph_edges(prods):
        graph.add_edge(edge_from, edge_to)
    return graph
    
def _first(prods, cur_symbol, start_symbol):
    if isinstance(cur_symbol, Term):
        # Terminal.
        return [cur_symbol]
    elif cur_symbol == empty_symbol:
        # Empty symbol.
        return [cur_symbol]
    else:
        assert cur_symbol in prods
    
        result = set()
        for right in prods[cur_symbol]:
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
                        firsts = list(_first(prods, symb, start_symbol))
                        result.update(firsts)
                        if empty_symbol not in firsts:
                            # Reach "N -> [e|'t'] ... [e|'t'] 't'".
                            break
                    else:
                        # Left recursion.
                        raise LeftRecursionException("Left recursion for nonterminal %s."%(start_symbol,))
        return result
    
def first(prods, *symbols):
    if len(symbols) == 1:
        # FIRST() for single nonterminal.
        return _first(prods, symbols[0], symbols[0])
    elif len(symbols) >= 2:
        # FIRST("N1 N2 .. NN") is union of FIRST(Ni) for i in 1..K, 
        # where K = argmax for k (e not in FIRST(Ni) for all i=1..k).
        result = set()
        for symbol in symbols:
            firsts = list(_first(prods, symbol, symbol))
            result.update(firsts)
            if empty_symbol not in firsts:
                # Found nonterminal which not produce e.
                break
        return result
    
def _follow(prods, cur_nonterm, start_nonterm):
    result = set()
    if cur_nonterm == prods.start_nonterm():
        # Start nonterm has $ in FOLLOW().
        result.add(word_end)
    # Look all productions for current nonterminal in right part.
    for left, rights in prods.iteritems():
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
                        firsts = set(first(prods, *right[i + 1:]))
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
                            result.update(_follow(prods, left, start_nonterm))
                        else:
                            # Recursion.
                            raise UnsupportedGrammarException("Recursion in FOLLOW(%s)."%(start_nonterm))
    assert empty_symbol not in result
    return result
    
def follow(prods, nonterm):
    return _follow(prods, nonterm, nonterm)

def parse_table(prods):
    """Return dictionary: {(stack_nonterm, input_symbol): (prod_idx, subprod_idx)}."""
    result = dict()
    
    def safe_add(stack_nonterm, input_symbol, prod_idx, subprod_idx):
        assert input_symbol != empty_symbol
        key = (stack_nonterm, input_symbol)
        value = (prod_idx, subprod_idx)
        if key not in result:
            result[key] = value
        else:
            assert result[key] != value
            ex = "Duplicate items for cell (%s, %s): %s"%(
                stack_nonterm, input_symbol, 
                format_production(*tuple(prods.with_number(*result[key]))),
                format_production(*tuple(prods.with_number(*value))))
            raise UnsupportedGrammarException(ex)
    
    for prod_idx, (left, rights) in enumerate(prods.items()):
        for subprod_idx, right in enumerate(rights):
            # Production N -> s1 s2 ...
            firsts = set(first(prods, *right))
            # Add FIRST(N).
            for firsts_prod_term in firsts.difference([empty_symbol]):
                safe_add(left, firsts_prod_term, prod_idx, subprod_idx)
            if empty_symbol in firsts:
                # Add FOLLOW(N).
                follows = set(follow(prods, left))
                for follow_prod_term in follows.difference([empty_symbol]):
                    safe_add(left, follow_prod_term, prod_idx, subprod_idx)
    return result

def format_parse_table(prods, ptable):
    """Return string with grammar parse table rendered in ASCII."""
    input_terms = list(prods.terms()) + [word_end]
    render_table = prettytable.PrettyTable([""] + list(map(str, input_terms)))
    for nonterm in prods.nonterms():
        render_column = [str(nonterm)]
        for input_term in input_terms:
            key = (nonterm, input_term)
            if key in ptable:
                left, right = prods.with_number(*tuple(ptable[key]))
                production_str = format_production(left, [right])
                render_column.append(production_str)
            else:
                render_column.append("")
        render_table.add_row(render_column)
    return str(render_table)
    
class ParserErrorException(Exception):
    def __init__(self, message):
        self.message = message
    def __str__(self):
        return self.message

def parse_word(prods, ptable, input_terms):
    """Parse input terminals list according to productions rules and built 
    parse table.
    Return list of productions indices which produces input terminals list,
    or raises exception, if input syntax isn't correct.
    """
    input_terms = list(input_terms)
    assert len(input_terms) >= 1 and input_terms[-1] == word_end
    
    result = []
    
    stack = [word_end, prods.start_nonterm()]
    input_terms_idx = 0
    while len(stack) > 1:
        stack_top = stack.pop()
        observe_term = input_terms[input_terms_idx]
        if isinstance(stack_top, Term):
            if stack_top == observe_term:
                # Skip terminal.
                input_terms_idx += 1
            else:
                # Unexpected terminal.
                if observe_term == word_end:
                    ex = "Unexpected end of input. "
                else:
                    ex = "Unexpected terminal #%d: %s. "%(input_terms_idx, observe_term)
                ex += "Expected terminal %s."%(stack_top,)
                raise ParserErrorException(ex)
        else:
            assert isinstance(stack_top, NonTerm)
            key = (stack_top, observe_term)
            if key in ptable:
                prod_idx, subprod_idx = ptable[key]
                result.append((prod_idx, subprod_idx))
                left, right = prods.with_number(prod_idx, subprod_idx)
                if right[0] != empty_symbol:
                    stack.extend(reversed(right))
            else:
                ex = "Unexpected terminal #%d: %s."%(input_terms_idx, observe_term)
                firsts = set(first(prods, stack_top))
                expected_terms = firsts.difference([empty_word])
                if empty_word in firsts:
                    follows = set(follow(prods, [stack_top] + list(reversed(stack[1:]))))
                    expected_terms.update(follows)
                ex += "Expected terminals: "
                ex += ", ".join(map(str, expected_terms))
                ex += "."
                raise ParserErrorException(ex)
        
    if input_terms_idx != len(input_terms) - 1:
        ex = "Unexpected terminals starting from #%d: "%(input_terms_idx,)
        ex += ", ".join(map(str, input_terms[input_terms_idx:-1]))
        ex += "."
        raise ParserErrorException(ex)
        
    return result

def format_productions_idxs(prods, prod_idxs):
    result = ""
    for prod_idx, subprod_idx in prod_idxs:
        left, right = prods.with_number(prod_idx, subprod_idx)
        result += format_production(left, [right]) + "\n"
    return result
