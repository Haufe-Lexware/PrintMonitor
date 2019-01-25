#include "stdafx.h"
#include "monitorui.h"
#include "log.h"

// static 
PMONITORUI CMonitorUI::Initialize()
{
	static MONITORUI ui = { sizeof(ui), 0 };
	ui.pfnAddPortUI = CMonitorUI::AddPortUI;
	ui.pfnConfigurePortUI = CMonitorUI::ConfigurePortUI;
	ui.pfnDeletePortUI = CMonitorUI::DeletePortUI;

	return &ui;
}
