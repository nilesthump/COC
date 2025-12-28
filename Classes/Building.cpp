#include "Building.h"
#include"SecondScene.h"
using namespace cocos2d;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

void Building::playFailBlinkAndRemove()
{
    _sprite->setColor(Color3B::RED); // 设为红色
    this->runAction(Sequence::create(
        Blink::create(0.5f, 2),    // 闪烁2次
        RemoveSelf::create(true),  
        nullptr
    ));
}

void Building::playFailBlink()
{
    _sprite->setColor(Color3B::RED); // 设为红色
    this->runAction(Sequence::create(
        Blink::create(0.5f, 2),    // 闪烁2次      
        nullptr
    ));
}