#ifndef _Login_H_
#define _Login_H_

#include "cocos2d.h"

// 网格绘制类
class GridDraw : public cocos2d::Node
{
public:
	// 创建函数
	static GridDraw* create(float cellSize, float width, float height);

	// 初始化函数
	bool init(float cellSize, float width, float height);

	// 绘制隐形网格
	void drawGrid();

	// 在指定位置绘制菱形
	// horizontalSize: 水平对角线长度
	// verticalSize: 垂直对角线长度
	void drawDiamond(const cocos2d::Vec2& center, float horizontalSize, float verticalSize, const cocos2d::Color4F& color = cocos2d::Color4F(1.0f, 0.0f, 0.0f, 1.0f));

	// 获取菱形中心点坐标
	cocos2d::Vec2 getDiamondCenter() const;

private:
	// 网格参数
	float _cellSize;
	float _width;
	float _height;

	// DrawNode用于绘制
	cocos2d::DrawNode* _drawNode;

	// 菱形中心点
	cocos2d::Vec2 _diamondCenter;
};

class SecondScene :public cocos2d::Scene
{
public:
	// Constructor
	static cocos2d::Scene* createScene();
	
	bool virtual init();
	void menuFirstCallback(cocos2d::Ref* pSender);
	void menuBuildCallback(Ref* pSender);

	// Touch event handlers
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

	// Mouse event handlers
	void onMouseScroll(cocos2d::EventMouse* event);
	void onMouseMove(cocos2d::EventMouse* event);
	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	// Background bounds update
	void updateBackgroundBounds();
	
	// Convert screen coordinates to diamond coordinates
	cocos2d::Vec2 convertScreenToDiamond(const cocos2d::Vec2& screenPos);
	
	// Check if position is inside diamond
	bool isInDiamond(const cocos2d::Vec2& diamondPos);

	CREATE_FUNC(SecondScene);
private:
	// Build panel node
	Node* buildPanel;

	// Background sprite reference
	cocos2d::Sprite* background_sprite_;

	// Drag control variables
	bool is_dragging_;
	cocos2d::Vec2 drag_start_position_;
	cocos2d::Vec2 background_start_position_;

	// Scaling control variables
	bool is_scaling_;
	float previous_distance_;
	cocos2d::Vec2 scale_center_;
	float min_scale_; // Minimum scaling factor
	float max_scale_; // Maximum scaling factor
	
	// Diamond position information
	cocos2d::Vec2 diamond_center_;
	float diamond_width_;
	float diamond_height_;
	
	// Diamond grid information
    float grid_cell_size_x_;
    float grid_cell_size_y_;
    int grid_count_;
	
	// UI elements
	cocos2d::Label* coordinate_label_;
	cocos2d::Vec2 mouse_pos_;
	
	cocos2d::Vec2 getDiamondCenter() const;
	cocos2d::Vec2 getCellPosition(int gridX, int gridY) const;

	// 网格绘制类实例
	GridDraw* _gridDraw;

	// 单元格大小
	float _cellSize;
};

#endif
