//
// Created by cheily on 08.06.2024.
//

#pragma once


#include "ECS/SignalQueue/DataCargo/SignalData.h"

struct SaveSignal : public SignalData {
    static Signal signal();
};