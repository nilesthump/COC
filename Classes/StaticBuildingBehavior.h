#pragma once
#include "BaseBehavior.h"
class StaticBuildingBehavior :public BaseBehavior
{
public:
    //可以覆盖基类方法或添加新方法
    std::string GetBehaviorType() const override
    {
        return "StaticBuildingBehavior";
    }

};


