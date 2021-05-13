#include <random>
#include "functional_map.h"
#include "backtester_models.h"
using namespace std;

/******************************
*                             *
*     Returns calculation     *
*                             *
******************************/

enum class CalculateSimpleReturnsState
{
    ExpectingBuy, ExpectingSell
};

double simple_returns(double psell, double pbuy, double transactionCost)
{
    return psell * (1 - transactionCost) - pbuy * (1 + transactionCost);
}

double log_returns(double psell, double pbuy, double transactionCost)
{
    return log(psell * (1 - transactionCost)) - log(pbuy * (1 + transactionCost));
}

double div_returns(double psell, double pbuy, double transactionCost)
{
    return (psell * (1 - transactionCost)) / (pbuy * (1 + transactionCost));
}

vector<double> calculate_returns(double (*returnFunction)(double, double, double), vector<StrategyExecutionData> executionData, double transactionCost)
{
    CalculateSimpleReturnsState state = CalculateSimpleReturnsState::ExpectingBuy;
    vector<double> returns;
    double buyPrice = 0.0;
    double sellPrice = 0.0;

    for (StrategyExecutionData execution : executionData)
    {
        if (state == CalculateSimpleReturnsState::ExpectingBuy)
        {
            if (execution.signalType == StrategySignal::Buy)
            {
                buyPrice = execution.price;
                state = CalculateSimpleReturnsState::ExpectingSell;
            }
            else
                throw runtime_error("calculate_simple_returns: Found sell signal while expecting buy signal.");
        }
        else if (state == CalculateSimpleReturnsState::ExpectingSell)
        {
            if (execution.signalType == StrategySignal::Sell)
            {
                sellPrice = execution.price;
                returns.push_back(returnFunction(sellPrice, buyPrice, transactionCost));
                state = CalculateSimpleReturnsState::ExpectingBuy;
            }
            else
                throw runtime_error("calculate_simple_returns: Found buy signal while expecting sell signal.");
        }
    }

    return returns;
}

vector<double> calculate_simple_returns(vector<StrategyExecutionData> executionData, double transactionCost)
{
    return calculate_returns(simple_returns, executionData, transactionCost);
}

vector<double> calculate_log_returns(vector<StrategyExecutionData> executionData, double transactionCost)
{
    return calculate_returns(log_returns, executionData, transactionCost);
}

vector<double> calculate_div_returns(vector<StrategyExecutionData> executionData, double transactionCost)
{
    return calculate_returns(div_returns, executionData, transactionCost);
}

/***************************************
*                                      *
*     backtest_stoploss_profittake     *
*                                      *
***************************************/

unsigned find_entry_point(const vector<bool>& strategySignals, unsigned cursor)
{
    // Lookup for the first true value.
    for (unsigned i = cursor; i < strategySignals.size(); i++)
    {
        if (strategySignals[i] == true)
            return i;
    }

    // If there aren't any matches, return the last position.
    return (unsigned)(strategySignals.size() - 1);
}

unsigned exit_position(const string& stock, unsigned entryTime, double profitTake, double stopLoss, double transactionCost)
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
    return (unsigned)(closePrices.size() - 1);
}

vector<StrategyExecutionData> backtest_stoploss_profittake(const vector<bool>& strategySignals, string stock, double profitTake,
                                                           double stopLoss, double transactionCost, int minibatchSize)
{
    vector<StrategyExecutionData> output;
    unsigned cursor, exitPosition;
    unsigned start, end;

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
    exitPosition = 0;

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

vector<vector<StrategyExecutionData>> backtest_stoploss_profittake_allstocks(const vector<vector<bool>>& strategySignals, double profitTake,
                                                                             double stopLoss, double transactionCost, int minibatchSize)
{
    vector<vector<StrategyExecutionData>> output;
    vector<string> stocks = utilities::Keys(*get_strategy_evaluator_dataset());

#if defined(_DEBUG)
    // In debug mode do not use multithread.
    if (stocks.size() == strategySignals.size())
    {
        for (unsigned i = 0; i < stocks.size(); i++)
            output.push_back(backtest_stoploss_profittake(strategySignals[i], stocks[i], profitTake, stopLoss, transactionCost, minibatchSize));
    }
#else
    const unsigned int hc = thread::hardware_concurrency();
    ThreadPool pool(hc);
    vector<future<vector<StrategyExecutionData>>> futureOutput;

    if (stocks.size() == strategySignals.size())
    {
        for (unsigned i = 0; i < stocks.size(); i++)
            futureOutput.push_back(pool.enqueue(backtest_stoploss_profittake, strategySignals[i], stocks[i], profitTake, 
                                                stopLoss, transactionCost, minibatchSize)
                                  );

        for (auto& r : futureOutput)
            output.push_back(r.get());
    }
#endif
    return output;
}

/********************************
*                               *
*     backtest_timestop_hit     *
*                               *
********************************/

vector<StrategyExecutionData> backtest_timestop_hit(const vector<bool>& strategySignals, string stock, int timePeriod,
                                                   double transactionCost, int minibatchSize)
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
    exitPosition = 0;
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

vector<vector<StrategyExecutionData>> backtest_timestop_hit_allstocks(const vector<vector<bool>>& strategySignals, int timePeriod,
                                                                     double transactionCost, int minibatchSize)
{
    vector<vector<StrategyExecutionData>> output;
    vector<string> stocks = utilities::Keys(*get_strategy_evaluator_dataset());

#if defined(_DEBUG)
    // In debug mode do not use multithread.
    if (stocks.size() == strategySignals.size())
    {
        for (unsigned i = 0; i < stocks.size(); i++)
            output.push_back(backtest_timestop_hit(strategySignals[i], stocks[i], timePeriod, transactionCost, minibatchSize));
    }
#else
    const unsigned int hc = thread::hardware_concurrency();
    ThreadPool pool(hc);
    vector<future<vector<StrategyExecutionData>>> futureOutput;

    if (stocks.size() == strategySignals.size())
    {
        for (unsigned i = 0; i < stocks.size(); i++)
            futureOutput.push_back(pool.enqueue(backtest_timestop_hit, strategySignals[i], stocks[i], timePeriod,
                                   transactionCost, minibatchSize)
            );

        for (auto& r : futureOutput)
            output.push_back(r.get());
    }
#endif
    return output;
}

/********************************
*                               *
*     backtest_markettiming     *
*                               *
********************************/

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
    Transition transition;
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

vector<TimingStrategySignal> execute_fsm(FSM& fsm, vector<bool> inputValues)
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
    // Set the last value to false to ensure it always exits its position.
    strategyValues.pop_back();
    strategyValues.push_back(false);

    // Transform strategyValues into TimingStrategySignal using a finite state machine.
    FSM strategySignalsFSM = init_strategy_signals_fsm();
    return execute_fsm(strategySignalsFSM, strategyValues);
}

vector<StrategyExecutionData> backtest_markettiming(const vector<bool>& strategySignals, string stock, double transactionCost,
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
            throw runtime_error("backtest_markettiming: minibatchSize is greater than the size of the dataset.");

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

vector<vector<StrategyExecutionData>> backtest_markettiming_allstocks(const vector<vector<bool>>& strategySignals, 
                                                                      double transactionCost, int minibatchSize)
{
    vector<vector<StrategyExecutionData>> output;
    vector<string> stocks = utilities::Keys(*get_strategy_evaluator_dataset());

#if defined(_DEBUG)
    // In debug mode do not use multithread.
    if (stocks.size() == strategySignals.size())
    {
        for (unsigned i = 0; i < stocks.size(); i++)
            output.push_back(backtest_markettiming(strategySignals[i], stocks[i], transactionCost, minibatchSize));
    }
#else
    const unsigned int hc = thread::hardware_concurrency();
    ThreadPool pool(hc);
    vector<future<vector<StrategyExecutionData>>> futureOutput;

    if (stocks.size() == strategySignals.size())
    {
        for (unsigned i = 0; i < stocks.size(); i++)
            futureOutput.push_back(pool.enqueue(backtest_markettiming, strategySignals[i], stocks[i],
                                   transactionCost, minibatchSize)
            );

        for (auto& r : futureOutput)
            output.push_back(r.get());
    }
#endif
    return output;
}