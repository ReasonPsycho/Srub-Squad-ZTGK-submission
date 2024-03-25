//
// Created by igork on 22.03.2024.
//

#ifndef ZTGK_MININGSTATE_H
#define ZTGK_MININGSTATE_H
#include "Unit/UnitAI/StateMachine/State.h"
class MovementState;
class CombatState;
class IdleState;
class MiningState : public State{
public:
    State* RunCurrentState() override;
    bool isTargetInRange() override;
    MovementState* MoveState;
    CombatState* CombatState;
    IdleState* IdleState;

private:
    void Mine();
};


#endif //ZTGK_MININGSTATE_H