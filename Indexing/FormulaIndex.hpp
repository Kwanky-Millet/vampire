/**
 * @file FormulaIndex.hpp
 * Defines class FormulaIndex.
 */

#ifndef __FormulaIndex__
#define __FormulaIndex__

#include "Forwards.hpp"

#include "Lib/MapToLIFO.hpp"

#include "Shell/AIG.hpp"

#include "Index.hpp"

namespace Indexing {

using namespace Lib;
using namespace Kernel;

/**
 * Results of these indexes may not be complete
 */
class FormulaIndex {
public:
  virtual ~FormulaIndex() {}

  virtual FormulaQueryResultIterator getEquivalent(Formula* f) = 0;

  virtual void insert(FormulaUnit* unit, Formula* f) = 0;
  virtual void remove(FormulaUnit* unit, Formula* f) = 0;
};

class StringFormulaIndex : public FormulaIndex {
public:
  virtual FormulaQueryResultIterator getEquivalent(Formula* f);

  virtual void insert(FormulaUnit* unit, Formula* f);
  virtual void remove(FormulaUnit* unit, Formula* f);
private:
  vstring getKey(Formula* f);

  struct Entry
  {
    Entry(FormulaUnit* unit, Formula* formula) : unit(unit), formula(formula) {}

    bool operator==(const Entry& o) const { return unit==o.unit && formula==o.formula; }
    bool operator!=(const Entry& o) const { return !((*this)==o); }

    FormulaUnit* unit;
    Formula* formula;
  };

  struct Entry2QR;

  MapToLIFO<vstring,Entry> _map;
};

class AIGFormulaIndex : public FormulaIndex {
public:
  virtual FormulaQueryResultIterator getEquivalent(Formula* f);

  virtual void insert(FormulaUnit* unit, Formula* f);
  virtual void remove(FormulaUnit* unit, Formula* f);
private:
  Formula* getKey(Formula* f);

  struct Entry
  {
    Entry(FormulaUnit* unit, Formula* formula) : unit(unit), formula(formula) {}

    bool operator==(const Entry& o) const { return unit==o.unit && formula==o.formula; }
    bool operator!=(const Entry& o) const { return !((*this)==o); }

    FormulaUnit* unit;
    Formula* formula;
  };

  struct Entry2QR;

  AIGFormulaSharer _aig;
  MapToLIFO<Formula*,Entry> _map;
};


}

#endif // __FormulaIndex__
