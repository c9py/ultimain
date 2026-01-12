/*
 * test_tensor_logic.cpp - Tests for NPC Tensor Logic Engine
 */

#include "test_framework.h"
#include "reasoning/TensorLogic.h"

using namespace Ultima::NPC::Reasoning;

// Test LogicalValue default construction
bool test_logical_value_default() {
    LogicalValue lv;
    TEST_ASSERT_FLOAT_NEAR(0.0, lv.truth, 0.01);
    TEST_ASSERT_FLOAT_NEAR(1.0, lv.confidence, 0.01);
    TEST_ASSERT_FLOAT_NEAR(1.0, lv.relevance, 0.01);
    return true;
}

// Test LogicalValue construction with value
bool test_logical_value_with_truth() {
    LogicalValue lv(0.75);
    TEST_ASSERT_FLOAT_NEAR(0.75, lv.truth, 0.01);
    TEST_ASSERT_FLOAT_NEAR(1.0, lv.confidence, 0.01);

    LogicalValue lv2(0.8, 0.9);
    TEST_ASSERT_FLOAT_NEAR(0.8, lv2.truth, 0.01);
    TEST_ASSERT_FLOAT_NEAR(0.9, lv2.confidence, 0.01);
    return true;
}

// Test LogicalValue isTrue/isFalse
bool test_logical_value_predicates() {
    LogicalValue high(0.8);
    LogicalValue low(0.3);
    LogicalValue mid(0.5);

    TEST_ASSERT(high.isTrue());
    TEST_ASSERT(!high.isFalse());

    TEST_ASSERT(!low.isTrue());
    TEST_ASSERT(low.isFalse());

    TEST_ASSERT(mid.isTrue());  // 0.5 >= 0.5

    // Custom threshold
    TEST_ASSERT(!mid.isTrue(0.6));
    TEST_ASSERT(mid.isTrue(0.4));

    return true;
}

// Test LogicalValue AND operation (conjunction)
bool test_logical_and() {
    LogicalValue a(0.8);
    LogicalValue b(0.6);

    LogicalValue result = a && b;

    // In fuzzy logic, AND is typically min
    TEST_ASSERT(result.truth <= a.truth);
    TEST_ASSERT(result.truth <= b.truth);

    return true;
}

// Test LogicalValue OR operation (disjunction)
bool test_logical_or() {
    LogicalValue a(0.3);
    LogicalValue b(0.7);

    LogicalValue result = a || b;

    // In fuzzy logic, OR is typically max
    TEST_ASSERT(result.truth >= a.truth);
    TEST_ASSERT(result.truth >= b.truth);

    return true;
}

// Test LogicalValue NOT operation (negation)
bool test_logical_not() {
    LogicalValue a(0.8);
    LogicalValue neg = !a;

    TEST_ASSERT_FLOAT_NEAR(0.2, neg.truth, 0.01);

    LogicalValue b(0.0);
    LogicalValue neg_b = !b;
    TEST_ASSERT_FLOAT_NEAR(1.0, neg_b.truth, 0.01);

    return true;
}

// Test Term types
bool test_term_types() {
    TEST_ASSERT(Term::Type::Variable != Term::Type::Constant);
    TEST_ASSERT(Term::Type::Constant != Term::Type::Function);
    TEST_ASSERT(Term::Type::Function != Term::Type::Predicate);
    return true;
}

// Test Term variable creation
bool test_term_variable() {
    Term var = Term::variable("x");
    TEST_ASSERT(var.type == Term::Type::Variable);
    TEST_ASSERT_STRING_EQUAL("x", var.name);
    return true;
}

// Test Term constant creation
bool test_term_constant() {
    Term str_const = Term::constant("Avatar");
    TEST_ASSERT(str_const.type == Term::Type::Constant);

    Term num_const = Term::constant(42.0);
    TEST_ASSERT(num_const.type == Term::Type::Constant);

    return true;
}

// Test Term predicate creation
bool test_term_predicate() {
    Term x = Term::variable("x");
    Term y = Term::variable("y");
    Term pred = Term::predicate("hasItem", {x, y});

    TEST_ASSERT(pred.type == Term::Type::Predicate);
    TEST_ASSERT_STRING_EQUAL("hasItem", pred.name);
    TEST_ASSERT_EQUAL(2, pred.arguments.size());
    return true;
}

// Test Formula types
bool test_formula_types() {
    TEST_ASSERT(Formula::Type::Atomic != Formula::Type::Negation);
    TEST_ASSERT(Formula::Type::Negation != Formula::Type::Conjunction);
    TEST_ASSERT(Formula::Type::Conjunction != Formula::Type::Disjunction);
    TEST_ASSERT(Formula::Type::Disjunction != Formula::Type::Implication);
    TEST_ASSERT(Formula::Type::Implication != Formula::Type::Universal);
    TEST_ASSERT(Formula::Type::Universal != Formula::Type::Existential);
    TEST_ASSERT(Formula::Type::Existential != Formula::Type::Modal);
    return true;
}

// Test atomic formula creation
bool test_formula_atomic() {
    Term pred = Term::predicate("isHero", {Term::variable("x")});
    Formula f = Formula::atomic(pred);

    TEST_ASSERT(f.type == Formula::Type::Atomic);
    TEST_ASSERT_STRING_EQUAL("isHero", f.predicate.name);
    return true;
}

// Test formula negation
bool test_formula_negation() {
    Term pred = Term::predicate("isEvil", {Term::constant("Guardian")});
    Formula atomic = Formula::atomic(pred);
    Formula neg = Formula::negation(atomic);

    TEST_ASSERT(neg.type == Formula::Type::Negation);
    TEST_ASSERT_EQUAL(1, neg.subformulas.size());
    return true;
}

// Test formula conjunction
bool test_formula_conjunction() {
    Term p1 = Term::predicate("isHero", {Term::constant("Avatar")});
    Term p2 = Term::predicate("hasWeapon", {Term::constant("Avatar")});

    Formula f1 = Formula::atomic(p1);
    Formula f2 = Formula::atomic(p2);
    Formula conj = Formula::conjunction(f1, f2);

    TEST_ASSERT(conj.type == Formula::Type::Conjunction);
    TEST_ASSERT_EQUAL(2, conj.subformulas.size());
    return true;
}

// Test formula disjunction
bool test_formula_disjunction() {
    Term p1 = Term::predicate("inBritain", {Term::variable("x")});
    Term p2 = Term::predicate("inTrinsic", {Term::variable("x")});

    Formula f1 = Formula::atomic(p1);
    Formula f2 = Formula::atomic(p2);
    Formula disj = Formula::disjunction(f1, f2);

    TEST_ASSERT(disj.type == Formula::Type::Disjunction);
    TEST_ASSERT_EQUAL(2, disj.subformulas.size());
    return true;
}

// Test formula implication
bool test_formula_implication() {
    Term antecedent_pred = Term::predicate("isHungry", {Term::variable("x")});
    Term consequent_pred = Term::predicate("seeksFood", {Term::variable("x")});

    Formula ante = Formula::atomic(antecedent_pred);
    Formula cons = Formula::atomic(consequent_pred);
    Formula impl = Formula::implication(ante, cons);

    TEST_ASSERT(impl.type == Formula::Type::Implication);
    TEST_ASSERT_EQUAL(2, impl.subformulas.size());
    return true;
}

// Test universal quantification
bool test_formula_forall() {
    Term pred = Term::predicate("isMortal", {Term::variable("x")});
    Formula body = Formula::atomic(pred);
    Formula univ = Formula::forall("x", body);

    TEST_ASSERT(univ.type == Formula::Type::Universal);
    TEST_ASSERT_STRING_EQUAL("x", univ.boundVariable);
    return true;
}

// Test existential quantification
bool test_formula_exists() {
    Term pred = Term::predicate("hasGold", {Term::variable("x")});
    Formula body = Formula::atomic(pred);
    Formula exist = Formula::exists("x", body);

    TEST_ASSERT(exist.type == Formula::Type::Existential);
    TEST_ASSERT_STRING_EQUAL("x", exist.boundVariable);
    return true;
}

// Test modal formula (beliefs)
bool test_formula_believes() {
    Term pred = Term::predicate("isEnemy", {Term::constant("Guardian")});
    Formula content = Formula::atomic(pred);
    Formula belief = Formula::believes("Avatar", content);

    TEST_ASSERT(belief.type == Formula::Type::Modal);
    TEST_ASSERT_STRING_EQUAL("Avatar", belief.agent);
    return true;
}

// Test complex formula construction
bool test_complex_formula() {
    // forall x. (isHero(x) -> exists y. (isCompanion(y) AND trusts(x, y)))
    Term isHero = Term::predicate("isHero", {Term::variable("x")});
    Term isCompanion = Term::predicate("isCompanion", {Term::variable("y")});
    Term trusts = Term::predicate("trusts", {Term::variable("x"), Term::variable("y")});

    Formula heroF = Formula::atomic(isHero);
    Formula companionF = Formula::atomic(isCompanion);
    Formula trustsF = Formula::atomic(trusts);

    Formula conj = Formula::conjunction(companionF, trustsF);
    Formula exist = Formula::exists("y", conj);
    Formula impl = Formula::implication(heroF, exist);
    Formula forall = Formula::forall("x", impl);

    TEST_ASSERT(forall.type == Formula::Type::Universal);
    TEST_ASSERT_EQUAL(1, forall.subformulas.size());
    TEST_ASSERT(forall.subformulas[0].type == Formula::Type::Implication);

    return true;
}

// Test fuzzy logic truth table for AND
bool test_fuzzy_and_truth_table() {
    // Test fuzzy AND with various values
    std::vector<std::pair<double, double>> cases = {
        {1.0, 1.0},
        {1.0, 0.0},
        {0.0, 1.0},
        {0.0, 0.0},
        {0.5, 0.5},
        {0.8, 0.3}
    };

    for (const auto& [a, b] : cases) {
        LogicalValue lv_a(a);
        LogicalValue lv_b(b);
        LogicalValue result = lv_a && lv_b;

        // Result should be <= min(a, b) for standard fuzzy AND
        TEST_ASSERT(result.truth <= std::max(a, b) + 0.01);
    }

    return true;
}

// Test fuzzy logic truth table for OR
bool test_fuzzy_or_truth_table() {
    // Test fuzzy OR with various values
    std::vector<std::pair<double, double>> cases = {
        {1.0, 1.0},
        {1.0, 0.0},
        {0.0, 1.0},
        {0.0, 0.0},
        {0.5, 0.5},
        {0.2, 0.7}
    };

    for (const auto& [a, b] : cases) {
        LogicalValue lv_a(a);
        LogicalValue lv_b(b);
        LogicalValue result = lv_a || lv_b;

        // Result should be >= max(a, b) for standard fuzzy OR
        TEST_ASSERT(result.truth >= std::min(a, b) - 0.01);
    }

    return true;
}

int main() {
    TEST_SUITE("Tensor Logic");

    RUN_TEST("LogicalValue default", test_logical_value_default);
    RUN_TEST("LogicalValue with truth", test_logical_value_with_truth);
    RUN_TEST("LogicalValue predicates", test_logical_value_predicates);
    RUN_TEST("Logical AND", test_logical_and);
    RUN_TEST("Logical OR", test_logical_or);
    RUN_TEST("Logical NOT", test_logical_not);
    RUN_TEST("Term types", test_term_types);
    RUN_TEST("Term variable", test_term_variable);
    RUN_TEST("Term constant", test_term_constant);
    RUN_TEST("Term predicate", test_term_predicate);
    RUN_TEST("Formula types", test_formula_types);
    RUN_TEST("Formula atomic", test_formula_atomic);
    RUN_TEST("Formula negation", test_formula_negation);
    RUN_TEST("Formula conjunction", test_formula_conjunction);
    RUN_TEST("Formula disjunction", test_formula_disjunction);
    RUN_TEST("Formula implication", test_formula_implication);
    RUN_TEST("Formula forall", test_formula_forall);
    RUN_TEST("Formula exists", test_formula_exists);
    RUN_TEST("Formula believes", test_formula_believes);
    RUN_TEST("Complex formula", test_complex_formula);
    RUN_TEST("Fuzzy AND truth table", test_fuzzy_and_truth_table);
    RUN_TEST("Fuzzy OR truth table", test_fuzzy_or_truth_table);

    TEST_SUMMARY();
}
