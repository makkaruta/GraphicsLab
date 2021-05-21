#pragma once

class GameComponent {
public:
	virtual void Initialize();
	virtual void Update();
	virtual void Draw();
	virtual void DestroyResources();
};
