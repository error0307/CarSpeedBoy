#include <QtTest/QtTest>
#include "expression_state_machine.h"

/**
 * @brief Unit tests for ExpressionStateMachine
 */
class TestExpressionStateMachine : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases
    void testInitialState();
    void testRelaxedState();
    void testNormalState();
    void testAlertState();
    void testWarningState();
    void testScaredState();
    void testHysteresis();
    void testBoundaryValues();
    void testStateTransitions();
    void testSignalEmission();

private:
    ExpressionStateMachine* state_machine_;
};

void TestExpressionStateMachine::initTestCase() {
    qInfo() << "Starting ExpressionStateMachine tests";
}

void TestExpressionStateMachine::cleanupTestCase() {
    qInfo() << "ExpressionStateMachine tests completed";
}

void TestExpressionStateMachine::init() {
    state_machine_ = new ExpressionStateMachine();
}

void TestExpressionStateMachine::cleanup() {
    delete state_machine_;
    state_machine_ = nullptr;
}

void TestExpressionStateMachine::testInitialState() {
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::RELAXED);
    QCOMPARE(state_machine_->getStateString(), QString("relaxed"));
}

void TestExpressionStateMachine::testRelaxedState() {
    state_machine_->updateSpeed(0.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::RELAXED);
    
    state_machine_->updateSpeed(10.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::RELAXED);
    
    state_machine_->updateSpeed(20.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::RELAXED);
}

void TestExpressionStateMachine::testNormalState() {
    state_machine_->updateSpeed(21.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::NORMAL);
    
    state_machine_->updateSpeed(40.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::NORMAL);
    
    state_machine_->updateSpeed(60.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::NORMAL);
}

void TestExpressionStateMachine::testAlertState() {
    state_machine_->updateSpeed(61.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::ALERT);
    
    state_machine_->updateSpeed(80.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::ALERT);
    
    state_machine_->updateSpeed(100.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::ALERT);
}

void TestExpressionStateMachine::testWarningState() {
    state_machine_->updateSpeed(101.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::WARNING);
    
    state_machine_->updateSpeed(110.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::WARNING);
    
    state_machine_->updateSpeed(120.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::WARNING);
}

void TestExpressionStateMachine::testScaredState() {
    state_machine_->updateSpeed(121.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::SCARED);
    
    state_machine_->updateSpeed(150.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::SCARED);
    
    state_machine_->updateSpeed(200.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::SCARED);
}

void TestExpressionStateMachine::testHysteresis() {
    // Move to NORMAL state
    state_machine_->updateSpeed(30.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::NORMAL);
    
    // Speed decreasing: hysteresis margin (2.0) is applied
    // relaxed_max (20) + margin (2) = 22
    // Speed 22.5 should stay NORMAL
    state_machine_->updateSpeed(22.5);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::NORMAL);
    
    // Speed 21 is <= 22, so it becomes RELAXED
    state_machine_->updateSpeed(21.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::RELAXED);
}

void TestExpressionStateMachine::testBoundaryValues() {
    // Test exact threshold values
    state_machine_->updateSpeed(20.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::RELAXED);
    
    state_machine_->updateSpeed(21.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::NORMAL);
    
    state_machine_->updateSpeed(60.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::NORMAL);
    
    state_machine_->updateSpeed(61.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::ALERT);
}

void TestExpressionStateMachine::testStateTransitions() {
    // RELAXED -> NORMAL -> ALERT -> WARNING -> SCARED
    state_machine_->updateSpeed(10.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::RELAXED);
    
    state_machine_->updateSpeed(40.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::NORMAL);
    
    state_machine_->updateSpeed(80.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::ALERT);
    
    state_machine_->updateSpeed(110.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::WARNING);
    
    state_machine_->updateSpeed(150.0);
    QCOMPARE(state_machine_->getCurrentState(), ExpressionState::SCARED);
}

void TestExpressionStateMachine::testSignalEmission() {
    QSignalSpy stateSpy(state_machine_, &ExpressionStateMachine::stateChanged);
    QSignalSpy stringStateSpy(state_machine_, &ExpressionStateMachine::stateStringChanged);
    
    // Change from RELAXED to NORMAL
    state_machine_->updateSpeed(30.0);
    
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stringStateSpy.count(), 1);
    
    QList<QVariant> arguments = stateSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<ExpressionState>(), ExpressionState::RELAXED);
    QCOMPARE(arguments.at(1).value<ExpressionState>(), ExpressionState::NORMAL);
    
    // Clear the spy after taking the first signal
    stringStateSpy.clear();
    
    // Same state should not emit signal
    state_machine_->updateSpeed(40.0);
    QCOMPARE(stateSpy.count(), 0);
    QCOMPARE(stringStateSpy.count(), 0);
}

QTEST_MAIN(TestExpressionStateMachine)
#include "test_expression_state_machine.moc"
