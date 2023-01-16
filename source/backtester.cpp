#include <random>
#include "backtester.h"
using namespace std;

//******************************************
//*     backtest_stop-loss_profit-take     *
//*****************************************/

size_t find_entry_point(const vector<bool>& strategySignals, unsigned cursor)
{
    // Lookup for the first true value.
    for (unsigned i = cursor; i < strategySignals.size(); i++)
    {
        if (strategySignals[i])
            return i;
    }

    // If there aren't any matches, return the last position.
    return strategySignals.size() - 1;
}

size_t exit_position(const string& stock, unsigned entryTime, double profitTake, double stopLoss, double transactionCost)
{
    vector<double>& closePrices = IndicatorTimeSeries("ClosePrice", stock);

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

vector<StrategyExecutionData> backtest_stoploss_profittake(const vector<bool>& strategySignals, const string& stock, double profitTake,
                                                           double stopLoss, double transactionCost, int minibatchSize)
{
    vector<StrategyExecutionData> output;
    size_t cursor, exitPosition;
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
        StrategyExecutionData buy, sell;

        cursor = find_entry_point(strategySignals, cursor);
        exitPosition = exit_position(stock, cursor, profitTake, stopLoss, transactionCost);

        buy.signalType = StrategySignal::Buy;
        buy.timeIndex = cursor;
        buy.time = Date(stock, cursor);
        buy.price = Indicator("ClosePrice", stock, cursor);

        sell.signalType = StrategySignal::Sell;
        sell.timeIndex = exitPosition;
        sell.time = Date(stock, exitPosition);
        sell.price = Indicator("ClosePrice", stock, exitPosition);

        output.push_back(buy);
        output.push_back(sell);

        cursor = exitPosition + 1;
    }

    return output;
}

//*********************************
//*     backtest_timestop_hit     *
//********************************/

vector<StrategyExecutionData> backtest_timestop_hit(const vector<bool>& strategySignals, const string& stock,
                                                    int timePeriod, double transactionCost, int minibatchSize)
{
    unsigned start, end;
    unsigned cursor, exitPosition;

    if (minibatchSize == -1)
    {
        start = 0U;
        end = (unsigned) strategySignals.size();
    }
    else
    {
        if (minibatchSize >= strategySignals.size() - 1)
            throw runtime_error("backtest_stoploss_profittake: minibatchSize is greater than the size of the dataset.");

        random_device rd;
        mt19937 mt(rd());
        uniform_int_distribution<unsigned> dist(0, (unsigned)(strategySignals.size() - minibatchSize - 1));

        start = dist(mt);
        end = start + minibatchSize;
    }

    cursor = start;
    vector<StrategyExecutionData> output;

    while (cursor < end)
    {
        if (strategySignals[cursor])
        {
            StrategyExecutionData buy, sell;

            if (cursor + timePeriod < end)
                exitPosition = cursor + timePeriod;
            else if (static_cast<unsigned long long>(cursor) + static_cast<unsigned>(timePeriod) >= strategySignals.size())
                exitPosition = end - 1;
            else
                exitPosition = end;

            buy.signalType = StrategySignal::Buy;
            buy.timeIndex = cursor;
            buy.time = Date(stock, cursor);
            buy.price = Indicator("ClosePrice", stock, cursor);

            sell.signalType = StrategySignal::Sell;
            sell.timeIndex = exitPosition;
            sell.time = Date(stock, exitPosition);
            sell.price = Indicator("ClosePrice", stock, exitPosition);

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
//*     backtest_market_timing     *
//*********************************/

enum class TimingStrategySignal
{
    NeutralHold,
    EnterHold,
    ExitHold,
    Enter,
    Exit
};

struct Transition
{
    TimingStrategySignal parent;
    TimingStrategySignal child;
    bool input;
};

struct FSM
{
    TimingStrategySignal startState;
    std::vector<Transition> transitions;
};

Transition create_fsm_transition(TimingStrategySignal parent, TimingStrategySignal child, bool input)
{
    Transition transition {};
    transition.parent = parent;
    transition.child = child;
    transition.input = input;
    return transition;
}

FSM init_strategy_signals_fsm()
{
    FSM strategySignalsFSM;

    strategySignalsFSM.startState = TimingStrategySignal::NeutralHold;
    strategySignalsFSM.transitions = vector<Transition>{
            create_fsm_transition(TimingStrategySignal::NeutralHold, TimingStrategySignal::Enter, true),
            create_fsm_transition(TimingStrategySignal::NeutralHold, TimingStrategySignal::NeutralHold, false),
            create_fsm_transition(TimingStrategySignal::Enter, TimingStrategySignal::EnterHold, true),
            create_fsm_transition(TimingStrategySignal::Enter, TimingStrategySignal::Exit, false),
            create_fsm_transition(TimingStrategySignal::EnterHold, TimingStrategySignal::EnterHold, true),
            create_fsm_transition(TimingStrategySignal::EnterHold, TimingStrategySignal::Exit, false),
            create_fsm_transition(TimingStrategySignal::Exit, TimingStrategySignal::Enter, true),
            create_fsm_transition(TimingStrategySignal::Exit, TimingStrategySignal::ExitHold, false),
            create_fsm_transition(TimingStrategySignal::ExitHold, TimingStrategySignal::Enter, true),
            create_fsm_transition(TimingStrategySignal::ExitHold, TimingStrategySignal::ExitHold, false)
    };

    return strategySignalsFSM;
}

TimingStrategySignal iterate_fsm(FSM& fsm, TimingStrategySignal state, bool inputValue)
{
    for (Transition& transition : fsm.transitions)
    {
        if (transition.parent == state && transition.input == inputValue)
            return transition.child;
    }

    cout << "Error: No match in FSM engine." << endl;
    return TimingStrategySignal();
}

vector<TimingStrategySignal> execute_fsm(FSM& fsm, const vector<bool>& inputValues)
{
    TimingStrategySignal state = fsm.startState;
    vector<TimingStrategySignal> fsmStates;

    for (bool value : inputValues)
    {
        state = iterate_fsm(fsm, state, value);
        fsmStates.push_back(state);
    }

    return fsmStates;
}

vector<TimingStrategySignal> get_timing_strategy_signals(vector<bool> strategyValues)
{
    // Set the last value as false to ensure it always exits its position.
    strategyValues.pop_back();
    strategyValues.push_back(false);

    // Transform strategyValues into TimingStrategySignal using a finite state machine.
    FSM strategySignalsFSM = init_strategy_signals_fsm();
    return execute_fsm(strategySignalsFSM, strategyValues);
}

vector<StrategyExecutionData> backtest_market_timing(const vector<bool> &strategySignals, string stock, double transactionCost,
                                                     int minibatchSize)
{
    vector<TimingStrategySignal> signals = get_timing_strategy_signals(strategySignals);
    unsigned start, end;

    if (minibatchSize == -1)
    {
        start = 0U;
        end = (unsigned) signals.size();
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
    vector<StrategyExecutionData> strategyExecutionData;
    for (unsigned i = start; i < end; i++)
    {
        if (expectingEnter && signals[i] == TimingStrategySignal::Enter)
        {
            StrategyExecutionData operation;
            operation.signalType = StrategySignal::Buy;
            operation.timeIndex = i;
            operation.time = Date(stock, i);
            operation.price = Indicator("ClosePrice", stock, i);
            strategyExecutionData.push_back(operation);
            expectingEnter = false;
        }
        else if (!expectingEnter && signals[i] == TimingStrategySignal::Exit)
        {
            StrategyExecutionData operation;
            operation.signalType = StrategySignal::Sell;
            operation.timeIndex = i;
            operation.time = Date(stock, i);
            operation.price = Indicator("ClosePrice", stock, i);
            strategyExecutionData.push_back(operation);
            expectingEnter = true;
        }
    }

    return strategyExecutionData;
}
