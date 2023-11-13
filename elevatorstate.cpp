#include "elevatorstate.h"

ElevatorState::ElevatorState(Elevator* elevator):elevator(elevator)
{ecs = ECS::getECS();}
