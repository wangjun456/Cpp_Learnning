#include<iostream>
#include<vector>

using namespace std;

//FSM状态项
class FSMItem
{
    friend class FSM;
private:
   	//动作函数
    static void getUp()
    {
        cout << "student is getting up!" << endl;
    }
    static void go2School()
    {
        cout << "student is going to school!" << endl;
    }
    static void haveLunch()
    {
        cout << "student is having lunch!" << endl;
    }
    static void doHomework()
    {
        cout << "student is doing homework!" << endl;
    }
    static void sleeping()
    {
        cout << "student is sleeping!" << endl;
    }
public:
    //枚举所有状态
    enum State
    {
        GETUP = 0,
        GOTOSCHOOL,
        HAVELUNCH,
        DOHOMEWORK,
        SLEEP
    };
    //枚举所有事件
    enum Events
    {
        EVENT1 = 0,
        EVENT2,
        EVENT3
    };
public:
    //初始化构造函数
    FSMItem(State curState, Events event, void(*action)(), State nextState)
        :_curState(curState), _event(event), _action(action), _nextState(nextState) {}
private:
    State   _curState;      //现态
    Events  _event;         //条件
    void    (*_action)();   //动作
    State   _nextState;     //次态
};

class FSM
{
public:
    //初始化状态机
    FSM(FSMItem::State curState= FSMItem::GETUP):_curState(curState)
    {
        initFSMTable();
    }
    //状态转移
    void transferState(FSMItem::State nextState)
    {
        _curState = nextState;
    }
    //根据当前状态和发生的事件，执行相应的动作，并进行状态转移
    void handleEvent(FSMItem::Events event)
    {
        FSMItem::State  curState = _curState;   //现态
        void (*action)() = nullptr;//动作
        FSMItem::State nextState;  //次态
        bool flag = false;
        for (int i = 0; i < _fsmTable.size(); i++)
        {
            if (event == _fsmTable[i]->_event && curState == _fsmTable[i]->_curState)
            {
                flag = true;
                action = _fsmTable[i]->_action;
                nextState = _fsmTable[i]->_nextState;
                break;
            }
        }
        //找到对应的状态项，执行动作，转移状态
        if (flag)
        {
            if (action)
            {
                action();
            }
            transferState(nextState);
        }
    }
private:
    //根据画的状态转移图初始化状态转移表
    void initFSMTable()
    {
        _fsmTable.push_back(new FSMItem(FSMItem::GETUP, FSMItem::EVENT1, &FSMItem::getUp, FSMItem::GOTOSCHOOL));
        _fsmTable.push_back(new FSMItem(FSMItem::GOTOSCHOOL, FSMItem::EVENT2, &FSMItem::go2School, FSMItem::HAVELUNCH));
        _fsmTable.push_back(new FSMItem(FSMItem::HAVELUNCH, FSMItem::EVENT3, &FSMItem::haveLunch, FSMItem::DOHOMEWORK));
        _fsmTable.push_back(new FSMItem(FSMItem::DOHOMEWORK, FSMItem::EVENT1, &FSMItem::doHomework, FSMItem::SLEEP));
        _fsmTable.push_back(new FSMItem(FSMItem::SLEEP, FSMItem::EVENT2, &FSMItem::sleeping, FSMItem::GETUP));
    }
public:
    FSMItem::State _curState;  //现态
private:
    vector<FSMItem*> _fsmTable;  //状态转移表
};

//测试事件变换
void testEvent(FSMItem::Events& event)
{
    switch (event)
    {
    case FSMItem::EVENT1:
        event = FSMItem::EVENT2;
        break;
    case FSMItem::EVENT2:
        event = FSMItem::EVENT3;
        break;
    case FSMItem::EVENT3:
        event = FSMItem::EVENT1;
        break;
    }
}
int main()
{
    FSM *fsm = new FSM();
    auto event = FSMItem::EVENT1;
    while (1)
    {
        cout << "event " << event << " is coming..." << endl;
        fsm->handleEvent(event);
        cout << "fsm current state is " << fsm->_curState << endl;
        testEvent(event);
    }
    return 0;
}   