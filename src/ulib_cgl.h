#pragma once

#include "chuck_def.h"
#include "chuck_dl.h"


#include <condition_variable>
#include <vector>

t_CKBOOL init_chugl(Chuck_DL_Query * QUERY);

// foward decls =========================================
class Scene;
class SceneGraphObject;
class SceneGraphCommand;


// exports =========================================
// DLL_QUERY cgl_query(Chuck_DL_Query* QUERY);

// class definitions ===============================

enum class CglEventType {
	CGL_FRAME,			// triggered at start of each window loop (assumes only 1 window)
	CGL_UPDATE,			// triggered after renderer swaps command queue double buffer (assumes only 1 renderer)
	CGL_WINDOW_RESIZE	// TODO: trigger on window resize
};

// storage class for thread-safe events
// all events are broadcast on the shared m_event_queue
// (as opposed to creating a separate circular buff for each event like how Osc does it)
class CglEvent  
{
public:
	CglEvent(Chuck_Event* event, Chuck_VM* vm, CK_DL_API api, CglEventType event_type);
	~CglEvent();
	// void wait(Chuck_VM_Shred* shred);
	void Broadcast();
	static void Broadcast(CglEventType event_type);
	static std::vector<CglEvent*>& GetEventQueue(CglEventType event_type);

	static CBufferSimple* s_SharedEventQueue;

private:
	Chuck_VM* m_VM;
	Chuck_Event* m_Event;
	CK_DL_API m_API;
	CglEventType m_EventType;

	// event queues, shared by all events
	static std::vector<CglEvent*> m_FrameEvents;
	static std::vector<CglEvent*> m_UpdateEvents;  // not used for now, will be used to support multiple windows
	static std::vector<CglEvent*> m_WindowResizeEvents;
};

// catch all class for exposing rendering API, will refactor later
class CGL
{
public:
	static void Render();  // called from chuck side to say update is done! begin deepcopy
	static void WaitOnUpdateDone();
	static bool shouldRender;
	static std::mutex GameLoopLock;
	static std::condition_variable renderCondition;

	static Scene mainScene;
	static SceneGraphObject mainCamera;
	// static PerspectiveCamera mainCamera;

	// static Chuck_Event s_UpdateChuckEvent;  // event used for waiting on update()

public: // command queue methods
	static void SwapCommandQueues();
	static void FlushCommandQueue(Scene& scene, bool swap);
	static void PushCommand(SceneGraphCommand * cmd);

private: // attributes
	// command queues 
	// the commands need to be executed before renderering...putting here for now
	static std::vector<SceneGraphCommand*> m_ThisCommandQueue;
	static std::vector<SceneGraphCommand*> m_ThatCommandQueue;
	static bool m_CQReadTarget;  // false = this, true = that
	// command queue lock
	static std::mutex m_CQLock; // only held when 1: adding new command and 2: swapping the read/write queues
private:
	static inline std::vector<SceneGraphCommand*>& GetReadCommandQueue() { 
		return m_CQReadTarget ? m_ThatCommandQueue : m_ThisCommandQueue; 
	}
	// get the write target command queue
	static inline std::vector<SceneGraphCommand*>& GetWriteCommandQueue() {
		return m_CQReadTarget ? m_ThisCommandQueue : m_ThatCommandQueue;
	}
};