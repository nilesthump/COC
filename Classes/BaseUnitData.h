//AttackerData和DefenderData都继承于它
#ifndef _BASE_UNIT_DATA_
#define _BASE_UNIT_DATA_
#include<string>

//近战远程枚举
enum class CombatType
{
	MELEE,      //近战
	RANGED      //远程
};

//单位类型枚举
enum class UnitType
{
	GROUND,		    // 只攻击地面
	AIR,			// 只攻击空中
	BOTH			// 都攻击
};

//攻击类型枚举（单体，范围，连锁）
enum class AttackType
{
	SINGLE_TARGET,  // 单体攻击
	SPLASH,         // 范围溅射
	CHAIN           // 连锁攻击
};

//资源使用枚举
enum class ResourceType
{
	ELIXIR,         //圣水
	DARK_ELIXIR,    //暗黑重油
	GOLD            //金币
};
struct BaseUnitData
{
    //标识信息
    std::string id;             //内部标识
    std::string name;           //显示名称
    int level;                  //等级

    //战斗属性
    int health;                 //生命值
    int damage;                 //攻击力
    double attack_interval;     //攻击间隔（秒）
    double attack_distance;     //攻击距离（格）

    //攻击特性
    AttackType attack_type;     //单体/范围/连锁
    UnitType unit_type;         //地面/空中/两者
    CombatType combat_type;     //近战/远程
    ResourceType resource_type; //圣水/暗黑重油/金币

    //移动属性（防御建筑为0）
    double move_speed;          //移动速度（格/秒）

    // 构造函数
    BaseUnitData()
        : level(1),
        health(0),
        damage(0),
        attack_interval(1.0),
        attack_distance(0.0),
        move_speed(0.0)
    {}

    virtual ~BaseUnitData() = default;

    BaseUnitData(const BaseUnitData& other) = default;  
    BaseUnitData& operator=(const BaseUnitData& other) = default; 

    //获取展示名称
    std::string GetDisplayName() const
    {
        return name + " Lv." + std::to_string(level);
    }

    //判断是否可以移动
    bool CanMove() const
    {
        return move_speed > 0.01; 
    }

    virtual bool IsAttacker() const { return false; }
    virtual bool IsDefender() const { return false; }
};
#endif //_BASE_UNIT_DATA_

