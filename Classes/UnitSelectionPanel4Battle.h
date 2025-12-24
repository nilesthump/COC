/*
UnitSelectionPanel (单位选择面板)
  │
  ├─ panel_background_ (DrawNode) - 半透明黑色背景
  │
  └─ unit_buttons_ (多个 UnitIconButton)
	   │
	   ├─ UnitIconButton #1 (野蛮人)
	   │    ├─ icon_sprite_ (图标图片)
	   │    ├─ selection_border_ (选中边框)
	   │    └─ name_label_ (单位名称)
	   │
	   ├─ UnitIconButton #2 (弓箭手)
	   └─ UnitIconButton #3 (巨人)
*/
#ifndef __UNIT_SELECTION_PANEL_H__
#define __UNIT_SELECTION_PANEL_H__

#include "cocos2d.h"
#include "CombatSessionManager.h"
#include <functional>
#include <string>

USING_NS_CC;


// 单位图标按钮类
class UnitIconButton : public Node
{
private:
	Sprite* icon_sprite_;              // 单位图标图片
	DrawNode* selection_border_;       // 选中时的黄色边框
	Label* name_label_;                // 单位名称标签
	Label* count_label_;			   // 剩余数量标签
	float base_scale_;

	UnitType unit_type_;               // 该按钮代表的单位类型
	bool is_selected_;                 // 是否被选中

	// 点击时的回调函数（通知Panel）
	std::function<void(UnitType)> on_click_callback_;

public:
	// 创建按钮
	static UnitIconButton* create(
		const std::string& iconPath,     // 图标图片路径
		const std::string& unitName,     // 显示的单位名称
		UnitType type,                   // 单位类型
		std::function<void(UnitType)> callback  // 点击回调
	);

	bool init(
		const std::string& iconPath,
		const std::string& unitName,
		UnitType type,
		std::function<void(UnitType)> callback
	);

	// 设置选中状态（会触发视觉变化）
	void setSelected(bool selected);
	void setCount(int count);
	// 获取状态
	bool isSelected() const { return is_selected_; }
	UnitType getUnitType() const { return unit_type_; }

private:
	void setupTouchListener();    // 设置触摸监听
	void updateVisuals();         // 更新视觉效果（边框、缩放）
};

// 单位选择面板类,确保同时只有一个被选中
class UnitSelectionPanel : public Node
{
private:
	DrawNode* panel_background_;               // 背景（半透明黑色矩形）
	std::vector<UnitIconButton*> unit_buttons_; // 所有单位按钮
	UnitIconButton* currently_selected_;       // 当前选中的按钮

	// 选择改变时的回调（通知BattleTestLayer）
	std::function<void(UnitType)> on_selection_changed_;

public:
	static UnitSelectionPanel* create(std::function<void(UnitType)> callback);

	bool init(std::function<void(UnitType)> callback);

	// 添加单位图标
	void addUnitIcon(
		const std::string& iconPath,
		const std::string& unitName,
		UnitType type
	);

	//告诉特定面板更新数字
	void updateUnitCount(UnitType type, int count);
	
	// 获取当前选中的单位类型
	UnitType getCurrentSelection() const;

	// 是否有选中的单位
	bool hasSelection() const { return currently_selected_ != nullptr; }

private:
	// 当某个按钮被点击时调用
	void onUnitButtonClicked(UnitType type);

	// 更新所有按钮的位置（居中排列）
	void updateButtonPositions();
};

#endif // __UNIT_SELECTION_PANEL_H__