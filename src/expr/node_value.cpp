/*********************                                           -*- C++ -*-  */
/** node_value.cpp
 ** This file is part of the CVC4 prototype.
 ** Copyright (c) 2009 The Analysis of Computer Systems Group (ACSys)
 ** Courant Institute of Mathematical Sciences
 ** New York University
 ** See the file COPYING in the top-level source directory for licensing
 ** information.
 **
 ** An expression node.
 **
 ** Instances of this class are generally referenced through
 ** cvc4::Node rather than by pointer; cvc4::Node maintains the
 ** reference count on NodeValue instances and
 **/

#include "node_value.h"
#include <sstream>

using namespace std;

namespace CVC4 {

size_t NodeValue::next_id = 1;

NodeValue::NodeValue() :
  d_id(0), d_rc(MAX_RC), d_kind(NULL_EXPR), d_nchildren(0) {
}

NodeValue::NodeValue(int) :
  d_id(0), d_rc(0), d_kind(unsigned(UNDEFINED_KIND)), d_nchildren(0) {
}

NodeValue::~NodeValue() {
  for(ev_iterator i = ev_begin(); i != ev_end(); ++i) {
    (*i)->dec();
  }
}

uint64_t NodeValue::hash() const {
  return computeHash(d_kind, ev_begin(), ev_end());
}

NodeValue* NodeValue::inc() {
  // FIXME multithreading
  if(d_rc < MAX_RC)
    ++d_rc;
  return this;
}

NodeValue* NodeValue::dec() {
  // FIXME multithreading
  if(d_rc < MAX_RC) {
    if(--d_rc == 0) {
      // FIXME gc
      return 0;
    }
  }
  return this;
}

NodeValue::iterator NodeValue::begin() {
  return node_iterator(d_children);
}

NodeValue::iterator NodeValue::end() {
  return node_iterator(d_children + d_nchildren);
}

NodeValue::iterator NodeValue::rbegin() {
  return node_iterator(d_children + d_nchildren - 1);
}

NodeValue::iterator NodeValue::rend() {
  return node_iterator(d_children - 1);
}

NodeValue::const_iterator NodeValue::begin() const {
  return const_node_iterator(d_children);
}

NodeValue::const_iterator NodeValue::end() const {
  return const_node_iterator(d_children + d_nchildren);
}

NodeValue::const_iterator NodeValue::rbegin() const {
  return const_node_iterator(d_children + d_nchildren - 1);
}

NodeValue::const_iterator NodeValue::rend() const {
  return const_node_iterator(d_children - 1);
}

NodeValue::ev_iterator NodeValue::ev_begin() {
  return d_children;
}

NodeValue::ev_iterator NodeValue::ev_end() {
  return d_children + d_nchildren;
}

NodeValue::ev_iterator NodeValue::ev_rbegin() {
  return d_children + d_nchildren - 1;
}

NodeValue::ev_iterator NodeValue::ev_rend() {
  return d_children - 1;
}

NodeValue::const_ev_iterator NodeValue::ev_begin() const {
  return d_children;
}

NodeValue::const_ev_iterator NodeValue::ev_end() const {
  return d_children + d_nchildren;
}

NodeValue::const_ev_iterator NodeValue::ev_rbegin() const {
  return d_children + d_nchildren - 1;
}

NodeValue::const_ev_iterator NodeValue::ev_rend() const {
  return d_children - 1;
}

string NodeValue::toString() const {
  stringstream ss;
  toStream(ss);
  return ss.str();
}

void NodeValue::toStream(std::ostream& out) const {
  out << "(" << Kind(d_kind);
  if(d_kind == VARIABLE) {
    out << ":" << this;
  } else {
    for(const_iterator i = begin(); i != end(); ++i) {
      if(i != end()) {
        out << " ";
      }
      out << *i;
    }
  }
  out << ")";
}

}/* CVC4 namespace */
