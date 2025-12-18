#pragma once
#ifndef __ZOOM_SCROLL_MANAGER_H__
#define __ZOOM_SCROLL_MANAGER_H__

#include "cocos2d.h"

class ZoomScrollManager : public cocos2d::Node
{
private:
	// 目标精灵
	cocos2d::Sprite* target_sprite_;

	// 拖拽控制变量
	bool is_dragging_;
	cocos2d::Vec2 drag_start_position_;
	cocos2d::Vec2 sprite_start_position_;

	// 缩放控制变量
	bool is_scaling_;
	float previous_distance_;
	cocos2d::Vec2 scale_center_;
	float min_scale_;
	float max_scale_;

public:
	// 创建函数
	static ZoomScrollManager* create(cocos2d::Sprite* targetSprite, float minScale = 0.5f, float maxScale = 2.0f);

	// 初始化函数
	bool init(cocos2d::Sprite* targetSprite, float minScale, float maxScale);

	// 更新背景边界
	void updateBackgroundBounds();

	// 触摸事件处理函数
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

	// 鼠标事件处理函数
	void onMouseScroll(cocos2d::EventMouse* event);

	// 多点触控事件处理函数
	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	// 以下用来设置获取缩放比例
	// 设置最小最大缩放比例
	void setMinScale(float minScale) { min_scale_ = minScale; }
	void setMaxScale(float maxScale) { max_scale_ = maxScale; }

	// 获取当前缩放比例
	float getCurrentScale() const { return target_sprite_->getScale(); }

	// 设置缩放比例
	void setScale(float scale);

	// 获取目标精灵
	cocos2d::Sprite* getTargetSprite() const { return target_sprite_; }


};

#endif // __ZOOM_SCROLL_MANAGER_H__