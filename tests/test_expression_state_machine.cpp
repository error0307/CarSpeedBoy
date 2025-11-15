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
    QCOMPARE(state_machine_->currentState(), ExpressionState::RELAXED);
    QCOMPARE(state_machine_->stateString(), QString("RELAXED"));
}

void TestExpressionStateMachine::testRelaxedState() {
    state_machine_->onSpeedUpdate(0.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::RELAXED);
    
    state_machine_->onSpeedUpdate(10.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::RELAXED);
    
    state_machine_->onSpeedUpdate(20.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::RELAXED);
}

void TestExpressionStateMachine::testNormalState() {
    state_machine_->onSpeedUpdate(21.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::NORMAL);
    
    state_machine_->onSpeedUpdate(40.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::NORMAL);
    
    state_machine_->onSpeedUpdate(60.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::NORMAL);
}

void TestExpressionStateMachine::testAlertState() {
    state_machine_->onSpeedUpdate(61.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::ALERT);
    
    state_machine_->onSpeedUpdate(80.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::ALERT);
    
    state_machine_->onSpeedUpdate(100.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::ALERT);
}

void TestExpressionStateMachine::testWarningState() {
    state_machine_->onSpeedUpdate(101.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::WARNING);
    
    state_machine_->onSpeedUpdate(110.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::WARNING);
    
    state_machine_->onSpeedUpdate(120.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::WARNING);
}

void TestExpressionStateMachine::testScaredState() {
    state_machine_->onSpeedUpdate(121.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::SCARED);
    
    state_machine_->onSpeedUpdate(150.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::SCARED);
    
    state_machine_->onSpeedUpdate(200.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::SCARED);
}

void TestExpressionStateMachine::testHysteresis() {
    // Move to NORMAL state
    state_machine_->onSpeedUpdate(30.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::NORMAL);
    
    // At boundary (20 km/h), should stay in NORMAL due to hysteresis
    state_machine_->onSpeedUpdate(20.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::NORMAL);
    
    // Below hysteresis margin (18 km/h), should transition to RELAXED
    state_machine_->onSpeedUpdate(18.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::RELAXED);
}

void TestExpressionStateMachine::testBoundaryValues() {
    // Test exact threshold values
    state_machine_->onSpeedUpdate(20.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::RELAXED);
    
    state_machine_->onSpeedUpdate(21.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::NORMAL);
    
    state_machine_->onSpeedUpdate(60.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::NORMAL);
    
    state_machine_->onSpeedUpdate(61.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::ALERT);
}

void TestExpressionStateMachine::testStateTransitions() {
    // RELAXED -> NORMAL -> ALERT -> WARNING -> SCARED
    state_machine_->onSpeedUpdate(10.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::RELAXED);
    
    state_machine_->onSpeedUpdate(40.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::NORMAL);
    
    state_machine_->onSpeedUpdate(80.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::ALERT);
    
    state_machine_->onSpeedUpdate(110.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::WARNING);
    
    state_machine_->onSpeedUpdate(150.0);
    QCOMPARE(state_machine_->currentState(), ExpressionState::SCARED);
}

void TestExpressionStateMachine::testSignalEmission() {
    QSignalSpy stateSpy(state_machine_, &ExpressionStateMachine::stateChanged);
    QSignalSpy stringStateSpy(state_machine_, &ExpressionStateMachine::stateStringChanged);
    
    // Change from RELAXED to NORMAL
    state_machine_->onSpeedUpdate(30.0);
    
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stringStateSpy.count(), 1);
    
    QList<QVariant> arguments = stateSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<ExpressionState>(), ExpressionState::RELAXED);
    QCOMPARE(arguments.at(1).value<ExpressionState>(), ExpressionState::NORMAL);
    
    // Same state should not emit signal
    state_machine_->onSpeedUpdate(40.0);
    QCOMPARE(stateSpy.count(), 0);
    QCOMPARE(stringStateSpy.count(), 0);
}

QTEST_MAIN(TestExpressionStateMachine)
#include "test_expression_state_machine.moc"
