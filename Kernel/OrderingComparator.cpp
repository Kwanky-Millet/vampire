/*
 * This file is part of the source code of the software program
 * Vampire. It is protected by applicable
 * copyright laws.
 *
 * This source code is distributed under the licence found here
 * https://vprover.github.io/license.html
 * and in the source directory
 */
/**
 * @file OrderingComparator.cpp
 * Implements class OrderingComparator.
 */

#include "Lib/Stack.hpp"
#include "Lib/Environment.hpp"

#include "KBO.hpp"
#include "SubstHelper.hpp"

#include "OrderingComparator.hpp"

using namespace std;

namespace Kernel {

std::ostream& operator<<(std::ostream& out, const OrderingComparator::BranchTag& t)
{
  switch (t) {
    case OrderingComparator::BranchTag::T_RESULT:
      out << "r";
      break;
    case OrderingComparator::BranchTag::T_COMPARISON:
      out << "c";
      break;
    case OrderingComparator::BranchTag::T_WEIGHT:
      out << "w";
      break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const OrderingComparator::Node& node)
{
  out << (OrderingComparator::BranchTag)node.tag << (node.ready?" ":"? ");
  switch (node.tag) {
    case OrderingComparator::BranchTag::T_RESULT: {
      out << node.data;
      break;
    }
    case OrderingComparator::BranchTag::T_WEIGHT: {
      out << node.w << " ";
      for (const auto& [var, coeff] : *node.varCoeffPairs) {
        out << "X" << var << " " << coeff << " ";
      }
      break;
    }
    case OrderingComparator::BranchTag::T_COMPARISON: {
      out << node.lhs << " " << node.rhs;
      break;
    }
  }
  return out;
}

std::ostream& operator<<(std::ostream& str, const OrderingComparator& comp)
{
  Stack<std::pair<const OrderingComparator::Branch*, unsigned>> todo;
  todo.push(std::make_pair(&comp._root,0));
  // Note: using this set we get a more compact representation
  DHSet<OrderingComparator::Node*> seen;

  while (todo.isNonEmpty()) {
    auto kv = todo.pop();
    for (unsigned i = 0; i < kv.second; i++) {
      if (i+1 == kv.second) {
        str << "  |--";
      } else {
        str << "  |  ";
      }
    }
    str << *kv.first->node() << std::endl;
    if (seen.insert(kv.first->node())) {
      if (kv.first->node()->tag==OrderingComparator::BranchTag::T_RESULT) {
        if (kv.first->node()->data) {
          todo.push(std::make_pair(&kv.first->node()->alternative,kv.second+1));
        }
      } else {
        todo.push(std::make_pair(&kv.first->node()->incBranch,kv.second+1));
        todo.push(std::make_pair(&kv.first->node()->gtBranch,kv.second+1));
        todo.push(std::make_pair(&kv.first->node()->eqBranch,kv.second+1));
      }
    }
  }
  return str;
}

OrderingComparator::OrderingComparator(const Ordering& ord, const Stack<Ordering::Constraint>& comps, void* result)
: _ord(ord), _root(), _fail(nullptr, Branch()), _curr(&_root)
{
  ASS(result);
  static Ordering::Result ordVals[] = { Ordering::EQUAL, Ordering::GREATER, Ordering::INCOMPARABLE };
  auto curr = &_root;
  for (const auto& [lhs,rhs,rel] : comps) {
    *curr = OrderingComparator::Branch(lhs, rhs);
    for (unsigned i = 0; i < 3; i++) {
      if (ordVals[i] != comps[0].rel) {
        curr->node()->getBranch(ordVals[i]) = _fail;
      }
    }
    curr = &curr->node()->getBranch(rel);
  }
  *curr = Branch(result, _fail);
  _fail.node()->ready = true;
}

OrderingComparator::~OrderingComparator() = default;

void* OrderingComparator::next(const SubstApplicator* applicator)
{
  ASS(_curr);
  for (;;) {
    expand();
    auto node = _curr->node();
    ASS(node->ready);

    if (node->tag == BranchTag::T_RESULT) {
      if (!node->data) {
        return nullptr;
      }
      _curr = &node->alternative;
      return node->data;
    }

    Ordering::Result comp = Ordering::INCOMPARABLE;
    if (node->tag == BranchTag::T_COMPARISON) {

      comp = _ord.isGreaterOrEq(AppliedTerm(node->lhs,applicator,true),AppliedTerm(node->rhs,applicator,true));
      _cache.push({ node->lhs, node->rhs, comp });

    } else {
      ASS(node->tag == BranchTag::T_WEIGHT);

      const auto& kbo = static_cast<const KBO&>(_ord);
      auto weight = node->w;
      ZIArray<int> varDiffs;
      for (const auto& [var, coeff] : *node->varCoeffPairs) {
        AppliedTerm tt(TermList::var(var), applicator, true);

        VariableIterator vit(tt.term);
        while (vit.hasNext()) {
          auto v = vit.next();
          varDiffs[v.var()] += coeff;
          // since the counts are sorted in descending order,
          // this can only mean we will fail
          if (varDiffs[v.var()]<0) {
            goto loop_end;
          }
        }
        int64_t w = kbo.computeWeight(tt);
        weight += coeff*w;
        // due to descending order of counts,
        // this also means failure
        if (coeff<0 && weight<0) {
          goto loop_end;
        }
      }

      if (weight > 0) {
        comp = Ordering::GREATER;
      } else if (weight == 0) {
        comp = Ordering::EQUAL;
      }
    }
loop_end:
    _curr = &node->getBranch(comp);
  }
  return nullptr;
}

void OrderingComparator::insert(const Stack<Ordering::Constraint>& comps, void* result)
{
  ASS(result);
  static Ordering::Result ordVals[] = { Ordering::EQUAL, Ordering::GREATER, Ordering::INCOMPARABLE };
  // we mutate current fail node and add a new one
  auto curr = &_fail;
  Branch newFail(nullptr, Branch());
  newFail.node()->ready = true;

  curr->node()->~Node();
  curr->node()->ready = false;

  if (comps.isNonEmpty()) {
    curr->node()->tag = T_COMPARISON;
    curr->node()->rhs = comps[0].rhs;
    curr->node()->lhs = comps[0].lhs;
    for (unsigned i = 0; i < 3; i++) {
      if (ordVals[i] != comps[0].rel) {
        curr->node()->getBranch(ordVals[i]) = newFail;
      }
    }
    curr = &curr->node()->getBranch(comps[0].rel);
    for (unsigned i = 1; i < comps.size(); i++) {
      auto [lhs,rhs,rel] = comps[i];
      *curr = OrderingComparator::Branch(lhs, rhs);
      for (unsigned i = 0; i < 3; i++) {
        if (ordVals[i] != rel) {
          curr->node()->getBranch(ordVals[i]) = newFail;
        }
      }
      curr = &curr->node()->getBranch(rel);
    }
    *curr = Branch(result, newFail);
  } else {
    curr->node()->tag = T_RESULT;
    curr->node()->data = result;
    curr->node()->alternative = newFail;
  }

  _fail = newFail;
}

void OrderingComparator::expand()
{
  ASS(_curr->node());
  while (_curr->node() && !_curr->node()->ready)
  {
    // take temporary ownership of node
    Branch nodeHolder = *_curr;
    auto node = nodeHolder.node();

    if (node->tag == BranchTag::T_RESULT) {
      *_curr = Branch(node->data, node->alternative);
      _curr->node()->ready = true;
      return;
    }
    ASS(node->tag != BranchTag::T_WEIGHT);

    // Use compare here to filter out as many
    // precomputable comparisons as possible.
    auto comp = _ord.compare(node->lhs,node->rhs);
    if (comp != Ordering::INCOMPARABLE) {
      if (comp == Ordering::LESS) {
        *_curr = node->incBranch;
      } else if (comp == Ordering::GREATER) {
        *_curr = node->gtBranch;
      } else {
        *_curr = node->eqBranch;
      }
      continue;
    }
    // If we have a variable, we cannot preprocess further.
    if (tryExpandVarCase()) {
      continue;
    }

    expandTermCase();
  }
}

void OrderingComparator::expandTermCase()
{
  ASS(_curr->node() && !_curr->node()->ready);
  _curr->node()->ready = true;
}

bool OrderingComparator::tryExpandVarCase()
{
  // take temporary ownership of node
  Branch nodeHolder = *_curr;
  auto node = nodeHolder.node();

  // If we have a variable, we cannot preprocess further.
  if (!node->lhs.isVar() && !node->rhs.isVar()) {
    return false;
  }
  // try cache
  for (const auto& [s,t,r] : _cache) {
    if (s == node->lhs && t == node->rhs) {
      if (r == Ordering::GREATER) {
        *_curr = node->gtBranch;
      } else if (r == Ordering::EQUAL) {
        *_curr = node->eqBranch;
      } else {
        ASS_EQ(r, Ordering::INCOMPARABLE);
        *_curr = node->incBranch;
      }
      return true;
    }

    if (s == node->rhs && t == node->lhs && r != Ordering::INCOMPARABLE) {
      if (r == Ordering::GREATER) {
        *_curr = node->incBranch;
      } else {
        ASS_EQ(r, Ordering::EQUAL);
        *_curr = node->eqBranch;
      }
      // Note: since we use isGreater which results
      // in INCOMPARABLE when compare would be LESS,
      // the INCOMPARABLE result we cannot use here
      return true;
    }
  }
  // make a fresh copy
  *_curr = Branch(node->lhs, node->rhs);
  _curr->node()->eqBranch = node->eqBranch;
  _curr->node()->gtBranch = node->gtBranch;
  _curr->node()->incBranch = node->incBranch;
  _curr->node()->ready = true;
  return true;
}

OrderingComparator::Branch::~Branch()
{
  setNode(nullptr);
}

OrderingComparator::Branch::Branch(const Branch& other)
{
  setNode(other.node());
}

OrderingComparator::Branch& OrderingComparator::Branch::operator=(const Branch& other)
{
  if (&other==this) {
    return *this;
  }
  setNode(other.node());
  return *this;
}

OrderingComparator::Branch::Branch(Branch&& other)
  : _node(other._node)
{
  other._node = nullptr;
}

OrderingComparator::Branch& OrderingComparator::Branch::operator=(Branch&& other)
{
  if (&other==this) {
    return *this;
  }
  _node = other._node;
  other._node = nullptr;
  return *this;
}

OrderingComparator::Node::~Node()
{
  switch(tag) {
    case T_RESULT:
      alternative.~Branch();
      break;
    case T_COMPARISON:
      break;
    case T_WEIGHT:
      delete varCoeffPairs;
      break;
  }
}

void OrderingComparator::Node::incRefCnt()
{
  refcnt++;
}

void OrderingComparator::Node::decRefCnt()
{
  ASS(refcnt);
  refcnt--;
  if (!refcnt) {
    delete this;
  }
}

}