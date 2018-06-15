/* Copyright (c) <2003-2016> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "dgPhysicsStdafx.h"
#include "dgWorld.h"
#include "dgWorldPlugins.h"


	

dgWorldPluginList::dgWorldPluginList(dgMemoryAllocator* const allocator)
	:dgList<dgWorldPluginModulePair>(allocator)
	,m_currentPlugin(NULL)
{
}

dgWorldPluginList::~dgWorldPluginList()
{
}

void dgWorldPluginList::LoadPlugins()
{
	char plugInPath[2048];
	char rootPathInPath[2048];

#ifdef _MSC_VER
	GetModuleFileNameA(NULL, plugInPath, 256);
#endif

	for (dgInt32 i = dgInt32(strlen(plugInPath) - 1); i; i--) {
		if ((plugInPath[i] == '\\') || (plugInPath[i] == '/')) {
			plugInPath[i] = 0;
			break;
		}
	}
#ifdef _DEBUG
	strcat(plugInPath, "/newtonPlugins/debug");
#else
	strcat(plugInPath, "/newtonPlugins/release");
#endif
	sprintf(rootPathInPath, "%s/*.dll", plugInPath);


	dgWorld* const world = (dgWorld*) this;

	// scan for all plugins in this folder
	_finddata_t data;
	intptr_t handle = _findfirst(rootPathInPath, &data);
	if (handle != -1) {
		do {
			sprintf(rootPathInPath, "%s/%s", plugInPath, data.name);
			HMODULE module = LoadLibrary(rootPathInPath);

			if (module) {
				// get the interface function pointer to the Plug in classes
				InitPlugin initModule = (InitPlugin)GetProcAddress(module, "GetPlugin");
				if (initModule) {
					dgWorldPlugin* const plugin = initModule(world, GetAllocator ());
					dgWorldPluginModulePair entry(plugin, module);
					Append(entry);
				} else {
					FreeLibrary(module);
				}
			}

		} while (_findnext(handle, &data) == 0);

		_findclose(handle);
	}
}

void dgWorldPluginList::UnloadPlugins()
{
	dgWorldPluginList& pluginsList = *this;
	for (dgWorldPluginList::dgListNode* node = pluginsList.GetFirst(); node; node = node->GetNext()) {
		HMODULE module = (HMODULE)node->GetInfo().m_module;
		FreeLibrary(module);
	}
}

dgWorldPluginList::dgListNode* dgWorldPluginList::GetCurrentPlugin()
{
	return m_currentPlugin;
}

dgWorldPluginList::dgListNode* dgWorldPluginList::GetFirstPlugin()
{
	dgWorldPluginList& list = *this;
	return list.GetFirst();
}

dgWorldPluginList::dgListNode* dgWorldPluginList::GetNextPlugin(dgListNode* const plugin)
{
	return plugin->GetNext();
}

const char* dgWorldPluginList::GetPluginId(dgListNode* const pluginNode)
{
	dgWorldPluginModulePair entry(pluginNode->GetInfo());
	dgWorldPlugin* const plugin = entry.m_plugin;
	return plugin->GetId();
}

void dgWorldPluginList::SelectPlugin(dgListNode* const plugin)
{
	m_currentPlugin = plugin;
}

