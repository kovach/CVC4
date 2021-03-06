/*********************                                                        */
/*! \file infer_info.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Andrew Reynolds
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2019 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief Inference information utility
 **/

#include "cvc4_private.h"

#ifndef CVC4__THEORY__STRINGS__INFER_INFO_H
#define CVC4__THEORY__STRINGS__INFER_INFO_H

#include <map>
#include <vector>
#include "expr/node.h"

namespace CVC4 {
namespace theory {
namespace strings {

/** Types of inferences used in the procedure
 *
 * These are variants of the inference rules in Figures 3-5 of Liang et al.
 * "A DPLL(T) Solver for a Theory of Strings and Regular Expressions", CAV 2014.
 */
enum Inference
{
  INFER_NONE = 0,
  // infer empty, for example:
  //     (~) x = ""
  // This is inferred when we encounter an x such that x = "" rewrites to a
  // constant. This inference is used for instance when we otherwise would have
  // split on the emptiness of x but the rewriter tells us the emptiness of x
  // can be inferred.
  INFER_INFER_EMP = 1,
  // string split constant propagation, for example:
  //     x = y, x = "abc", y = y1 ++ "b" ++ y2
  //       implies y1 = "a" ++ y1'
  INFER_SSPLIT_CST_PROP,
  // string split variable propagation, for example:
  //     x = y, x = x1 ++ x2, y = y1 ++ y2, len( x1 ) >= len( y1 )
  //       implies x1 = y1 ++ x1'
  // This is inspired by Zheng et al CAV 2015.
  INFER_SSPLIT_VAR_PROP,
  // length split, for example:
  //     len( x1 ) = len( y1 ) V len( x1 ) != len( y1 )
  // This is inferred when e.g. x = y, x = x1 ++ x2, y = y1 ++ y2.
  INFER_LEN_SPLIT,
  // length split empty, for example:
  //     z = "" V z != ""
  // This is inferred when, e.g. x = y, x = z ++ x1, y = y1 ++ z
  INFER_LEN_SPLIT_EMP,
  // string split constant binary, for example:
  //     x1 = "aaaa" ++ x1' V "aaaa" = x1 ++ x1'
  // This is inferred when, e.g. x = y, x = x1 ++ x2, y = "aaaaaaaa" ++ y2.
  // This inference is disabled by default and is enabled by stringBinaryCsp().
  INFER_SSPLIT_CST_BINARY,
  // string split constant
  //    x = y, x = "c" ++ x2, y = y1 ++ y2, y1 != ""
  //      implies y1 = "c" ++ y1'
  // This is a special case of F-Split in Figure 5 of Liang et al CAV 2014.
  INFER_SSPLIT_CST,
  // string split variable, for example:
  //    x = y, x = x1 ++ x2, y = y1 ++ y2
  //      implies x1 = y1 ++ x1' V y1 = x1 ++ y1'
  // This is rule F-Split in Figure 5 of Liang et al CAV 2014.
  INFER_SSPLIT_VAR,
  // flat form loop, for example:
  //    x = y, x = x1 ++ z, y = z ++ y2
  //      implies z = u2 ++ u1, u in ( u1 ++ u2 )*, x1 = u2 ++ u, y2 = u ++ u1
  //        for fresh u, u1, u2.
  // This is the rule F-Loop from Figure 5 of Liang et al CAV 2014.
  INFER_FLOOP,
};
std::ostream& operator<<(std::ostream& out, Inference i);

/**
 * Length status, used for indicating the length constraints for Skolems
 * introduced by the theory of strings.
 */
enum LengthStatus
{
  // The length of the Skolem should not be constrained. This should be
  // used for Skolems whose length is already implied.
  LENGTH_IGNORE,
  // The length of the Skolem is not specified, and should be split on.
  LENGTH_SPLIT,
  // The length of the Skolem is exactly one.
  LENGTH_ONE,
  // The length of the Skolem is greater than or equal to one.
  LENGTH_GEQ_ONE
};

/**
 * This data structure encapsulates an inference for strings. This includes
 * the form of the inference, as well as the side effects it generates.
 */
class InferInfo
{
 public:
  InferInfo();
  /**
   * The identifier for the inference, indicating the kind of reasoning used
   * for this conclusion.
   */
  Inference d_id;
  /** The conclusion of the inference */
  Node d_conc;
  /**
   * The antecedant(s) of the inference, interpreted conjunctively. These are
   * literals that currently hold in the equality engine.
   */
  std::vector<Node> d_ant;
  /**
   * The "new literal" antecedant(s) of the inference, interpreted
   * conjunctively. These are literals that were needed to show the conclusion
   * but do not currently hold in the equality engine.
   */
  std::vector<Node> d_antn;
  /**
   * A list of new skolems introduced as a result of this inference. They
   * are mapped to by a length status, indicating the length constraint that
   * can be assumed for them.
   */
  std::map<LengthStatus, std::vector<Node> > d_new_skolem;
  /**
   * The pending phase requirements, see InferenceManager::sendPhaseRequirement.
   */
  std::map<Node, bool> d_pending_phase;
  /**
   * The index in the normal forms under which this inference is addressing.
   * For example, if the inference is inferring x = y from |x|=|y| and
   *   w ++ x ++ ... = w ++ y ++ ...
   * then d_index is 1, since x and y are at index 1 in these concat terms.
   */
  unsigned d_index;
  /**
   * The normal form pair that is cached as a result of this inference.
   */
  Node d_nf_pair[2];
  /** for debugging
   *
   * The base pair of strings d_i/d_j that led to the inference, and whether
   * (d_rev) we were processing the normal forms of these strings in reverse
   * direction.
   */
  Node d_i;
  Node d_j;
  bool d_rev;
};

}  // namespace strings
}  // namespace theory
}  // namespace CVC4

#endif /* CVC4__THEORY__STRINGS__THEORY_STRINGS_H */
