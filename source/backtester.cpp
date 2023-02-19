#include <random>
#include "backtester.h"
using namespace std;
using namespace backtester;

//******************************************
//*     backtest_stop-loss_profit-take     *
//*****************************************/

size_t Backtester::findEntryPoint(const vector<bool>& strategySignals, unsigned cursor)
{
    // Lookup for the first true value.
    for (size_t i = cursor; i < strategySignals.size(); i++)
    {
        if (strategySignals[i])
            return i;
    }

    // If there aren't any matches, return the last position.
    return strategySignals.size() - 1;
}

size_t Backtester::exitPosition(const string& stock, unsigned entryTime, double profitTake, double stopLoss, double transactionCost)
{
    vector<double>& closePrices = Evaluator::IndicatorTimeSeries("ClosePrice", stock);

    // Lookup for the first time after the entry that satisfies the exit condition.
    const double buyPrice = (1.0 + transactionCost) * closePrices[entryTime];
    for (unsigned i = entryTime + 1; i < closePrices.size(); i++)
    {
        const double sellPrice = closePrices[i] * (1.0 - transactionCost);
        const double _return = (sellPrice / buyPrice) - 1.0;
        if (_return <= stopLoss || _return >= profitTake)
            return i;
    }

    // If there aren't any matches, return the last position.
    return closePrices.size() - 1;
}

vector<ExecutionData> Backtester::BacktestStoplossProfittake(const vector<bool>& strategySignals, const string& stock, double profitTake,
                                                             double stopLoss, double transactionCost, int minibatchSize)
{
    vector<ExecutionData> output;
    size_t cursor, exitIndex;
    size_t start, end;

    if (minibatchSize == -1)
    {
        start = 0U;
        end = (unsigned) strategySignals.size();
    }
    else
    {
        if (minibatchSize >= strategySignals.size() - 1)
            throw runtime_error("minibatchSize is greater than the size of the dataset.");

        random_device rd;
        mt19937 mt(rd());
        uniform_int_distribution<unsigned> dist(0, (unsigned)(strategySignals.size() - minibatchSize - 1));

        start = dist(mt);
        end = start + minibatchSize;
    }

    cursor = start;

    while (cursor < end)
    {
        ExecutionData buy, sell;

        cursor = findEntryPoint(strategySignals, cursor);
        exitIndex = exitPosition(stock, cursor, profitTake, stopLoss, transactionCost);

        buy.signalType = StrategySignal::Buy;
        buy.timeIndex = cursor;
        buy.time = Evaluator::Date(stock, cursor);
        buy.price = Evaluator::Indicator("ClosePrice", stock, cursor);

        sell.signalType = StrategySignal::Sell;
        sell.timeIndex = exitIndex;
        sell.time = Evaluator::Date(stock, exitIndex);
        sell.price = Evaluator::Indicator("ClosePrice", stock, exitIndex);

        output.push_back(buy);
        output.push_back(sell);

        cursor = exitIndex + 1;
    }

    return output;
}

//*********************************
//*     BacktestTimestopHit     *
//********************************/

vector<ExecutionData> Backtester::BacktestTimestopHit(const vector<bool>& strategySignals, const string& stock,
                                                      int timePeriod, int minibatchSize)
{
    size_t start, end;
    size_t cursor, exitPosition;

    if (minibatchSize == -1)
    {
        start = 0U;
        end = (unsigned) strategySignals.size();
    }
    else
    {
        if (minibatchSize >= strategySignals.size() - 1)
            throw runtime_error("BacktestStoplossProfittake: minibatchSize is greater than the size of the dataset.");

        random_device rd;
        mt19937 mt(rd());
        uniform_int_distribution<unsigned> dist(0, (unsigned)(strategySignals.size() - minibatchSize - 1));

        start = dist(mt);
        end = start + minibatchSize;
    }

    cursor = start;
    vector<ExecutionData> output;

    while (cursor < end)
    {
        if (strategySignals[cursor])
        {
            ExecutionData buy, sell;

            if (cursor + timePeriod < end)
                exitPosition = cursor + timePeriod;
            else if (static_cast<unsigned long long>(cursor) + static_cast<unsigned>(timePeriod) >= strategySignals.size())
                exitPosition = end - 1;
            else
                exitPosition = end;

            buy.signalType = StrategySignal::Buy;
            buy.timeIndex = cursor;
            buy.time = Evaluator::Date(stock, cursor);
            buy.price = Evaluator::Indicator("ClosePrice", stock, cursor);

            sell.signalType = StrategySignal::Sell;
            sell.timeIndex = exitPosition;
            sell.time = Evaluator::Date(stock, exitPosition);
            sell.price = Evaluator::Indicator("ClosePrice", stock, exitPosition);

            output.push_back(buy);
            output.push_back(sell);

            cursor += timePeriod;
        }
        else
            cursor++;
    }

    return output;
}

//**********************************
//*     BacktestMarketTiming     *
//*********************************/

struct StateMachine
{
    TimingStrategySignal startState {};
    std::vector<Transition> transitions {};

    static Transition CreateTransition(TimingStrategySignal parent, TimingStrategySignal child, bool input)
    {
        Transition transition {};
        transition.parent = parent;
        transition.child = child;
        transition.input = input;
        return transition;
    }

    static StateMachine Initialize()
    {
        StateMachine strategySignalsFSM;

        strategySignalsFSM.startState = TimingStrategySignal::NeutralHold;
        strategySignalsFSM.transitions = vector<Transition>{
                CreateTransition(TimingStrategySignal::NeutralHold, TimingStrategySignal::Enter, true),
                CreateTransition(TimingStrategySignal::NeutralHold, TimingStrategySignal::NeutralHold, false),
                CreateTransition(TimingStrategySignal::Enter, TimingStrategySignal::EnterHold, true),
                CreateTransition(TimingStrategySignal::Enter, TimingStrategySignal::Exit, false),
                CreateTransition(TimingStrategySignal::EnterHold, TimingStrategySignal::EnterHold, true),
                CreateTransition(TimingStrategySignal::EnterHold, TimingStrategySignal::Exit, false),
                CreateTransition(TimingStrategySignal::Exit, TimingStrategySignal::Enter, true),
                CreateTransition(TimingStrategySignal::Exit, TimingStrategySignal::ExitHold, false),
                CreateTransition(TimingStrategySignal::ExitHold, TimingStrategySignal::Enter, true),
                CreateTransition(TimingStrategySignal::ExitHold, TimingStrategySignal::ExitHold, false)
        };

        return strategySignalsFSM;
    }

    static TimingStrategySignal Iterate(StateMachine& fsm, TimingStrategySignal state, bool inputValue)
    {
        for (Transition& transition : fsm.transitions)
        {
            if (transition.parent == state && transition.input == inputValue)
                return transition.child;
        }

        cout << "Error: No match in StateMachine engine." << endl;
        return TimingStrategySignal();
    }

    static vector<TimingStrategySignal> Execute(StateMachine& fsm, const vector<bool>& inputValues)
    {
        TimingStrategySignal state = fsm.startState;
        vector<TimingStrategySignal> fsmStates;

        for (bool value : inputValues)
        {
            state = Iterate(fsm, state, value);
            fsmStates.push_back(state);
        }

        return fsmStates;
    }

};

vector<TimingStrategySignal> Backtester::getTimingStrategySignals(vector<bool> strategyValues)
{
    // Set the last value as false to ensure it always exits its position.
    strategyValues.pop_back();
    strategyValues.push_back(false);

    // Transform strategyValues into TimingStrategySignal using a finite state machine.
    StateMachine strategySignalsFSM = StateMachine::Initialize();
    return StateMachine::Execute(strategySignalsFSM, strategyValues);
}

vector<ExecutionData> Backtester::BacktestMarketTiming(const vector<bool> &strategySignals, const string& stock,
                                                       int minibatchSize)
{
    vector<TimingStrategySignal> signals = getTimingStrategySignals(strategySignals);
    size_t start, end;

    if (minibatchSize == -1)
    {
        start = 0;
        end = signals.size();
    }
    else
    {
        if (minibatchSize >= strategySignals.size() - 1)
            throw runtime_error("minibatchSize is greater than the size of the dataset.");

        random_device rd;
        mt19937 mt(rd());
        uniform_int_distribution<unsigned> dist(0, (unsigned)(strategySignals.size() - minibatchSize - 1));

        start = dist(mt);
        end = start + minibatchSize;
    }

    bool expectingEnter = true;
    vector<ExecutionData> strategyExecutionData;
    for (size_t i = start; i < end; i++)
    {
        if (expectingEnter && signals[i] == TimingStrategySignal::Enter)
        {
            ExecutionData operation;
            operation.signalType = StrategySignal::Buy;
            operation.timeIndex = i;
            operation.time = Evaluator::Date(stock, i);
            operation.price = Evaluator::Indicator("ClosePrice", stock, i);
            strategyExecutionData.push_back(operation);
            expectingEnter = false;
        }
        else if (!expectingEnter && signals[i] == TimingStrategySignal::Exit)
        {
            ExecutionData operation;
            operation.signalType = StrategySignal::Sell;
            operation.timeIndex = i;
            operation.time = Evaluator::Date(stock, i);
            operation.price = Evaluator::Indicator("ClosePrice", stock, i);
            strategyExecutionData.push_back(operation);
            expectingEnter = true;
        }
    }

    return strategyExecutionData;
}
