#pragma once
#include "DirectXBase.h"

class ModelCommon
{
public:
	// 初期化
	void Initialize(DirectXBase* dxCommon);

	DirectXBase* GetDxCommon() const { return dxCommon_; }

private:
	DirectXBase* dxCommon_;
};

